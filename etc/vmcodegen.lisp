#!/usr/bin/sbcl --noinform

(defvar insns nil)

(setf
 insns
 '((:name nop :arity nil)
   (:name immval :arity 1)
   (:name immval_num :arity 1)
   (:name immval_char :arity 1)
   (:name push :arity nil)
   (:name pop :arity nil)
   (:name pushi :arity 1)
   (:name pushi_num :arity 1)
   (:name pushi_char :arity 1)
   (:name lref :arity 1)
   (:name lref_push :arity 1)
   (:name lref_look :arity 1)
   (:name lset :arity 1)
   (:name linc :arity 1)
   (:name ldefun :arity 1)
   (:name gref :arity 1)
   (:name gref_push :arity 1)
   (:name gset :arity 1)
   (:name ginc :arity 1)
   (:name gdefun :arity 1)

   (:name add2 :arity nil)
   (:name lref_add2 :arity 1)
   (:name add2_num :arity nil)
   (:name add2_str :arity nil)
   (:name sub2 :arity nil)
   (:name lref_sub2 :arity 1)
   (:name sub2_num :arity nil)
   (:name mul2 :arity nil)
   (:name lref_mul2 :arity 1)
   (:name mul2_num :arity nil)
   (:name div2 :arity nil)
   (:name lref_div2 :arity 1)
   (:name div2_num :arity nil)
   (:name and2 :arity nil)
   (:name or2 :arity nil)
   (:name not :arity nil)
   (:name bitand2 :arity nil)
   (:name bitor2 :arity nil)
   (:name bitxor2 :arity nil)
   (:name bitinv :arity nil)
   (:name rshift2 :arity nil)
   (:name lshift2 :arity nil)

   (:name eql2 :arity nil)
   (:name neql2 :arity nil)
   (:name numlt2 :arity nil)
   (:name numleq2 :arity nil)
   (:name numgt2 :arity nil)
   (:name numgeq2 :arity nil)
   (:name is2 :arity nil)
   (:name in2 :arity nil)

   (:name goto :arity 1)
   (:name gotoif :arity 1)
   (:name gotoifnot :arity 1)
   (:name self_call :arity 1)
   (:name vref_call :arity nil)
   (:name gref_call :arity 1)
   (:name lref_call :arity 1)
   (:name push_env :arity nil)
   (:name ret :arity nil)
   (:name end :erity nil)

   (:name mklist :arity 1)
   (:name mktuple :arity 1)
   (:name mkdict :arity 1)
   (:name len :arity nil)
   (:name getitem :arity nil)
   (:name setitem :arity nil)
   (:name delitem :arity nil)

   (:name print :arity nil)
   (:name printstr :arity nil)

   (:name thread_fork :arity nil)
   (:name thread_join :arity nil)
   ))

(asdf:operate 'asdf:load-op :cl-emb :verbose nil)
; (require :cl-emb)

(defvar i 0)
(emb:register-emb 
 "header"
 "// -*- mode: C++; coding: utf-8; tab-width: 4; indent-tabs-mode: nil -*-

/* DO NOT EDIT THIS FILE!
 * this file is automatically generated by etc/vmcodegen.lisp
 * ./vmcodegen.lisp define [output path]
 */
<% (defvar i 0) %>
typedef enum {<% @loop insns %>
    VM_<% @var name %> = <%= i %>,<% (setf i (+ i 1)) %><% @endloop %>
} vm_insn_type;

#define VM_INSNS_QTY <% @var qty %>

<% @loop insns %>
#define <% @var name %>(<% @if arity %>arg, <% @endif %>pos) add(VM_<% @var name %><% @if arity %>, arg<% @endif %>, pos)<% @endloop %>
")

(emb:register-emb 
 "dispatchtable"
 "// -*- mode: C++; coding: utf-8; tab-width: 4; indent-tabs-mode: nil -*-

/* DO NOT EDIT THIS FILE!
 * this file is automatically generated by etc/vmcodegen.lisp
 * ./vmcodegen.lisp define [output path]
 */

void * dispatchtable[<% @var qty %>] = {<% @loop insns %>
dispatchtable[VM_<% @var name %>] = && VM_<% @var name %>_CASE,<% @endloop %>
};
")

(emb:register-emb
 "printer"
 "// -*- mode: C++; coding: utf-8; tab-width: 4; indent-tabs-mode: nil -*-

/* DO NOT EDIT THIS FILE!
 * this file is automatically generated by etc/vmcodegen.lisp
 * ./vmcodegen.lisp print [output path]
 */
    vector<vm_inst>::iterator itr;
    int i;
    for(itr = this->insns.begin(), i = 0; itr != this->insns.end(); itr++, i++){
        switch((*itr).type){
<% @loop insns %>
        case VM_<% @var name %>:
            printf(\"%4d: VM_<% @var name %>(<% @if arity %> %s <% @endif %>) --%s@%d\\n\", i<% @if arity %>, operand_str((*itr).type, (py_val_t)(*itr).operand, genv).c_str()<% @endif %> , (*itr).p->filename.c_str(), (*itr).p->line_no);
            break;<% @endloop %>
        }
    }
")

(emb:register-emb
 "runcase"
 "
<% @loop insns %>
    case VM_<% @var name %>:
    {
        <% @if arity %>// one operand<% @else %>// no operand<% @endif %>

    }
    break;<% @endloop %>
")

(defun main (args)
  (if (null (cdr args))
      1
    (let ((output
	   (cond
	    ((equal "insns" (cadr args))
	     (emb:execute-emb "header" :env `(:insns ,insns :qty ,(length insns))))
	    ((equal "dispatchtable" (cadr args))
	     (emb:execute-emb "dispatchtable" :env `(:insns ,insns
						     :qty ,(length insns))))
	    ((equal "print" (cadr args))
	     (emb:execute-emb "printer" :env `(:insns ,insns)))
	    ((equal "runcase" (cadr args))
	     (emb:execute-emb "runcase" :env `(:insns ,insns)))
	    )))
      (format t output)))
  0)

(main (cdr *posix-argv*))