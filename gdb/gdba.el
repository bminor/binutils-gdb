(defmacro gud (form)
  (` (save-excursion (set-buffer "*gud-a.out*") (, form))))

(defun dbug (foo &optional fun)
  (save-excursion
    (set-buffer (get-buffer-create "*trace*"))
    (goto-char (point-max))
    (insert "***" (symbol-name foo) "\n")
    (if fun
	(funcall fun))))


;;; gud.el --- Grand Unified Debugger mode for gdb, sdb, dbx, or xdb
;;;            under Emacs

;; Author: Eric S. Raymond <esr@snark.thyrsus.com>
;; Maintainer: FSF
;; Version: 1.3
;; Keywords: unix, tools

;; Copyright (C) 1992, 1993 Free Software Foundation, Inc.

;; This file is part of GNU Emacs.

;; GNU Emacs is free software; you can redistribute it and/or modify
;; it under the terms of the GNU General Public License as published by
;; the Free Software Foundation; either version 2, or (at your option)
;; any later version.

;; GNU Emacs is distributed in the hope that it will be useful,
;; but WITHOUT ANY WARRANTY; without even the implied warranty of
;; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;; GNU General Public License for more details.

;; You should have received a copy of the GNU General Public License
;; along with GNU Emacs; see the file COPYING.  If not, write to
;; the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.

;;; Commentary:

;; The ancestral gdb.el was by W. Schelter <wfs@rascal.ics.utexas.edu>
;; It was later rewritten by rms.  Some ideas were due to Masanobu. 
;; Grand Unification (sdb/dbx support) by Eric S. Raymond <esr@thyrsus.com>
;; The overloading code was then rewritten by Barry Warsaw <bwarsaw@cen.com>,
;; who also hacked the mode to use comint.el.  Shane Hartman <shane@spr.com>
;; added support for xdb (HPUX debugger).

;; Cygnus Support added support for gdb's --annotate=2.

;;; Code:

(require 'comint)
(require 'etags)

;; ======================================================================
;; GUD commands must be visible in C buffers visited by GUD

(defvar gud-key-prefix "\C-x\C-a"
  "Prefix of all GUD commands valid in C buffers.")

(global-set-key (concat gud-key-prefix "\C-l") 'gud-refresh)
(global-set-key "\C-x " 'gud-break)	;; backward compatibility hack

;; ======================================================================
;; the overloading mechanism

(defun gud-overload-functions (gud-overload-alist)
  "Overload functions defined in GUD-OVERLOAD-ALIST.
This association list has elements of the form
     (ORIGINAL-FUNCTION-NAME  OVERLOAD-FUNCTION)"
  (mapcar
   (function (lambda (p) (fset (car p) (symbol-function (cdr p)))))
   gud-overload-alist))

(defun gud-massage-args (file args)
  (error "GUD not properly entered."))

(defun gud-marker-filter (str)
  (error "GUD not properly entered."))

(defun gud-find-file (f)
  (error "GUD not properly entered."))

;; ======================================================================
;; command definition

;; This macro is used below to define some basic debugger interface commands.
;; Of course you may use `gud-def' with any other debugger command, including
;; user defined ones.

;; A macro call like (gud-def FUNC NAME KEY DOC) expands to a form
;; which defines FUNC to send the command NAME to the debugger, gives
;; it the docstring DOC, and binds that function to KEY in the GUD
;; major mode.  The function is also bound in the global keymap with the
;; GUD prefix.

(defmacro gud-def (func cmd key &optional doc)
  "Define FUNC to be a command sending STR and bound to KEY, with
optional doc string DOC.  Certain %-escapes in the string arguments
are interpreted specially if present.  These are:

  %f	name (without directory) of current source file. 
  %d	directory of current source file. 
  %l	number of current source line
  %e	text of the C lvalue or function-call expression surrounding point.
  %a	text of the hexadecimal address surrounding point
  %p	prefix argument to the command (if any) as a number

  The `current' source file is the file of the current buffer (if
we're in a C file) or the source file current at the last break or
step (if we're in the GUD buffer).
  The `current' line is that of the current buffer (if we're in a
source file) or the source line number at the last break or step (if
we're in the GUD buffer)."
  (list 'progn
	(list 'defun func '(arg)
	      (or doc "")
	      '(interactive "p")
	      (list 'gud-call cmd 'arg))
	(if key
	    (list 'define-key
		  '(current-local-map)
		  (concat "\C-c" key)
		  (list 'quote func)))
	(if key
	    (list 'global-set-key
		  (list 'concat 'gud-key-prefix key)
		  (list 'quote func)))))

;; Where gud-display-frame should put the debugging arrow.  This is
;; set by the marker-filter, which scans the debugger's output for
;; indications of the current program counter.
(defvar gud-last-frame nil)

;; Used by gud-refresh, which should cause gud-display-frame to redisplay
;; the last frame, even if it's been called before and gud-last-frame has
;; been set to nil.
(defvar gud-last-last-frame nil)

;; All debugger-specific information is collected here.
;; Here's how it works, in case you ever need to add a debugger to the mode.
;;
;; Each entry must define the following at startup:
;;
;;<name>
;; comint-prompt-regexp
;; gud-<name>-massage-args
;; gud-<name>-marker-filter
;; gud-<name>-find-file
;;
;; The job of the massage-args method is to modify the given list of
;; debugger arguments before running the debugger.
;;
;; The job of the marker-filter method is to detect file/line markers in
;; strings and set the global gud-last-frame to indicate what display
;; action (if any) should be triggered by the marker.  Note that only
;; whatever the method *returns* is displayed in the buffer; thus, you
;; can filter the debugger's output, interpreting some and passing on
;; the rest.
;;
;; The job of the find-file method is to visit and return the buffer indicated
;; by the car of gud-tag-frame.  This may be a file name, a tag name, or
;; something else.

;; ======================================================================
;; gdb functions

;;; History of argument lists passed to gdb.
(defvar gud-gdb-history nil)

(defun gud-gdb-massage-args (file args)
  (cons "--annotate=2" (cons file args)))


;;
;; In this world, there are gdb instance objects (of unspecified 
;; representation) and buffers associated with those objects.
;;

;; 
;; gdb-instance objects
;; 

(defun make-gdb-instance (proc)
  "Create a gdb instance object from a gdb process."
  (let ((instance (cons 'gdb-instance proc)))
    (save-excursion
      (set-buffer (process-buffer proc))
      (if (not (equal gdb-buffer-instance instance))
	  (progn
	    (mapcar 'make-variable-buffer-local gdb-instance-variables)
	    (setq gdb-buffer-instance instance) ; These are both...
	    (setq gdb-buffer-type 'gud)))) ; ...instance variables
    instance))

(defun gdb-instance-process (inst) (cdr inst))

(defvar gdb-instance-variables '()
  "A list of variables that are local to the gud buffer associated
with a gdb instance.") 

(defmacro def-gdb-variable
  (name accessor setter &optional default doc)
  (`
   (progn
     (defvar (, name) (, default) (, (or doc "undocumented")))
     (if (not (memq '(, name) gdb-instance-variables))
	 (setq gdb-instance-variables
	       (cons '(, name) gdb-instance-variables)))
     (, (and accessor
	     (`
	      (defun (, accessor) (instance)
		(let
		    ((buffer (gdb-get-instance-buffer instance 'gud)))
		  (and buffer
		       (save-excursion
			 (set-buffer buffer)
			 (, name))))))))
     (, (and setter
	     (`
	      (defun (, setter) (instance val)
		(let
		    ((buffer (gdb-get-instance-buffer instance 'gud)))
		  (and buffer
		       (save-excursion
			 (set-buffer buffer)
			 (setq (, name) val)))))))))))

(defmacro def-gdb-var (root-symbol &optional default doc)
  (let* ((root (symbol-name root-symbol))
	 (accessor (intern (concat "gdb-instance-" root)))
	 (setter (intern (concat "set-gdb-instance-" root)))
	 (var-name (intern (concat "gdb-" root))))
    (` (def-gdb-variable
	 (, var-name) (, accessor) (, setter)
	 (, default) (, doc)))))

(def-gdb-var buffer-instance nil
  "In an instance buffer, the buffer's instance.")

(def-gdb-var buffer-type nil
  "One of the symbols bound in gdb-instance-buffer-rules")

(def-gdb-var burst ""
  "A string of characters from gdb that have not yet been processed.")

(def-gdb-var input-queue ()
  "A list of high priority gdb command objects.")

(def-gdb-var idle-input-queue ()
  "A list of low priority gdb command objects.")

(def-gdb-var prompting nil
  "True when gdb is idle with no pending input.")

(def-gdb-var output-sink 'user
  "The disposition of the output of the current gdb command.")

(def-gdb-var current-item nil
  "The most recent command item sent to gdb.")

(def-gdb-var pending-triggers '()
  "A list of trigger functions that have run later than their output
handlers.")

(defun in-gdb-instance-context (instance form)
  "Funcall `form' in the gud buffer of `instance'"
  (save-excursion
    (set-buffer (gdb-get-instance-buffer instance 'gud))
    (funcall form)))

;; end of instance vars

;;
;; finding instances
;;

(defun gdb-proc->instance (proc)
  (save-excursion
    (set-buffer (process-buffer proc))
    gdb-buffer-instance))

(defun gdb-mru-instance-buffer ()
  "Return the most recently used (non-auxiliary) gdb gud buffer."
  (save-excursion
    (gdb-goto-first-gdb-instance (buffer-list))))

(defun gdb-goto-first-gdb-instance (blist)
  "Use gdb-mru-instance-buffer -- not this."
  (and blist
       (progn
	 (set-buffer (car blist))
	 (or (and gdb-buffer-instance
		  (eq gdb-buffer-type 'gud)
		  (car blist))
	     (gdb-find-first-gdb-instance (cdr blist))))))

(defun buffer-gdb-instance (buf)
  (save-excursion
    (set-buffer buf)
    gdb-buffer-instance))

(defun gdb-needed-default-instance ()
  "Return the most recently used gdb instance or signal an error."
  (let ((buffer (gdb-mru-instance-buffer)))
    (or (and buffer (buffer-gdb-instance buffer))
	(error "No instance of gdb found."))))

(defun gdb-instance-target-string (instance)
  "The apparent name of the program being debugged by a gdb instance.
For sure this the root string used in smashing together the gud 
buffer's name, even if that doesn't happen to be the name of a 
program."
  (in-gdb-instance-context
   instance
   (function (lambda () gud-target-name))))

;; More than one buffer can be associated with a gdb instance.
;;
;; Each buffer has a TYPE -- an atom that identifies the function
;; of that particular buffer.
;;
;; The usual gud interaction buffer is given the type `gud' and
;; is constructed specially.  
;;
;; Others are constructed by gdb-get-create-instance-buffer and 
;; named according to the rules set forth here:
;;

(defvar gdb-instance-buffer-rules-assoc
  '((gud error)				; gud buffers construct specially
    (gdb-partial-output-buffer
     gdb-partial-output-name
     )
    (gdb-registers-buffer
     gdb-registers-buffer-name)
    (gdb-breakpoints-buffer
     gdb-breakpoints-buffer-name)
    (gdb-frames-buffer
     gdb-frames-buffer-name)))


;;
;; gdb communications
;;

;; input: things sent to gdb
;;
;; Each instance has a high and low priority 
;; input queue.  Low priority input is sent only 
;; when the high priority queue is idle.
;;
;; The queues are lists.  Each element is either 
;; a string (indicating user or user-like input)
;; or a list of the form:
;;
;;    (INPUT-STRING  HANDLER-FN)
;;
;;
;; The handler function will be called from the 
;; partial-output buffer when the command completes.
;;
;; These lists are consumed tail first.
;;

(defun gdb-send (proc string)
  "A comint send filter for gdb.
This filter may simply queue output for a later time."
  (let ((instance (gdb-proc->instance proc)))
    (gdb-instance-enqueue-input instance (concat string "\n"))))

;; Note: Stuff enqueued here will be sent to the next prompt, even if it
;; is a query, or other non-top-level prompt.  To guarantee stuff will get
;; sent to the top-level prompt, currently it must be put in the idle queue.
(defun gdb-instance-enqueue-input (instance item)
  "Enqueue an input item (a string or a list) for a gdb instance."
  (if (gdb-instance-prompting instance)
      (progn
	(gdb-send-item instance item)
	(set-gdb-instance-prompting instance nil))
    (set-gdb-instance-input-queue
     instance
     (cons item (gdb-instance-input-queue instance)))))

(defun gdb-instance-dequeue-input (instance)
  (let ((queue (gdb-instance-input-queue instance)))
    (and queue
       (if (not (cdr queue))
	   (let ((answer (car queue)))
	     (set-gdb-instance-input-queue instance '())
	     answer)
	 (gdb-take-last-elt queue)))))

(defun gdb-instance-enqueue-idle-input (instance item)
  "Enqueue idle input (a string or a list) for a gdb instance."
  (if (gdb-instance-prompting instance)
      (progn
	(gdb-send-item instance item)
	(set-gdb-instance-prompting instance nil))
    (set-gdb-instance-idle-input-queue
     instance
     (cons item (gdb-instance-idle-input-queue instance)))))

(defun gdb-instance-dequeue-idle-input (instance)
  (let ((queue (gdb-instance-idle-input-queue instance)))
    (and queue
       (if (not (cdr queue))
	   (let ((answer (car queue)))
	     (set-gdb-instance-idle-input-queue instance '())
	     answer)
	 (gdb-take-last-elt queue)))))

(defun gdb-take-last-elt (l)
  "Don't use this in general."
  (if (cdr (cdr l))
      (gdb-take-last-elt (cdr l))
    (let ((answer (car (cdr l))))
      (setcdr l '())
      answer)))


;;
;; output -- things gdb prints to emacs
;;
;; GDB output is a stream interrupted by annotations.
;; Annotations can be recognized by their beginning
;; with \C-j\C-z\C-z<tag><opt>\C-j
;;
;; The tag is a string obeying symbol syntax.
;;
;; The optional part `<opt>' can be either the empty string
;; or a space followed by more data relating to the annotation.
;; For example, the SOURCE annotation is followed by a filename,
;; line number and various useless goo.  This data must not include
;; any newlines.
;;


(defun gud-gdb-marker-filter (string)
  "A gud marker filter for gdb."
  ;; Bogons don't tell us the process except through scoping crud.
  (let ((instance (gdb-proc->instance proc)))
    (gdb-output-burst instance string)))

(defvar gdb-annotation-rules
  '(("frames-invalid" gdb-invalidate-frames)
    ("breakpoints-invalid" gdb-invalidate-breakpoints)
    ("pre-prompt" gdb-pre-prompt)
    ("prompt" gdb-prompt)
    ("commands" gdb-subprompt)
    ("overload-choice" gdb-subprompt)
    ("query" gdb-subprompt)
    ("prompt-for-continue" gdb-subprompt)
    ("post-prompt" gdb-post-prompt)
    ("source" gdb-source)
    )
  "An assoc mapping annotation tags to functions which process them.")


(defun gdb-ignore-annotation (instance args)
  nil)

(defconst gdb-source-spec-regexp
  "\\(.*\\):\\([0-9]*\\):[0-9]*:[a-z]*:0x[a-f0-9]*")

(defun gdb-source (instance args)
  "Do not use this except as an annotation handler."
  (string-match gdb-source-spec-regexp args)
  ;; Extract the frame position from the marker.
  (setq gud-last-frame
	(cons
	 (substring args (match-beginning 1) (match-end 1))
	 (string-to-int (substring args
				   (match-beginning 2)
				   (match-end 2))))))

(defun gdb-prompt (instance ignored)
  "An annotation handler for `prompt'.
This sends the next command (if any) to gdb."
  (let ((sink (gdb-instance-output-sink instance)))
    (cond
     ((eq sink 'user) t)
     ((eq sink 'post-emacs)
      (set-gdb-instance-output-sink instance 'user))
     ((or (eq sink 'emacs)
	  (eq sink 'pre-emacs))
      (set-gdb-instance-output-sink instance 'user)
      (error "Phase error in gdb-prompt (got %s)" sink))
     (t (set-gdb-instance-output-sink instance 'user))))
  (let ((highest (gdb-instance-dequeue-input instance)))
    (if highest
	(gdb-send-item instance highest)
      (let ((lowest (gdb-instance-dequeue-idle-input instance)))
	(if lowest
	    (gdb-send-item instance lowest)
	  (set-gdb-instance-prompting instance t))))))

(defun gdb-subprompt (instance ignored)
  "An annotation handler for non-top-level prompts."
  (let ((highest (gdb-instance-dequeue-input instance)))
    (if highest
	(gdb-send-item instance highest)
      (set-gdb-instance-prompting instance t))))

(defun gdb-send-item (instance item)
  (dbug 'sending
	(function
	 (lambda ()
	   (insert (format "%s\n" item)))))
  (set-gdb-instance-current-item instance item)
  (if (stringp item)
      (progn
	(set-gdb-instance-output-sink instance 'user)
	(process-send-string (gdb-instance-process instance)
			     item))
    (progn
      (gdb-clear-partial-output instance)
      (set-gdb-instance-output-sink instance 'pre-emacs)
      (process-send-string (gdb-instance-process instance)
			   (car item)))))

(defun gdb-pre-prompt (instance ignored)
  "An annotation handler for `pre-prompt'.
This terminates the collection of output from a previous
command if that happens to be in effect."
  (let ((sink (gdb-instance-output-sink instance)))
    (cond
     ((eq sink 'user) t)
     ((eq sink 'emacs)
      (set-gdb-instance-output-sink instance 'post-emacs)
      (let ((handler
	     (car (cdr (gdb-instance-current-item instance)))))
	(save-excursion
	  (set-buffer (gdb-get-create-instance-buffer
		       instance 'gdb-partial-output-buffer))
	  (funcall handler))))
     ((eq sink 'pre-emacs)
      (set-gdb-instance-output-sink instance 'user)
      (error "Output sink phase error 1."))
     ((eq sink 'post-emacs)
      (set-gdb-instance-output-sink instance 'user)
      (error "Output sink phase error 2.")))))

(defun gdb-post-prompt (instance ignored)
  "An annotation handler for `post-prompt'.
This begins the collection of output from the current
command if that happens to be appropriate."
  (gdb-invalidate-registers instance ignored)
  (let ((sink (gdb-instance-output-sink instance)))
    (cond
     ((eq sink 'user) t)
     ((eq sink 'pre-emacs)
      (set-gdb-instance-output-sink instance 'emacs))

     ((eq sink 'emacs)
      (set-gdb-instance-output-sink instance 'user)
      (error "Output sink phase error 3."))

     ((eq sink 'post-emacs)
      (set-gdb-instance-output-sink instance 'user)
      (error "Output sink phase error 3.")))))


(defmacro def-gdb-auto-update-trigger (name demand-predicate gdb-command output-handler)
  (`
   (defun (, name) (instance ignored)
     (if (and ((, demand-predicate) instance)
	      (not (member '(, name)
			   (gdb-instance-pending-triggers instance))))
	 (progn
	   (gdb-instance-enqueue-idle-input
	    instance
	    (list (, gdb-command) '(, output-handler)))
	   (set-gdb-instance-pending-triggers
	    instance
	    (cons '(, name)
		  (gdb-instance-pending-triggers instance))))))))
		
(defmacro def-gdb-auto-update-handler (name trigger buf-key)
  (`
   (defun (, name) ()
     (set-gdb-instance-pending-triggers
      instance
      (delq '(, trigger)
	    (gdb-instance-pending-triggers instance)))
     (let ((buf (gdb-get-instance-buffer instance
					  '(, buf-key))))
       (and buf
	    (save-excursion
	      (set-buffer buf)
	      (let ((p (point)))
		(kill-region (point-min) (point-max))
		(insert-buffer (gdb-get-create-instance-buffer
				instance
				'gdb-partial-output-buffer))
		(goto-char p))))))))

(defmacro def-gdb-auto-updated-buffer
  (buffer-key trigger-name gdb-command output-handler-name)
  (`
   (progn
     (def-gdb-auto-update-trigger (, trigger-name)
       ;; The demand predicate:
       (lambda (instance)
	 (gdb-get-instance-buffer instance '(, buffer-key)))
       (, gdb-command)
       (, output-handler-name))
     (def-gdb-auto-update-handler (, output-handler-name)
       (, trigger-name) (, buffer-key)))))
     
       

(def-gdb-auto-updated-buffer gdb-breakpoints-buffer
  ;; This defines the auto update rule for buffers of type
  ;; `gdb-breakpoints-buffer'.
  ;;
  ;; It defines a function to serve as the annotation handler that
  ;; handles the `foo-invalidated' message.  That function is called:
  gdb-invalidate-breakpoints

  ;; To update the buffer, this command is sent to gdb.
  "server info breakpoints\n"

  ;; This also defines a function to be the handler for the output
  ;; from the command above.  That function will copy the output into
  ;; the appropriately typed buffer.  That function will be called:
  gdb-info-breakpoints-handler)

(def-gdb-auto-updated-buffer gdb-frames-buffer
  gdb-invalidate-frames
  "server where\n"
  gdb-info-frames-handler)


(def-gdb-auto-updated-buffer gdb-registers-buffer
  gdb-invalidate-registers
  "server info registers\n"
  gdb-info-registers-handler)

;;
;; At any given time, output from gdb is being directed 
;; one of three places.  By default, it goes into the gdb 
;; interaction buffer.  For commands executed on behalf
;; of emacs, it goes into a scratch buffer (not `the').
;; Finally, some gdb output is simply thrown away; for example,
;; the prompt that follows output from a command executed
;; for emacs.  
;;

(defvar gdb-output-sink 'user
  "An buffer-local indication of how output from an inferior gdb 
should be directed.  Legit values are:

    USER -- the output should be appended to the gud
            buffer.

    PRE-EMACS -- throw away output preceding output for emacs.
    EMACS -- redirect output to the partial-output buffer.
    POST-EMACS -- throw away output following output for emacs.")

(defun gdb-output-burst (instance string)
  "Handle a burst of output from a gdb instance.
This function is (indirectly) used as a gud-marker-filter.
It must return output (if any) to be insterted in the gud 
buffer."

  (save-match-data
    (let (
	  ;; Recall the left over burst from last time
	  (burst (concat (gdb-instance-burst instance) string))
	  ;; Start accumulating output for the gud buffer
	  (output ""))

      ;; Process all the complete markers in this chunk.

      (while (string-match "\n\032\032\\(.*\\)\n" burst)
	(let ((annotation (substring burst
				     (match-beginning 1)
				     (match-end 1))))
	    
	  ;; Stuff prior to the match is just ordinary output.
	  ;; It is either concatenated to OUTPUT or directed
	  ;; elsewhere.
	  (setq output
		(gdb-concat-output
		 instance
		 output
		 (substring burst 0 (match-beginning 0))))

	  ;; Take that stuff off the burst.
	  (setq burst (substring burst (match-end 0)))
	    
	  ;; Parse the tag from the annotation, and maybe its arguments.
	  (string-match "\\(\\S-*\\) ?\\(.*\\)" annotation)
	  (let* ((annotation-type (substring annotation
					     (match-beginning 1)
					     (match-end 1)))
		 (annotation-arguments (substring annotation
						  (match-beginning 2)
						  (match-end 2)))
		 (annotation-rule (assoc annotation-type
					 gdb-annotation-rules)))
	    ;; Call the handler for this annotation.
	    (dbug 'annotation
		  '(lambda () (insert annotation-type "\n")))
	    (if annotation-rule
		(funcall (car (cdr annotation-rule))
			 instance
			 annotation-arguments)
	      ;; Else the annotation is not recognized.  Ignore it silently,
	      ;; so that GDB can add new annotations without causing
	      ;; us to blow up.
	      ))))


      ;; Does the remaining text end in a partial line?
      ;; If it does, then keep part of the burst until we get more.
      (if (string-match "\n\\'\\|\n\032\\'\\|\n\032\032.*\\'"
			burst)
	  (progn
	    ;; Everything before the potential marker start can be output.
	    (setq output
		  (gdb-concat-output
		   instance
		   output
		   (substring burst 0 (match-beginning 0))))

	    ;; Everything after, we save, to combine with later input.
	    (setq burst (substring burst (match-beginning 0))))

	;; In case we know the burst contains no partial annotations:
	(progn
	  (setq output (gdb-concat-output instance output burst))
	  (setq burst "")))

      ;; Save the remaining burst for the next call to this function.
      (set-gdb-instance-burst instance burst)
      output)))

(defun gdb-concat-output (instance so-far new)
  (let ((sink (gdb-instance-output-sink instance)))
    (cond
     ((eq sink 'user) (concat so-far new))
     ((or (eq sink 'pre-emacs) (eq sink 'post-emacs)) so-far)
     ((eq sink 'emacs)
      (gdb-append-to-partial-output instance new)
      so-far)
     (t (error "Bogon output sink %d" sink)))))

(defun gdb-append-to-partial-output (instance string)
  (save-excursion
    (set-buffer
     (gdb-get-create-instance-buffer
      instance 'gdb-partial-output-buffer))
    (goto-char (point-max))
    (insert string)))

(defun gdb-clear-partial-output (instance)
  (save-excursion
    (set-buffer
     (gdb-get-create-instance-buffer
      instance 'gdb-partial-output-buffer))
    (kill-region (point-min) (point-max))))

;;
;; Instance Buffers.
;;
;; These are buffers that display output from gdb (or other
;; information) that we want to filter out from the general gdb 
;; interaction buffer.  (e.g. the backtrace buffer).
;;
;; The general pattern is that each kind of buffer is associated
;; with a rule to refresh its contents.  The rule includes one
;; function to call when it is noticed that the buffer is out of
;; date.  Typically, that will queue up an idle command for gdb.
;; 
;; Every type of instance buffer is identified by some atom 
;; such as gdb-frames-buffer.  An instance and one of these
;; atoms uniquely identifies a particular instance buffer.
;;

(defun gdb-get-instance-buffer (instance key)
  "Return the instance buffer for `instance' tagged with type `key'.
The key should be one of the cars in `gdb-instance-buffer-rules-assoc'."
  (save-excursion
    (gdb-look-for-tagged-buffer instance key (buffer-list))))

(defun gdb-get-create-instance-buffer (instance key)
  "Create a new gdb instance buffer of the type specified by `key'.
The key should be one of the cars in `gdb-instance-buffer-rules-assoc'."
  (or (gdb-get-instance-buffer instance key)
      (let* ((rules (assoc key gdb-instance-buffer-rules-assoc))
	     (name (funcall (gdb-rules-name-maker rules) instance))
	     (new (get-buffer-create name)))
	(save-excursion
	  (set-buffer new)
	  (make-variable-buffer-local 'gdb-buffer-type)
	  (setq gdb-buffer-type key)
	  (make-variable-buffer-local 'gdb-buffer-instance)
	  (setq gdb-buffer-instance instance)
	  new))))

(defun gdb-rules-name-maker (rules) (car (cdr rules)))

(defun gdb-look-for-tagged-buffer (instance key bufs)
  (and bufs
       (set-buffer (car bufs))
       (or (and (eq gdb-buffer-instance instance)
		(eq gdb-buffer-type key)
		(car bufs))
	   (gdb-look-for-tagged-buffer instance key (cdr bufs)))))

(defun gdb-instance-buffer-p (buf)
  (save-excursion
    (set-buffer buf)
    (and gdb-buffer-type
	 (not (eq gdb-buffer-type 'gud)))))

;;
;; partial-output buffers
;;
;; These accumulate output from a command executed on
;; behalf of emacs (rather than the user).  When the
;; output is complete, the hooks bound to `gdb-command-complete-hooks'
;; are called (and then cleared).  Usually these hooks are not
;; set directly but rather implicitly according to the 
;; instance-buffer rules.
;;

(defun gdb-partial-output-name (instance)
  (concat "*partial-output-"
	  (gdb-instance-target-string instance)
	  "*"))

;;
;; Backtrace buffers
;;

(defun gdb-frames-buffer-name (instance)
  (save-excursion
    (set-buffer (process-buffer (gdb-instance-process instance)))
    (concat "*stack frames of "
	    (gdb-instance-target-string instance) "*")))

(defun gud-display-frames-buffer (instance)
  (interactive (list (gdb-needed-default-instance)))
  (display-buffer
   (gdb-get-create-instance-buffer instance
				    'gdb-frames-buffer)))

;;
;; Breakpoint buffers
;;

(defun gdb-breakpoints-buffer-name (instance)
  (save-excursion
    (set-buffer (process-buffer (gdb-instance-process instance)))
    (concat "*breakpoints of " (gdb-instance-target-string instance) "*")))

(defun gud-display-breakpoints-buffer (instance)
  (interactive (list (gdb-needed-default-instance)))
  (display-buffer
   (gdb-get-create-instance-buffer instance
				    'gdb-breakpoints-buffer)))


;;
;; Registers buffers
;;

(defun gdb-registers-buffer-name (instance)
  (save-excursion
    (set-buffer (process-buffer (gdb-instance-process instance)))
    (concat "*registers of " (gdb-instance-target-string instance) "*")))

(defun gud-display-registers-buffer (instance)
  (interactive (list (gdb-needed-default-instance)))
  (display-buffer
   (gdb-get-create-instance-buffer instance
				    'gdb-registers-buffer)))




(defun gud-gdb-find-file (f)
  (find-file-noselect f))

;;;###autoload
(defun gdb (command-line)
  "Run gdb on program FILE in buffer *gud-FILE*.
The directory containing FILE becomes the initial working directory
and source-file directory for your debugger."
  (interactive
   (list (read-from-minibuffer "Run gdb (like this): "
			       (if (consp gud-gdb-history)
				   (car gud-gdb-history)
				 "gdb ")
			       nil nil
			       '(gud-gdb-history . 1))))
  (gud-overload-functions
   '((gud-massage-args . gud-gdb-massage-args)
     (gud-marker-filter . gud-gdb-marker-filter)
     (gud-find-file . gud-gdb-find-file)
     ))

  (gud-common-init command-line)

  (gud-def gud-break  "break %f:%l"  "\C-b" "Set breakpoint at current line.")
  (gud-def gud-tbreak "tbreak %f:%l" "\C-t" "Set breakpoint at current line.")
  (gud-def gud-remove "clear %l"     "\C-d" "Remove breakpoint at current line")
  (gud-def gud-step   "step %p"      "\C-s" "Step one source line with display.")
  (gud-def gud-stepi  "stepi %p"     "\C-i" "Step one instruction with display.")
  (gud-def gud-next   "next %p"      "\C-n" "Step one line (skip functions).")
  (gud-def gud-cont   "cont"         "\C-r" "Continue with display.")
  (gud-def gud-finish "finish"       "\C-f" "Finish executing current function.")
  (gud-def gud-up     "up %p"        "<" "Up N stack frames (numeric arg).")
  (gud-def gud-down   "down %p"      ">" "Down N stack frames (numeric arg).")
  (gud-def gud-print  "print %e"     "\C-p" "Evaluate C expression at point.")

  (setq comint-prompt-regexp "^(.*gdb[+]?) *")
  (setq comint-input-sender 'gdb-send)
  (run-hooks 'gdb-mode-hook)
  (make-gdb-instance (get-buffer-process (current-buffer)))
  )


;; ======================================================================
;; sdb functions

;;; History of argument lists passed to sdb.
(defvar gud-sdb-history nil)

(defvar gud-sdb-needs-tags (not (file-exists-p "/var"))
  "If nil, we're on a System V Release 4 and don't need the tags hack.")

(defvar gud-sdb-lastfile nil)

(defun gud-sdb-massage-args (file args)
  (cons file args))

(defun gud-sdb-marker-filter (string)
  (cond 
   ;; System V Release 3.2 uses this format
   ((string-match "\\(^0x\\w* in \\|^\\|\n\\)\\([^:\n]*\\):\\([0-9]*\\):.*\n"
		    string)
    (setq gud-last-frame
	  (cons
	   (substring string (match-beginning 2) (match-end 2))
	   (string-to-int 
	    (substring string (match-beginning 3) (match-end 3))))))
   ;; System V Release 4.0 
   ((string-match "^\\(BREAKPOINT\\|STEPPED\\) process [0-9]+ function [^ ]+ in \\(.+\\)\n"
		       string)
    (setq gud-sdb-lastfile
	  (substring string (match-beginning 2) (match-end 2))))
   ((and gud-sdb-lastfile (string-match "^\\([0-9]+\\):" string))
	 (setq gud-last-frame
	       (cons
		gud-sdb-lastfile
		(string-to-int 
		 (substring string (match-beginning 1) (match-end 1))))))
   (t 
    (setq gud-sdb-lastfile nil)))
  string)

(defun gud-sdb-find-file (f)
  (if gud-sdb-needs-tags
      (find-tag-noselect f)
    (find-file-noselect f)))

;;;###autoload
(defun sdb (command-line)
  "Run sdb on program FILE in buffer *gud-FILE*.
The directory containing FILE becomes the initial working directory
and source-file directory for your debugger."
  (interactive
   (list (read-from-minibuffer "Run sdb (like this): "
			       (if (consp gud-sdb-history)
				   (car gud-sdb-history)
				 "sdb ")
			       nil nil
			       '(gud-sdb-history . 1))))
  (if (and gud-sdb-needs-tags
	   (not (and (boundp 'tags-file-name) (file-exists-p tags-file-name))))
      (error "The sdb support requires a valid tags table to work."))
  (gud-overload-functions '((gud-massage-args . gud-sdb-massage-args)
			    (gud-marker-filter . gud-sdb-marker-filter)
			    (gud-find-file . gud-sdb-find-file)
			    ))

  (gud-common-init command-line)

  (gud-def gud-break  "%l b" "\C-b"   "Set breakpoint at current line.")
  (gud-def gud-tbreak "%l c" "\C-t"   "Set temporary breakpoint at current line.")
  (gud-def gud-remove "%l d" "\C-d"   "Remove breakpoint at current line")
  (gud-def gud-step   "s %p" "\C-s"   "Step one source line with display.")
  (gud-def gud-stepi  "i %p" "\C-i"   "Step one instruction with display.")
  (gud-def gud-next   "S %p" "\C-n"   "Step one line (skip functions).")
  (gud-def gud-cont   "c"    "\C-r"   "Continue with display.")
  (gud-def gud-print  "%e/"  "\C-p"   "Evaluate C expression at point.")

  (setq comint-prompt-regexp  "\\(^\\|\n\\)\\*")
  (run-hooks 'sdb-mode-hook)
  )

;; ======================================================================
;; dbx functions

;;; History of argument lists passed to dbx.
(defvar gud-dbx-history nil)

(defun gud-dbx-massage-args (file args)
  (cons file args))

(defun gud-dbx-marker-filter (string)
  (if (or (string-match
         "stopped in .* at line \\([0-9]*\\) in file \"\\([^\"]*\\)\""
         string)
        (string-match
         "signal .* in .* at line \\([0-9]*\\) in file \"\\([^\"]*\\)\""
         string))
      (setq gud-last-frame
	    (cons
	     (substring string (match-beginning 2) (match-end 2))
	     (string-to-int 
	      (substring string (match-beginning 1) (match-end 1))))))
  string)

(defun gud-dbx-find-file (f)
  (find-file-noselect f))

;;;###autoload
(defun dbx (command-line)
  "Run dbx on program FILE in buffer *gud-FILE*.
The directory containing FILE becomes the initial working directory
and source-file directory for your debugger."
  (interactive
   (list (read-from-minibuffer "Run dbx (like this): "
			       (if (consp gud-dbx-history)
				   (car gud-dbx-history)
				 "dbx ")
			       nil nil
			       '(gud-dbx-history . 1))))
  (gud-overload-functions '((gud-massage-args . gud-dbx-massage-args)
			    (gud-marker-filter . gud-dbx-marker-filter)
			    (gud-find-file . gud-dbx-find-file)
			    ))

  (gud-common-init command-line)

  (gud-def gud-break  "file \"%d%f\"\nstop at %l"
	   			  "\C-b" "Set breakpoint at current line.")
;;  (gud-def gud-break  "stop at \"%f\":%l"
;;	   			  "\C-b" "Set breakpoint at current line.")
  (gud-def gud-remove "clear %l"  "\C-d" "Remove breakpoint at current line")
  (gud-def gud-step   "step %p"	  "\C-s" "Step one line with display.")
  (gud-def gud-stepi  "stepi %p"  "\C-i" "Step one instruction with display.")
  (gud-def gud-next   "next %p"	  "\C-n" "Step one line (skip functions).")
  (gud-def gud-cont   "cont"	  "\C-r" "Continue with display.")
  (gud-def gud-up     "up %p"	  "<" "Up (numeric arg) stack frames.")
  (gud-def gud-down   "down %p"	  ">" "Down (numeric arg) stack frames.")
  (gud-def gud-print  "print %e"  "\C-p" "Evaluate C expression at point.")

  (setq comint-prompt-regexp  "^[^)]*dbx) *")
  (run-hooks 'dbx-mode-hook)
  )

;; ======================================================================
;; xdb (HP PARISC debugger) functions

;;; History of argument lists passed to xdb.
(defvar gud-xdb-history nil)

(defvar gud-xdb-directories nil
  "*A list of directories that xdb should search for source code.
If nil, only source files in the program directory
will be known to xdb.

The file names should be absolute, or relative to the directory
containing the executable being debugged.")

(defun gud-xdb-massage-args (file args)
  (nconc (let ((directories gud-xdb-directories)
	       (result nil))
	   (while directories
	     (setq result (cons (car directories) (cons "-d" result)))
	     (setq directories (cdr directories)))
	   (nreverse (cons file result)))
	 args))

(defun gud-xdb-file-name (f)
  "Transform a relative pathname to a full pathname in xdb mode"
  (let ((result nil))
    (if (file-exists-p f)
        (setq result (expand-file-name f))
      (let ((directories gud-xdb-directories))
        (while directories
          (let ((path (concat (car directories) "/" f)))
            (if (file-exists-p path)
                (setq result (expand-file-name path)
                      directories nil)))
          (setq directories (cdr directories)))))
    result))

;; xdb does not print the lines all at once, so we have to accumulate them
(defvar gud-xdb-accumulation "")

(defun gud-xdb-marker-filter (string)
  (let (result)
    (if (or (string-match comint-prompt-regexp string)
            (string-match ".*\012" string))
        (setq result (concat gud-xdb-accumulation string)
              gud-xdb-accumulation "")
      (setq gud-xdb-accumulation (concat gud-xdb-accumulation string)))
    (if result
        (if (or (string-match "\\([^\n \t:]+\\): [^:]+: \\([0-9]+\\):" result)
                (string-match "[^: \t]+:[ \t]+\\([^:]+\\): [^:]+: \\([0-9]+\\):"
                              result))
            (let ((line (string-to-int 
                         (substring result (match-beginning 2) (match-end 2))))
                  (file (gud-xdb-file-name
                         (substring result (match-beginning 1) (match-end 1)))))
              (if file
                  (setq gud-last-frame (cons file line))))))
    (or result "")))    
               
(defun gud-xdb-find-file (f)
  (let ((realf (gud-xdb-file-name f)))
    (if realf (find-file-noselect realf))))

;;;###autoload
(defun xdb (command-line)
  "Run xdb on program FILE in buffer *gud-FILE*.
The directory containing FILE becomes the initial working directory
and source-file directory for your debugger.

You can set the variable 'gud-xdb-directories' to a list of program source
directories if your program contains sources from more than one directory."
  (interactive
   (list (read-from-minibuffer "Run xdb (like this): "
			       (if (consp gud-xdb-history)
				   (car gud-xdb-history)
				 "xdb ")
			       nil nil
			       '(gud-xdb-history . 1))))
  (gud-overload-functions '((gud-massage-args . gud-xdb-massage-args)
			    (gud-marker-filter . gud-xdb-marker-filter)
			    (gud-find-file . gud-xdb-find-file)))

  (gud-common-init command-line)

  (gud-def gud-break  "b %f:%l"    "\C-b" "Set breakpoint at current line.")
  (gud-def gud-tbreak "b %f:%l\\t" "\C-t"
           "Set temporary breakpoint at current line.")
  (gud-def gud-remove "db"         "\C-d" "Remove breakpoint at current line")
  (gud-def gud-step   "s %p"	   "\C-s" "Step one line with display.")
  (gud-def gud-next   "S %p"	   "\C-n" "Step one line (skip functions).")
  (gud-def gud-cont   "c"	   "\C-r" "Continue with display.")
  (gud-def gud-up     "up %p"	   "<"    "Up (numeric arg) stack frames.")
  (gud-def gud-down   "down %p"	   ">"    "Down (numeric arg) stack frames.")
  (gud-def gud-finish "bu\\t"      "\C-f" "Finish executing current function.")
  (gud-def gud-print  "p %e"       "\C-p" "Evaluate C expression at point.")

  (setq comint-prompt-regexp  "^>")
  (make-local-variable 'gud-xdb-accumulation)
  (setq gud-xdb-accumulation "")
  (run-hooks 'xdb-mode-hook))

;; ======================================================================
;; perldb functions

;;; History of argument lists passed to perldb.
(defvar gud-perldb-history nil)

(defun gud-perldb-massage-args (file args)
  (cons "-d" (cons file (cons "-emacs" args))))

;; There's no guarantee that Emacs will hand the filter the entire
;; marker at once; it could be broken up across several strings.  We
;; might even receive a big chunk with several markers in it.  If we
;; receive a chunk of text which looks like it might contain the
;; beginning of a marker, we save it here between calls to the
;; filter.
(defvar gud-perldb-marker-acc "")

(defun gud-perldb-marker-filter (string)
  (save-match-data
    (setq gud-perldb-marker-acc (concat gud-perldb-marker-acc string))
    (let ((output ""))

      ;; Process all the complete markers in this chunk.
      (while (string-match "^\032\032\\([^:\n]*\\):\\([0-9]*\\):.*\n"
			   gud-perldb-marker-acc)
	(setq

	 ;; Extract the frame position from the marker.
	 gud-last-frame
	 (cons (substring gud-perldb-marker-acc (match-beginning 1) (match-end 1))
	       (string-to-int (substring gud-perldb-marker-acc
					 (match-beginning 2)
					 (match-end 2))))

	 ;; Append any text before the marker to the output we're going
	 ;; to return - we don't include the marker in this text.
	 output (concat output
			(substring gud-perldb-marker-acc 0 (match-beginning 0)))

	 ;; Set the accumulator to the remaining text.
	 gud-perldb-marker-acc (substring gud-perldb-marker-acc (match-end 0))))

      ;; Does the remaining text look like it might end with the
      ;; beginning of another marker?  If it does, then keep it in
      ;; gud-perldb-marker-acc until we receive the rest of it.  Since we
      ;; know the full marker regexp above failed, it's pretty simple to
      ;; test for marker starts.
      (if (string-match "^\032.*\\'" gud-perldb-marker-acc)
	  (progn
	    ;; Everything before the potential marker start can be output.
	    (setq output (concat output (substring gud-perldb-marker-acc
						   0 (match-beginning 0))))

	    ;; Everything after, we save, to combine with later input.
	    (setq gud-perldb-marker-acc
		  (substring gud-perldb-marker-acc (match-beginning 0))))

	(setq output (concat output gud-perldb-marker-acc)
	      gud-perldb-marker-acc ""))

      output)))

(defun gud-perldb-find-file (f)
  (find-file-noselect f))

;;;###autoload
(defun perldb (command-line)
  "Run perldb on program FILE in buffer *gud-FILE*.
The directory containing FILE becomes the initial working directory
and source-file directory for your debugger."
  (interactive
   (list (read-from-minibuffer "Run perldb (like this): "
			       (if (consp gud-perldb-history)
				   (car gud-perldb-history)
				 "perl ")
			       nil nil
			       '(gud-perldb-history . 1))))
  (gud-overload-functions '((gud-massage-args . gud-perldb-massage-args)
			    (gud-marker-filter . gud-perldb-marker-filter)
			    (gud-find-file . gud-perldb-find-file)
			    ))

  (gud-common-init command-line)

  (gud-def gud-break  "b %l"         "\C-b" "Set breakpoint at current line.")
  (gud-def gud-remove "d %l"         "\C-d" "Remove breakpoint at current line")
  (gud-def gud-step   "s"            "\C-s" "Step one source line with display.")
  (gud-def gud-next   "n"            "\C-n" "Step one line (skip functions).")
  (gud-def gud-cont   "c"            "\C-r" "Continue with display.")
;  (gud-def gud-finish "finish"       "\C-f" "Finish executing current function.")
;  (gud-def gud-up     "up %p"        "<" "Up N stack frames (numeric arg).")
;  (gud-def gud-down   "down %p"      ">" "Down N stack frames (numeric arg).")
  (gud-def gud-print  "%e"           "\C-p" "Evaluate perl expression at point.")

  (setq comint-prompt-regexp "^  DB<[0-9]+> ")
  (run-hooks 'perldb-mode-hook)
  )

;;
;; End of debugger-specific information
;;


;;; When we send a command to the debugger via gud-call, it's annoying
;;; to see the command and the new prompt inserted into the debugger's
;;; buffer; we have other ways of knowing the command has completed.
;;;
;;; If the buffer looks like this:
;;; --------------------
;;; (gdb) set args foo bar
;;; (gdb) -!-
;;; --------------------
;;; (the -!- marks the location of point), and we type `C-x SPC' in a
;;; source file to set a breakpoint, we want the buffer to end up like
;;; this:
;;; --------------------
;;; (gdb) set args foo bar
;;; Breakpoint 1 at 0x92: file make-docfile.c, line 49.
;;; (gdb) -!-
;;; --------------------
;;; Essentially, the old prompt is deleted, and the command's output
;;; and the new prompt take its place.
;;;
;;; Not echoing the command is easy enough; you send it directly using
;;; process-send-string, and it never enters the buffer.  However,
;;; getting rid of the old prompt is trickier; you don't want to do it
;;; when you send the command, since that will result in an annoying
;;; flicker as the prompt is deleted, redisplay occurs while Emacs
;;; waits for a response from the debugger, and the new prompt is
;;; inserted.  Instead, we'll wait until we actually get some output
;;; from the subprocess before we delete the prompt.  If the command
;;; produced no output other than a new prompt, that prompt will most
;;; likely be in the first chunk of output received, so we will delete
;;; the prompt and then replace it with an identical one.  If the
;;; command produces output, the prompt is moving anyway, so the
;;; flicker won't be annoying.
;;;
;;; So - when we want to delete the prompt upon receipt of the next
;;; chunk of debugger output, we position gud-delete-prompt-marker at
;;; the start of the prompt; the process filter will notice this, and
;;; delete all text between it and the process output marker.  If
;;; gud-delete-prompt-marker points nowhere, we leave the current
;;; prompt alone.
(defvar gud-delete-prompt-marker nil)


(defvar gdbish-comint-mode-map (copy-keymap comint-mode-map))
(define-key gdbish-comint-mode-map "\C-c\M-\C-r" 'gud-display-registers-buffer)
(define-key gdbish-comint-mode-map "\C-c\M-\C-f" 'gud-display-frames-buffer)
(define-key gdbish-comint-mode-map "\C-c\M-\C-b" 'gud-display-breakpoints-buffer)

(defun gud-mode ()
  "Major mode for interacting with an inferior debugger process.

   You start it up with one of the commands M-x gdb, M-x sdb, M-x dbx,
or M-x xdb.  Each entry point finishes by executing a hook; `gdb-mode-hook',
`sdb-mode-hook', `dbx-mode-hook' or `xdb-mode-hook' respectively.

After startup, the following commands are available in both the GUD
interaction buffer and any source buffer GUD visits due to a breakpoint stop
or step operation:

\\[gud-break] sets a breakpoint at the current file and line.  In the
GUD buffer, the current file and line are those of the last breakpoint or
step.  In a source buffer, they are the buffer's file and current line.

\\[gud-remove] removes breakpoints on the current file and line.

\\[gud-refresh] displays in the source window the last line referred to
in the gud buffer.

\\[gud-step], \\[gud-next], and \\[gud-stepi] do a step-one-line,
step-one-line (not entering function calls), and step-one-instruction
and then update the source window with the current file and position.
\\[gud-cont] continues execution.

\\[gud-print] tries to find the largest C lvalue or function-call expression
around point, and sends it to the debugger for value display.

The above commands are common to all supported debuggers except xdb which
does not support stepping instructions.

Under gdb, sdb and xdb, \\[gud-tbreak] behaves exactly like \\[gud-break],
except that the breakpoint is temporary; that is, it is removed when
execution stops on it.

Under gdb, dbx, and xdb, \\[gud-up] pops up through an enclosing stack
frame.  \\[gud-down] drops back down through one.

If you are using gdb or xdb, \\[gud-finish] runs execution to the return from
the current function and stops.

All the keystrokes above are accessible in the GUD buffer
with the prefix C-c, and in all buffers through the prefix C-x C-a.

All pre-defined functions for which the concept make sense repeat
themselves the appropriate number of times if you give a prefix
argument.

You may use the `gud-def' macro in the initialization hook to define other
commands.

Other commands for interacting with the debugger process are inherited from
comint mode, which see."
  (interactive)
  (comint-mode)
  (setq major-mode 'gud-mode)
  (setq mode-name "Debugger")
  (setq mode-line-process '(": %s"))
  (use-local-map (copy-keymap gdbish-comint-mode-map))
  (make-local-variable 'gud-last-frame)
  (setq gud-last-frame nil)
  (make-local-variable 'comint-prompt-regexp)
  (make-local-variable 'gud-delete-prompt-marker)
  (setq gud-delete-prompt-marker (make-marker))
  (run-hooks 'gud-mode-hook)
)

(defvar gud-comint-buffer nil)

;; Chop STRING into words separated by SPC or TAB and return a list of them.
(defun gud-chop-words (string)
  (let ((i 0) (beg 0)
	(len (length string))
	(words nil))
    (while (< i len)
      (if (memq (aref string i) '(?\t ? ))
	  (progn
	    (setq words (cons (substring string beg i) words)
		  beg (1+ i))
	    (while (and (< beg len) (memq (aref string beg) '(?\t ? )))
	      (setq beg (1+ beg)))
	    (setq i (1+ beg)))
	(setq i (1+ i))))
    (if (< beg len)
	(setq words (cons (substring string beg) words)))
    (nreverse words)))

(defvar gud-target-name "--unknown--"
  "The apparent name of the program being debugged in a gud buffer.
For sure this the root string used in smashing together the gud 
buffer's name, even if that doesn't happen to be the name of a 
program.")

;; Perform initializations common to all debuggers.
(defun gud-common-init (command-line)
  (let* ((words (gud-chop-words command-line))
	 (program (car words))
	 (file-word (let ((w (cdr words)))
		      (while (and w (= ?- (aref (car w) 0)))
			(setq w (cdr w)))
		      (car w)))
	 (args (delq file-word (cdr words)))
	 (file (expand-file-name file-word))
	 (filepart (file-name-nondirectory file))
	 (buffer-name (concat "*gud-" filepart "*")))
    (switch-to-buffer buffer-name)
    (setq default-directory (file-name-directory file))
    (or (bolp) (newline))
    (insert "Current directory is " default-directory "\n")
    (apply 'make-comint (concat "gud-" filepart) program nil
	   (gud-massage-args file args))
    (gud-mode)
    (make-variable-buffer-local 'gud-target-name)
    (setq gud-target-name filepart))
  (set-process-filter (get-buffer-process (current-buffer)) 'gud-filter)
  (set-process-sentinel (get-buffer-process (current-buffer)) 'gud-sentinel)
  (gud-set-buffer)
  )

(defun gud-set-buffer ()
  (cond ((eq major-mode 'gud-mode)
	(setq gud-comint-buffer (current-buffer)))))

;; These functions are responsible for inserting output from your debugger
;; into the buffer.  The hard work is done by the method that is
;; the value of gud-marker-filter.

(defun gud-filter (proc string)
  ;; Here's where the actual buffer insertion is done
  (let ((inhibit-quit t))
    (save-excursion
      (set-buffer (process-buffer proc))
      (let (moving output-after-point)
	(save-excursion
	  (goto-char (process-mark proc))
	  ;; If we have been so requested, delete the debugger prompt.
	  (if (marker-buffer gud-delete-prompt-marker)
	      (progn
		(delete-region (point) gud-delete-prompt-marker)
		(set-marker gud-delete-prompt-marker nil)))
	  (insert-before-markers (gud-marker-filter string))
	  (setq moving (= (point) (process-mark proc)))
	  (setq output-after-point (< (point) (process-mark proc)))
	  ;; Check for a filename-and-line number.
	  ;; Don't display the specified file
	  ;; unless (1) point is at or after the position where output appears
	  ;; and (2) this buffer is on the screen.
	  (if (and gud-last-frame
		   (not output-after-point)
		   (get-buffer-window (current-buffer)))
	      (gud-display-frame)))
	(if moving (goto-char (process-mark proc)))))))

(defun gud-sentinel (proc msg)
  (cond ((null (buffer-name (process-buffer proc)))
	 ;; buffer killed
	 ;; Stop displaying an arrow in a source file.
	 (setq overlay-arrow-position nil)
	 (set-process-buffer proc nil))
	((memq (process-status proc) '(signal exit))
	 ;; Stop displaying an arrow in a source file.
	 (setq overlay-arrow-position nil)
	 ;; Fix the mode line.
	 (setq mode-line-process
	       (concat ": "
		       (symbol-name (process-status proc))))
	 (let* ((obuf (current-buffer)))
	   ;; save-excursion isn't the right thing if
	   ;;  process-buffer is current-buffer
	   (unwind-protect
	       (progn
		 ;; Write something in *compilation* and hack its mode line,
		 (set-buffer (process-buffer proc))
		 ;; Force mode line redisplay soon
		 (set-buffer-modified-p (buffer-modified-p))
		 (if (eobp)
		     (insert ?\n mode-name " " msg)
		   (save-excursion
		     (goto-char (point-max))
		     (insert ?\n mode-name " " msg)))
		 ;; If buffer and mode line will show that the process
		 ;; is dead, we can delete it now.  Otherwise it
		 ;; will stay around until M-x list-processes.
		 (delete-process proc))
	     ;; Restore old buffer, but don't restore old point
	     ;; if obuf is the gud buffer.
	     (set-buffer obuf))))))

(defun gud-display-frame ()
  "Find and obey the last filename-and-line marker from the debugger.
Obeying it means displaying in another window the specified file and line."
  (interactive)
  (if gud-last-frame
   (progn
     (gud-set-buffer)
     (gud-display-line (car gud-last-frame) (cdr gud-last-frame))
     (setq gud-last-last-frame gud-last-frame
	   gud-last-frame nil))))

;; Make sure the file named TRUE-FILE is in a buffer that appears on the screen
;; and that its line LINE is visible.
;; Put the overlay-arrow on the line LINE in that buffer.
;; Most of the trickiness in here comes from wanting to preserve the current
;; region-restriction if that's possible.  We use an explicit display-buffer
;; to get around the fact that this is called inside a save-excursion.

(defun gud-display-line (true-file line)
  (let* ((buffer (gud-find-file true-file))
	 (window (display-buffer buffer))
	 (pos))
;;;    (if (equal buffer (current-buffer))
;;;	nil
;;;      (setq buffer-read-only nil))
    (save-excursion
;;;      (setq buffer-read-only t)
      (set-buffer buffer)
      (save-restriction
	(widen)
	(goto-line line)
	(setq pos (point))
	(setq overlay-arrow-string "=>")
	(or overlay-arrow-position
	    (setq overlay-arrow-position (make-marker)))
	(set-marker overlay-arrow-position (point) (current-buffer)))
      (cond ((or (< pos (point-min)) (> pos (point-max)))
	     (widen)
	     (goto-char pos))))
    (set-window-point window overlay-arrow-position)))

;;; The gud-call function must do the right thing whether its invoking
;;; keystroke is from the GUD buffer itself (via major-mode binding)
;;; or a C buffer.  In the former case, we want to supply data from
;;; gud-last-frame.  Here's how we do it:

(defun gud-format-command (str arg)
  (let ((insource (not (eq (current-buffer) gud-comint-buffer))))
    (if (string-match "\\(.*\\)%f\\(.*\\)" str)
	(setq str (concat
		   (substring str (match-beginning 1) (match-end 1))
		   (file-name-nondirectory (if insource
					       (buffer-file-name)
					     (car gud-last-frame)))
		   (substring str (match-beginning 2) (match-end 2)))))
    (if (string-match "\\(.*\\)%d\\(.*\\)" str)
	(setq str (concat
		   (substring str (match-beginning 1) (match-end 1))
		   (file-name-directory (if insource
					    (buffer-file-name)
					  (car gud-last-frame)))
		   (substring str (match-beginning 2) (match-end 2)))))
    (if (string-match "\\(.*\\)%l\\(.*\\)" str)
	(setq str (concat
		   (substring str (match-beginning 1) (match-end 1))
		   (if insource
		       (save-excursion
			 (beginning-of-line)
			 (save-restriction (widen) 
					   (1+ (count-lines 1 (point)))))
		     (cdr gud-last-frame))
		   (substring str (match-beginning 2) (match-end 2)))))
    (if (string-match "\\(.*\\)%e\\(.*\\)" str)
	(setq str (concat
		   (substring str (match-beginning 1) (match-end 1))
		   (find-c-expr)
		   (substring str (match-beginning 2) (match-end 2)))))
    (if (string-match "\\(.*\\)%a\\(.*\\)" str)
	(setq str (concat
		   (substring str (match-beginning 1) (match-end 1))
		   (gud-read-address)
		   (substring str (match-beginning 2) (match-end 2)))))
    (if (string-match "\\(.*\\)%p\\(.*\\)" str)
	(setq str (concat
		   (substring str (match-beginning 1) (match-end 1))
		   (if arg (int-to-string arg) "")
		   (substring str (match-beginning 2) (match-end 2)))))
    )
  str
  )

(defun gud-read-address ()
  "Return a string containing the core-address found in the buffer at point."
  (save-excursion
    (let ((pt (point)) found begin)
      (setq found (if (search-backward "0x" (- pt 7) t) (point)))
      (cond
       (found (forward-char 2)
	      (buffer-substring found
				(progn (re-search-forward "[^0-9a-f]")
				       (forward-char -1)
				       (point))))
       (t (setq begin (progn (re-search-backward "[^0-9]") 
			     (forward-char 1)
			     (point)))
	  (forward-char 1)
	  (re-search-forward "[^0-9]")
	  (forward-char -1)
	  (buffer-substring begin (point)))))))

(defun gud-call (fmt &optional arg)
  (let ((msg (gud-format-command fmt arg)))
    (message "Command: %s" msg)
    (sit-for 0)
    (gud-basic-call msg)))

(defun gud-basic-call (command)
  "Invoke the debugger COMMAND displaying source in other window."
  (interactive)
  (gud-set-buffer)
  (let ((command (concat command "\n"))
	(proc (get-buffer-process gud-comint-buffer)))

    ;; Arrange for the current prompt to get deleted.
    (save-excursion
      (set-buffer gud-comint-buffer)
      (goto-char (process-mark proc))
      (beginning-of-line)
      (if (looking-at comint-prompt-regexp)
	  (set-marker gud-delete-prompt-marker (point))))
    (process-send-string proc command)))

(defun gud-refresh (&optional arg)
  "Fix up a possibly garbled display, and redraw the arrow."
  (interactive "P")
  (recenter arg)
  (or gud-last-frame (setq gud-last-frame gud-last-last-frame))
  (gud-display-frame))

;;; Code for parsing expressions out of C code.  The single entry point is
;;; find-c-expr, which tries to return an lvalue expression from around point.
;;;
;;; The rest of this file is a hacked version of gdbsrc.el by
;;; Debby Ayers <ayers@asc.slb.com>,
;;; Rich Schaefer <schaefer@asc.slb.com> Schlumberger, Austin, Tx.

(defun find-c-expr ()
  "Returns the C expr that surrounds point."
  (interactive)
  (save-excursion
    (let ((p) (expr) (test-expr))
      (setq p (point))
      (setq expr (expr-cur))
      (setq test-expr (expr-prev))
      (while (expr-compound test-expr expr)
	(setq expr (cons (car test-expr) (cdr expr)))
	(goto-char (car expr))
	(setq test-expr (expr-prev)))
      (goto-char p)
      (setq test-expr (expr-next))
      (while (expr-compound expr test-expr)
	(setq expr (cons (car expr) (cdr test-expr)))
	(setq test-expr (expr-next))
	)
      (buffer-substring (car expr) (cdr expr)))))

(defun expr-cur ()
  "Returns the expr that point is in; point is set to beginning of expr.
The expr is represented as a cons cell, where the car specifies the point in
the current buffer that marks the beginning of the expr and the cdr specifies 
the character after the end of the expr."
  (let ((p (point)) (begin) (end))
    (expr-backward-sexp)
    (setq begin (point))
    (expr-forward-sexp)
    (setq end (point))
    (if (>= p end) 
	(progn
	 (setq begin p)
	 (goto-char p)
	 (expr-forward-sexp)
	 (setq end (point))
	 )
      )
    (goto-char begin)
    (cons begin end)))

(defun expr-backward-sexp ()
  "Version of `backward-sexp' that catches errors."
  (condition-case nil
      (backward-sexp)
    (error t)))

(defun expr-forward-sexp ()
  "Version of `forward-sexp' that catches errors."
  (condition-case nil
     (forward-sexp)
    (error t)))

(defun expr-prev ()
  "Returns the previous expr, point is set to beginning of that expr.
The expr is represented as a cons cell, where the car specifies the point in
the current buffer that marks the beginning of the expr and the cdr specifies 
the character after the end of the expr"
  (let ((begin) (end))
    (expr-backward-sexp)
    (setq begin (point))
    (expr-forward-sexp)
    (setq end (point))
    (goto-char begin)
    (cons begin end)))

(defun expr-next ()
  "Returns the following expr, point is set to beginning of that expr.
The expr is represented as a cons cell, where the car specifies the point in
the current buffer that marks the beginning of the expr and the cdr specifies 
the character after the end of the expr."
  (let ((begin) (end))
    (expr-forward-sexp)
    (expr-forward-sexp)
    (setq end (point))
    (expr-backward-sexp)
    (setq begin (point))
    (cons begin end)))

(defun expr-compound-sep (span-start span-end)
  "Returns '.' for '->' & '.', returns ' ' for white space,
returns '?' for other punctuation."
  (let ((result ? )
	(syntax))
    (while (< span-start span-end)
      (setq syntax (char-syntax (char-after span-start)))
      (cond
       ((= syntax ? ) t)
       ((= syntax ?.) (setq syntax (char-after span-start))
	(cond 
	 ((= syntax ?.) (setq result ?.))
	 ((and (= syntax ?-) (= (char-after (+ span-start 1)) ?>))
	  (setq result ?.)
	  (setq span-start (+ span-start 1)))
	 (t (setq span-start span-end)
	    (setq result ??)))))
      (setq span-start (+ span-start 1)))
    result))

(defun expr-compound (first second)
  "Non-nil if concatenating FIRST and SECOND makes a single C token.
The two exprs are represented as a cons cells, where the car 
specifies the point in the current buffer that marks the beginning of the 
expr and the cdr specifies the character after the end of the expr.
Link exprs of the form:
      Expr -> Expr
      Expr . Expr
      Expr (Expr)
      Expr [Expr]
      (Expr) Expr
      [Expr] Expr"
  (let ((span-start (cdr first))
	(span-end (car second))
	(syntax))
    (setq syntax (expr-compound-sep span-start span-end))
    (cond
     ((= (car first) (car second)) nil)
     ((= (cdr first) (cdr second)) nil)
     ((= syntax ?.) t)
     ((= syntax ? )
	 (setq span-start (char-after (- span-start 1)))
	 (setq span-end (char-after span-end))
	 (cond
	  ((= span-start ?) ) t )
	  ((= span-start ?] ) t )
          ((= span-end ?( ) t )
	  ((= span-end ?[ ) t )
	  (t nil))
	 )
     (t nil))))

(provide 'gud)

;;; gud.el ends here

(defun toggle-bp-this-line ()
  (interactive)
  (save-excursion
    (let (bp-num bp-state)
      (beginning-of-line 1)
      (if (not (looking-at "\\([0-9]*\\)\\s-*\\S-*\\s-*\\S-*\\s-*\\(.\\)"))
	  (error "Not recognized as breakpoint line (demo foo).")
	(process-send-string
	 gdb-buffer-process!!!
	 (concat
	  (if (eq ?y (char-after (match-beginning 2)))
	      "server disable "
	    "server enable ")
	  (buffer-substring (match-beginning 0)
			    (match-end 1))
	  "\n"))))))


