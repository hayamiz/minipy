;; This buffer is for notes you don't want to save, and for Lisp evaluation.
;; If you want to create a file, visit that file with C-x C-f,
;; then enter the text in that file's own buffer.

(setq blank-width 60)
(setq success-message "[ \\(^O^)/ success ]")
(setq accepted-message "[ \\(^O^)/ accepted ]")

(defmacro minipy-insert-with-separator (separator &rest body)
  `(let ((p (point)))
     (when (string-match ,separator (buffer-string) p)
       (delete-region (point) (+ 1 (match-end 0)))
       ,@body
       )
     (insert ,(concat "\n" separator))
     (goto-char p)))

(defun minipy-insert-pretty-print()
  (minipy-insert-with-separator
   "// ---- pretty print ----"
   (dolist
       (tok '(
	      rparen lparen lbrace rbrace
	      rbracket lbracket 
	      period comma colon eq_eq eq
	      neq geq rshift gt leq lshift
	      lt mul div mod tilde amp plus
	      minus bar xor 
	      kw_or kw_and kw_not kw_is kw_in kw_break kw_continue
	      kw_pass kw_return kw_del kw_print kw_global
	      kw_if kw_elif kw_else kw_for kw_while
	      kw_def literal_int literal_float
	      literal_string id newline indent dedent
	      none
	      eof
	      unrecognized
	      ))
     (insert
      (cond
       ((find tok '(id literal_string literal_int literal_float unrecognized))
	(concat"\ncase "
	       (upcase (format "tok_%s" tok))
	       ": cout << "
	       (upcase (format "\"tok_%s\" << \" (\"" tok))
	       " << this->cur_token_string << \")\""
	       " << endl; break;"))
       (t (concat"\ncase "
		 (upcase (format "tok_%s" tok))
		 ": cout << "
		 (upcase (format "\"tok_%s\"" tok))
		 " << endl; break;")))))))

(defun minipy-insert-keywords-init ()
  (interactive)
  (dolist
      (kw '(break
	    continue pass return del
	    print global if elif else
	    for while def or and not is in))
    (insert
     (concat
      (format "\nthis->keywords[\"%s\"] = " kw)
      (upcase (format "tok_kw_%s;" kw))))))

(defun minipy-insert-tokenizer-test ()
  (minipy-insert-with-separator
   "# ---- tokenizer test end ----"
   (progn
     (insert "\n\t@echo \"** Tokenizer normal test **\"")
     (dolist (target '(1 2 3 4 5 6))
       (insert
	(concat
	 (format "\n\t@echo \"Tokenizer test: prob_%d.py\"" target)
	 (format "\n\t@./test_tokenizer testdata-tokenizer/prob_%d.py | diff -uB - ./testdata-tokenizer/ans_%d" target target target)
	 (format "\n\t@perl -e \"print ' ' x %d;\"" blank-width)
	 (format "\n\t@echo \"%s\"" success-message)
	 )))
     (insert "\n\t@echo \"** Tokenizer ABNORMAL test **\"")
     (dolist (target '(1 2 3 4))
       (insert
	(concat
	 (format "\n\t@echo \"Tokenizer abnormal test (MUST fail): err_prob_%d.py\"" target)
	 (format "\n\t@-./test_tokenizer testdata-tokenizer/err_prob_%d.py 1> /dev/null" target)
	 ))))
     ))

(defun minipy-insert-expr-constructor ()
  (interactive)
  (dolist (target '((string var var)
		    (int literal_int lit_i)
		    (double literal_float lit_f)
		    (string literal_string lit_s)
		    (void none)
		    (ExprList display_tuple disp)
		    (ExprList display_list disp)
		    (ExprList display_dict disp)
		    (Expr paren paren)
		    (ExprOperator infix_operator op)
		    (ExprOperator prefix_operator op)
		    (ExprAttref attref atr)
		    (ExprList subscript sub)
		    (ExprCall call call)))
    (insert
     (concat
      (cond ((eq (cadr target) 'none)
	     (format "\n    static Expr make_%s(SrcPos p){" (cadr target)))
	    ((eq (cadr target) 'infix_operator)
	     (format "\n    static Expr make_%s(token_kind_t op, Expr a, Expr b, SrcPos p){" (cadr target)))
	    ((eq (cadr target) 'prefix_operator)
	     (format "\n    static Expr make_%s(token_kind_t op, Expr a, SrcPos p){" (cadr target)))
	    (t
	     (format "\n    static Expr make_%s(%s %s, SrcPos p){" (cadr target) (car target) (cadr target))))
      (format "\n        Expr expr(expr_kind_%s, p);" (cadr target))
      (cond ((find (car target) '(int double))
	     (format "\n        expr.u.%s = %s;" (caddr target) (cadr target)))
	    (t 
	     (format "\n        expr.u.%s = new %s(%s);" (caddr target) (car target) (cadr target))))
      (format "\n        return expr;")
      (format "\n    };")))))


(defun minipy-insert-parser-test()
  (minipy-insert-with-separator
   "# ---- end of minipy-insert-parser-test ----"
   (let ((sources
	  '(./testdata-parser/basics/big_list.py
	    ./testdata-parser/basics/test_dict.py
	    ./testdata-parser/basics/test_tuple.py
	    ./testdata-parser/basics/test_list.py
	    ./testdata-parser/basics/test_string.py
	    ./testdata-parser/basics/test_number.py
	    ./testdata-parser/basics/big_dict.py
	    ./testdata-parser/simple/integral.py
	    ./testdata-parser/simple/gcd.py
	    ./testdata-parser/simple/fib.py
	    ./testdata-parser/simple/fact.py
	    ./testdata-parser/full/qsort.py
	    ./testdata-parser/full/mst.py
	    ./testdata-parser/full/nbody.py
	    ./testdata-parser/full/sor.py
	    ./testdata-parser/full/graph.py
	    ./testdata-parser/graphics/taylor.py
	    ./testdata-parser/graphics/drawmap.py
	    ./testdata-parser/graphics/drawgraph.py
	    ./testdata-parser/graphics/draw3D.py
	    ./testdata-parser/graphics/newton.py
	    )))
       (dolist (source sources)
	 (insert
	  (concat
	   (format "\n\t@echo \"Parser test: %s\"" source)
	   (format "\n\t@./test_parser %s > %s.parsed" source source)
	   (format "\n\t@./test_parser %s.parsed | diff -uB - %s.parsed" source source)
	   (format "\n\t@echo \"%s%s\"" (make-string 60 ? ) success-message)
	   (format "\n\t@rm %s.parsed" source)
	   )))
       )))

(defun minipy-insert-operator-string-map ()
  (minipy-insert-with-separator
   "// ---- operator string map ----"
   (let ((map-data
	  '((eq "=") (eq_eq "==") (neq "!=") (gt ">")
	    (geq ">=") (lt "<") (leq "<=") (plus "+")
	    (minus "-") (mul "*") (div "/") (mod "%")
	    (tilde "~") (lshift "<<") (rshift ">>") (xor "^")
	    (amp "&") (bar "|") (kw_or "or") (kw_and "and")
	    (kw_not "not") (kw_is "is") (kw_in "in") (kw_is_not "is not")
	    (kw_not_in "not in") (colon ":")
	    )))
     (dolist (data map-data)
       (insert
	(format "\n        operator_map[%s] = \"%s\";"
		(upcase (format "tok_%s" (car data)))
		(cadr data)))))))

(defun minipy-insert-bootstrap ()
  (minipy-insert-with-separator
   "// ---- bootstrap ----"
   (let ((native-funcs
	  '((is_int 1)
	    (is_float 1)
	    (is_string 1)
	    (is_dict 1)
	    (is_list 1)
	    (is_tuple 1)
	    (is_ifun 1)
	    (is_nfun 1)

	    (add_int 2)
	    (sub_int 2)
	    (mul_int 2)
	    (div_int 2)
	    (mod_int 2)
	    (cmp_int 2)
	    (eq_int 2)
	    (ne_int 2)
	    (gt_int 2)
	    (ge_int 2)
	    (lt_int 2)
	    (le_int 2)
	    (invert_int 1)
	    (and_int 2)
	    (or_int 2)
	    (xor_int 2)
	    (lshift_int 2)
	    (rshift_int 2)
	    
	    (add_float 2)
	    (sub_float 2)
	    (mul_float 2)
	    (div_float 2)
	    (cmp_float 2)
	    
	    (len_string 1)
	    (getitem_string 2)
	    (add_string 2)
	    (eq_string 2)
	    
	    (len_tuple 1)
	    (getitem_tuple 2)
	    (add_tuple 2)
	    (hasitem_tuple 2)
	    
	    (len_list 1)
	    (getitem_list 2)
	    (append_list 2)
	    (setitem_list 3)
	    (delitem_list 2)
	    (pop_list 1)
	    (hasitem_list 2)
	    
	    (len_dict 1)
	    (has_key_dict 2)
	    (getitem_dict 2)
	    (setitem_dict 3)
	    (delitem_dict 2)
	    (key_dict 1)
	    (values_dict 1)
	    (items_dict 1)
	    
	    (repr_int 1)
	    (repr_float 1)
	    (repr_ifun 1)
	    (repr_nfun 1)
	    
	    (print_string 1)
	    (print_ 1)
	    
	    (not_ 1)

	    (itof 1)

	    (append 2)
	    (add_list 2)
	    (range -1)
	    )))
     (dolist (fun-data native-funcs)
       (insert
	(format "\n    genv.set(\"%s\", Py_val::mk_nfun(\"%s\", %d, native::%s));"
		(car fun-data) (car fun-data) (cadr fun-data) (car fun-data)))))))
