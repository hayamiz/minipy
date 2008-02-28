#!/usr/bin/sbcl --noinform

(require 'asdf)
(asdf:operate 'asdf:load-op 'cl-emb :verbose nil)
(asdf:operate 'asdf:load-op 'cl-fad :verbose nil)

;;;
;;; Constants
;;;

(defvar *working-dir* #P"/home/haya/python-enshu/trunk/tests/")

(defvar *src-root-dir* #P"../"
  "Relative path from *WORKING-DIR*")

(defvar *template-dir* #P"templates/")

(defvar *test-suite-dir* #P"suites/")

;;;
;;; Low level functions
;;;

(set-dispatch-macro-character
 #\# #\t
 #'(lambda (stream char1 char2)
     (declare (ignore char1 char2))
     (loop for c = (read-char stream)
	   while (not (eq #\space c)))
     (let* ((testcase-name (read stream))
	    (env (read stream))
	    (terminal-str "")
	    (end-mark "#end"))
       (if (not (symbolp testcase-name))
	   (error "Invalid testcase name.")
	 (let* ((raw-ret ; read until '#end' appears
		 (with-output-to-string 
		   (out)
		   (loop for c = (read-char stream nil #\space)
			 while (not (string-equal terminal-str end-mark)) do
			 (write-char c out)
			 (setq terminal-str
			       (if (< (length terminal-str) (length end-mark))
				   (concatenate 'string terminal-str (string c))
				 (concatenate
				  'string
				  (subseq terminal-str
					  (- (length terminal-str)
					     (- (length end-mark) 1))
				  (length terminal-str))
				  (string c)))))))
		(ret-str
		 (subseq raw-ret
			 0 (- (length raw-ret) (length end-mark)))))
	   `(add-test-case
	     (make-test-case :name
			     ,(string-downcase (string testcase-name))
			     :code
			     ,ret-str
			     :env
			     (list ,@env)))
;; 	   `(quote (:name ,testcase-name
;; 		    :code ,ret-str
;; 		    :env ,env))
	   )))))

(set-dispatch-macro-character
 #\# #\f
 #'(lambda (stream char1 char2)
     (declare (ignore char1 char2))
     (loop for c = (read-char stream)
	   while (not (or (eq #\space c)
			  (eq #\newline c)))) ; discard remaining chars
     (let ((terminal-str "")
	   (end-mark "#end"))
       (let* ((raw-ret			; read until '#end' appears
	       (with-output-to-string 
		 (out)
		 (loop for c = (read-char stream nil #\space)
		       while (not (string-equal terminal-str end-mark)) do
		       (write-char c out)
		       (setq terminal-str
			     (if (< (length terminal-str) (length end-mark))
				 (concatenate 'string terminal-str (string c))
			       (concatenate
				'string
				(subseq terminal-str
					(- (length terminal-str)
					   (- (length end-mark) 1))
					(length terminal-str))
				(string c)))))))
	      (ret-str
	       (subseq raw-ret
		       0 (- (length raw-ret) (length end-mark)))))
	 `(if (not *current-test-suite*)
	      (error "There's no current test-suite.")
	    (setf (test-suite-fixtures *current-test-suite*) ,ret-str))
	 ))))

(set-dispatch-macro-character
 #\# #\r
 #'(lambda (stream char1 char2)
     (declare (ignore char1 char2))
     (loop for c = (read-char stream)
	   while (not (or (eq #\space c)
			  (eq #\newline c)))) ; discard remaining chars
     (let* ((requires nil))
       (with-input-from-string
	(in (with-output-to-string
	      (s)
	      (loop for c = (read-char stream)
		    while (not (eq c #\newline)) do ; read until newline
		    (princ c s))))
	(loop for exp = (read in nil nil)
	      while exp do
	      (push (string-downcase (string exp)) requires)))
       `(if (not *current-test-suite*)
	    (error "There's no current test-suite.")
	  (setf (test-suite-requires *current-test-suite*) (quote ,requires)))
       )))

(defvar *current-test-suite* nil)
(defvar *test-suites* (make-hash-table :test 'equal))

(defstruct test-suite
  (name "foo" :read-only t :type string)
  (testcases (make-hash-table :test 'equal) :type hash-table)
  (fixtures "" :type string)
  (requires nil :type list))

(defstruct test-case
  (name "bar" :read-only t :type string)
  (code "" :read-only t :type string)
  (env nil :read-only t :type list))

(defmethod start-test-suite ((name string))
  (let ((new-suite (make-test-suite :name name)))
    (setf (gethash name *test-suites*) new-suite)
    (setf *current-test-suite* new-suite)))

(defmacro end-test-suite ()
  `(setf *current-test-suite* nil))

(defmethod add-test-case ((test-case test-case))
  (if (not *current-test-suite*)
      (error "There's no current test-suite.")
    (setf (gethash (test-case-name test-case)
		   (test-suite-testcases *current-test-suite*))
	  test-case)))

(defun read-file (file)
  (with-output-to-string
    (out)
    (with-open-file
     (in file :direction :input)
     (loop for c = (read-char in nil)
	   while c do (write-char c out)))))

(defun temp-pathname (name)
  (reduce 'merge-pathnames
	  (list (concatenate 'string name ".lcpp") *template-dir* *working-dir*)))

(defun suite-pathname (name)
  (reduce 'merge-pathnames
	  (list (concatenate 'string name ".suite") *test-suite-dir* *working-dir*)))

(defun write-file (file string)
  (with-open-file (s file :direction :output :if-exists :supersede)
		  (write-string string s)))

;;;
;;; c++ code generators
;;;

(defun setup-emb ()
  (mapcar
   (lambda (temp-pathname)
     (emb:register-emb (pathname-name temp-pathname)
		       (read-file temp-pathname)))
   (remove-if-not
    (lambda (pathname)
      (equal "lcpp" (pathname-type pathname)))
    (fad:list-directory (merge-pathnames *template-dir* *working-dir*)))))

(defun load-suites ()
  (setf *test-suites* (make-hash-table :test 'equal))
  (mapcar
   (lambda (suite-name)
     (start-test-suite suite-name)
     (load (suite-pathname suite-name))
     (end-test-suite)
     t)
   (api-list-test-suites)))

(defun gen-test-runner (suites)
  (if (null suites)
      (setf suites (api-list-test-suites)))
  (write-file 
   (merge-pathnames "testRunner.cpp" *working-dir*)
   (emb:execute-emb "testRunner"
		    :env `(:test-suites
			   ,(mapcar
			     (lambda (suite-name)
			       `(:name ,suite-name))
			     suites))))
  )

(defun gen-test-suite (test-suite)
  (write-file
   (merge-pathnames (concatenate 'string (test-suite-name test-suite) "Test.cpp")
		    *working-dir*)
   (test-suite-code test-suite))
  (write-file
   (merge-pathnames (concatenate 'string (test-suite-name test-suite) "Test.hpp")
		    *working-dir*)
   (test-suite-header test-suite))
  t)

(defun gen-makefile ()
  (write-file
   (merge-pathnames "makefile" *working-dir*)
   (emb:execute-emb "makefile"
		    :env
		    `(:suites
		      ,(mapcar (lambda (suite)
				 `(:name ,suite
				   :requires
				   ,(mapcar
				     (lambda (req)
				       `(:name ,req))
				     (test-suite-requires (gethash suite
								   *test-suites*)))
				   ))
			       (api-list-test-suites))))))

(defmethod test-suite-code ((test-suite test-suite))
  (emb:execute-emb
   "test-suite"
   :env (list :suite-name (test-suite-name test-suite)
	      :test-cases (let ((ret ()))
			    (maphash (lambda (key test-case)
				       (push 
					(list :suite-name (test-suite-name test-suite)
					      :name key
					      :code (test-case-code test-case))
					ret))
				     (test-suite-testcases test-suite))
			    ret))))

(defmethod test-suite-header ((test-suite test-suite))
  (emb:execute-emb
   "test-suite-header"
   :env (list :suite-name (test-suite-name test-suite)
	      :test-cases (let ((ret ()))
			    (maphash (lambda (key test-case)
				       (push `(:name ,key) ret))
				     (test-suite-testcases test-suite))
			    ret)
	      :fixtures (test-suite-fixtures test-suite))))

;;;
;;; User api for defining testsuite
;;;

(defun api-list-test-suites ()
  (remove
   "testRunner"
   (mapcar
    #'pathname-name
    (remove-if-not
     (lambda (name)
       (equal "suite" (pathname-type name)))
     (fad:list-directory (merge-pathnames *test-suite-dir* *working-dir*))))
   :test #'equal))

(defvar *lvals* (make-hash-table :test 'eq))

(defun api-add-lval (name value)
  (if (not (symbolp name))
      (error "NAME must be SYMBOL.")
    (setf (gethash name *lvals*) value)))

;;;
;;; Entry point
;;;

(load-suites)
(setup-emb)

(emb:register-emb
 "makefile"
 "
TEST_OBJ_FILES = testRunner.o

CPP_FLAGS = -g -Wall -ansi

all :
	./generate.lisp all 2> /dev/null
	make test_all.out
	./test_all.out

<% @loop suites %>
<% @var name %> :
	./generate.lisp runner <% @var name %> 2> /dev/null
	./generate.lisp suite <% @var name %> 2> /dev/null
	@make test_<% @var name %>.out
	./test_<% @var name %>.out

test_<% @var name %>.out : <% @var name %>Test.o ../<% @var name %>.o <% @loop requires %>../<% @var name %>.o <% @endloop %> ${TEST_OBJ_FILES}
	cd ../ ; make
	g++ ${CPP_FLAGS} ${TEST_OBJ_FILES} <% @var name%>Test.o ../<% @var name%>.o <% @loop requires %>../<% @var name %>.o <% @endloop %> -o test_<% @var name %>.out -lcppunit
<% @endloop %>

test_all.out: <% @loop suites %><% @var name%>Test.o ../<% @var name%>.o <% @loop requires %>../<% @var name %>.o <% @endloop %><% @endloop %> ${TEST_OBJ_FILES}
	g++ ${CPP_FLAGS} <% @loop suites %><% @var name%>Test.o ../<% @var name%>.o <% @loop requires %>../<% @var name %>.o <% @endloop %><% @endloop %> -o test_all.out -lcppunit

testRunner.o : testRunner.cpp
	g++ ${CPP_FLAGS} -c testRunner.cpp

%.o : %.cpp %.hpp 
	g++ ${CPP_FLAGS} -c $<

clean :
	rm *.o *.out <% @loop suites %><% @var name %>Test.hpp <% @var name %>Test.cpp <% @endloop %>

file :
	./generate.lisp makefile 2> /dev/null
")

(defun main (args)
  (if (null (cdr args))
      1
    (progn
      (cond
       ((equal (cadr args) "makefile")
	(gen-makefile))
       ((equal (cadr args) "runner")
	(gen-test-runner (cddr args)))
       ((equal (cadr args) "suite")
	(mapcar
	 (lambda (suite)
	   (gen-test-suite (gethash suite *test-suites*)))
	 (remove-if-not (lambda (name) (gethash name *test-suites* nil))
			(cddr args))))
       (t
	(gen-makefile)
	(gen-test-runner (api-list-test-suites))
	(mapcar 
	 (lambda (suite)
	   (gen-test-suite (gethash suite *test-suites*)))
	 (remove-if-not (lambda (name) (gethash name *test-suites* nil))
			(api-list-test-suites)))))
      0)))

(main (cdr *posix-argv*))

;; #testcase readerMacroTest ()

;;     stack<Stack_trace_entry> bt; // dummy stack trace
;;     SrcPos pos; // dummy position

;;     this->py_int_val = Py_val::mk_int(0);
;;     CPPUNIT_ASSERT(Py_val::is_int(this->py_int_val));
;;     CPPUNIT_ASSERT_EQUAL(0, Py_val::py_val_int(py_int_val, bt, pos));

;;     // Max : 1073741823 = 2^30 - 1
;;     this->py_int_val = Py_val::mk_int(1073741823);
;;     CPPUNIT_ASSERT(Py_val::is_int(this->py_int_val));
;;     CPPUNIT_ASSERT_EQUAL(1073741823, Py_val::py_val_int(py_int_val, bt, pos));

;;     // Min : -1073741824 = - 2^30
;;     this->py_int_val = Py_val::mk_int(-1073741824);
;;     CPPUNIT_ASSERT(Py_val::is_int(this->py_int_val));
;;     CPPUNIT_ASSERT_EQUAL(-1073741824, Py_val::py_val_int(py_int_val, bt, pos));
;; #end
