;;; gdb-mi.el

;; Author: Nick Roberts <nickrob@gnu.org>
;; Maintainer: Nick Roberts <nickrob@gnu.org>
;; Keywords: unix, tools

;; Copyright (C) 2004, 2005, 2006 Free Software Foundation, Inc.

;; This file is part of GNU GDB.

;; GNU GDB is free software; you can redistribute it and/or modify
;; it under the terms of the GNU General Public License as published by
;; the Free Software Foundation; either version 2, or (at your option)
;; any later version.

;; This program is distributed in the hope that it will be useful,
;; but WITHOUT ANY WARRANTY; without even the implied warranty of
;; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;; GNU General Public License for more details.

;;; Commentary:

;; This mode acts as a graphical user interface to GDB and works with Emacs
;; 22.x and the version of GDB with which it is distributed.  You can interact
;; with GDB through the GUD buffer in the usual way, but there are also
;; buffers which control the execution and describe the state of your program.
;; It separates the input/output of your program from that of GDB and displays
;; expressions and their current values in their own buffers.  It also uses
;; features of Emacs 21 such as the fringe/display margin for breakpoints, and
;; the toolbar (see the GDB Graphical Interface section in the Emacs info
;; manual).

;; Start the debugger with M-x gdbmi.

;; This file uses GDB/MI as the primary interface to GDB.  It is still under
;; development and is part of a process to migrate Emacs from annotations (as
;; used in gdb-ui.el) to GDB/MI.  It runs gdb with GDB/MI (-interp=mi) and
;; access CLI using "-interpreter-exec console-quoted cli-command".  This code
;; works without gdb-ui.el and uses MI TOKENS instead of queues.

;; This file uses GDB/MI as the primary interface to GDB. It is still under
;; development and is part of a process to migrate Emacs from annotations
;; (as used in gdb-ui.el) to GDB/MI.
;;
;; This mode ONLY WORKS WITH THE nickrob-async-20060513 branch using
;; "gdb --async -i=mi" for asynchronous interaction.
;;
;; Windows Platforms:
;;
;; If you are using Emacs and GDB on Windows you will need to flush the buffer
;; explicitly in your program if you want timely display of I/O in Emacs.
;; Alternatively you can make the output stream unbuffered, for example, by
;; using a macro:
;;
;;           #ifdef UNBUFFERED
;;	     setvbuf (stdout, (char *) NULL, _IONBF, 0);
;;	     #endif
;;
;; and compiling with -DUNBUFFERED while debugging.
;;
;; Known Bugs:
;;
;; 1) Cannot handle programs that need input without separating the I/O
;;    of GDB with that of program.
;; 2) Completion doesn't work.
;; 3) To avoid extra output in the GUD buffer it is best not to use run, step,
;;    next or continue etc but their MI counterparts through gud-run, gud-step
;;    etc, e.g clicking on the appropriate icon in the toolbar.
;; 4) With the "tbreak" or "start" command the *stopped output doesn't give a
;;    reason field so this mode fails.

;; TODO:
;; 1) Use MI command -data-read-memory for memory window.
;; 2) Use MI command -data-disassemble for disassembly window.
;; 4) Watch windows to work with threads.
;;
;;; Code:

(require 'gud)

(defvar tool-bar-map)

(defvar gdb-pc-address "main" "Initialisation for Assembler buffer.
Set to \"main\" at start if gdb-show-main is t.")
(defvar gdb-previous-frame-address nil)
(defvar gdb-memory-address "main")
(defvar gdb-previous-frame nil)
(defvar gdb-selected-frame nil)
(defvar gdb-frame-number nil)
(defvar gdb-current-language nil)
(defvar gdb-var-list nil
 "List of variables in watch window.
Each element has the form (VARNUM EXPRESSION NUMCHILD TYPE VALUE STATUS) where
STATUS is nil (unchanged), `changed' or `out-of-scope'.")
(defvar gdb-main-file nil "Source file from which program execution begins.")
(defvar gdb-overlay-arrow-position nil)
(defvar gdb-stack-position nil)
(defvar gdb-server-prefix nil)
(defvar gdb-location-alist nil
  "Alist of breakpoint numbers and full filenames.")
(defvar gdb-active-process nil "GUD tooltips display variable values when t, \
and #define directives otherwise.")
(defvar gdb-error "Non-nil when GDB is reporting an error.")
(defvar gdb-macro-info nil
  "Non-nil if GDB knows that the inferior includes preprocessor macro info.")
(defvar gdb-register-names nil "List of register names.")
(defvar gdb-changed-registers nil
  "List of changed register numbers (strings).")
(defvar gdb-buffer-fringe-width nil)
(defvar gdb-last-command nil)
(defvar gdb-prompt-name nil)
(defvar gdb-token-number 0)
(defvar gdb-handler-alist '())
(defvar gdb-handler-number nil)
(defvar gdb-source-file-list nil)
(defvar gdb-first-done-or-error t)
(defvar gdb-source-window nil)
(defvar gdb-inferior-status nil)
(defvar gdb-continuation nil)

(defvar gdb-buffer-type nil
  "One of the symbols bound in `gdb-buffer-rules'.")
(make-variable-buffer-local 'gdb-buffer-type)

(defvar gdb-output-sink 'user
  "The disposition of the output of the current gdb command.
Possible values are these symbols:

    `user' -- gdb output should be copied to the GUD buffer
              for the user to see.

    `emacs' -- output should be collected in the partial-output-buffer
	       for subsequent processing by a command.  This is the
	       disposition of output generated by commands that
	       gdb mode sends to gdb on its own behalf.")

(defvar gdb-pending-triggers '()
  "A list of trigger functions that have run later than their output
handlers.")

(defcustom gdb-debug-ring-max 128
  "Maximum size of `gdb-debug-ring'."
  :group 'gud
  :type 'integer
  :version "22.1")

(defvar gdb-debug-ring nil
  "List of commands, most recent first, sent to and replies received from GDB.
This variable is used to debug GDB-UI.")

;;;###autoload
(defcustom gdb-enable-debug nil
  "Non-nil means record the process input and output in `gdb-debug-ring'."
  :type 'boolean
  :group 'gud
  :version "22.1")

(defcustom gdb-cpp-define-alist-program "gcc -E -dM -"
  "Shell command for generating a list of defined macros in a source file.
This list is used to display the #define directive associated
with an identifier as a tooltip.  It works in a debug session with
GDB, when gud-tooltip-mode is t.

Set `gdb-cpp-define-alist-flags' for any include paths or
predefined macros."
  :type 'string
  :group 'gud
  :version "22.1")

(defcustom gdb-cpp-define-alist-flags ""
  "Preprocessor flags for `gdb-cpp-define-alist-program'."
  :type 'string
  :group 'gud
  :version "22.1")

(defcustom gdb-show-main nil
  "Non-nil means display source file containing the main routine at startup.
Also display the main routine in the disassembly buffer if present."
  :type 'boolean
  :group 'gud
  :version "22.1")

; Note: This mode seems to require a separate buffer for inferior IO.
(defcustom gdb-use-separate-io-buffer t
  "Non-nil means display output from the debugged program in a separate buffer."
  :type 'boolean
  :group 'gud
  :version "22.1")

(defun gdb-force-mode-line-update (status)
  (let ((buffer gud-comint-buffer))
    (if (and buffer (buffer-name buffer))
	(with-current-buffer buffer
	  (setq mode-line-process
		(format ":%s [%s]"
			(process-status (get-buffer-process buffer)) status))
	  ;; Force mode line redisplay soon.
	  (force-mode-line-update)))))

(defun gdb-use-separate-io-buffer (arg)
  "Toggle separate IO for debugged program.
With arg, use separate IO iff arg is positive."
  (interactive "P")
  (setq gdb-use-separate-io-buffer
	(if (null arg)
	    (not gdb-use-separate-io-buffer)
	  (> (prefix-numeric-value arg) 0)))
  (message (format "Separate IO %sabled"
		   (if gdb-use-separate-io-buffer "en" "dis")))
  (if (and gud-comint-buffer
	   (buffer-name gud-comint-buffer))
      (condition-case nil
	  (if gdb-use-separate-io-buffer
	      (progn
		gdb-use-separate-io-buffer
		(gdb-get-buffer-create 'gdb-inferior-io)
		(gdb-clear-inferior-io)
		(set-process-filter
		 (get-process "gdb-inferior") 'gdb-inferior-filter)
		(gdb-input
		 ;; Needs GDB 6.4 onwards
		 (list
		  (concat "-inferior-tty-set "
			  (process-tty-name (get-process "gdb-inferior")) "\n")
		  'ignore))
		(gdb-restore-windows))
	    (kill-buffer (gdb-inferior-io-name)))
	(error nil))))

(defun gdb-find-watch-expression ()
  (let* ((var (nth (- (line-number-at-pos (point)) 2) gdb-var-list))
	 (varnum (car var)) expr array)
    (string-match "\\(var[0-9]+\\)\\.\\(.*\\)" varnum)
    (let ((var1 (assoc (match-string 1 varnum) gdb-var-list)) var2 varnumlet
	  (component-list (split-string (match-string 2 varnum) "\\." t)))
      (setq expr (nth 1 var1))
      (setq varnumlet (car var1))
      (dolist (component component-list)
	(setq var2 (assoc varnumlet gdb-var-list))
	(setq expr (concat expr
			   (if (string-match ".*\\[[0-9]+\\]$" (nth 3 var2))
			       (concat "[" component "]")
			     (concat "." component))))
	(setq varnumlet (concat varnumlet "." component)))
      expr)))

(defvar gdb-locals-font-lock-keywords
  '(
    ;; var = type value
    ( "\\(^\\(\\sw\\|[_.]\\)+\\)\t+\\(\\(\\sw\\|[_.]\\)+\\)"
      (1 font-lock-variable-name-face)
      (3 font-lock-type-face))
    )
  "Font lock keywords used in `gdb-local-mode'.")

;;;###autoload
(defun gdbmi (command-line)
  "Run gdb on program FILE in buffer *gud-FILE*.
The directory containing FILE becomes the initial working directory
and source-file directory for your debugger.

If `gdb-many-windows' is nil (the default value) then gdb just
pops up the GUD buffer unless `gdb-show-main' is t.  In this case
it starts with two windows: one displaying the GUD buffer and the
other with the source file with the main routine of the inferior.

If `gdb-many-windows' is t, regardless of the value of
`gdb-show-main', the layout below will appear unless
`gdb-use-separate-io-buffer' is nil when the source buffer
occupies the full width of the frame.  Keybindings are shown in
some of the buffers.

Watch expressions appear in the speedbar/slowbar.

The following commands help control operation :

`gdb-many-windows'    - Toggle the number of windows gdb uses.
`gdb-restore-windows' - To restore the window layout.

See Info node `(emacs)GDB Graphical Interface' for a more
detailed description of this mode.


+----------------------------------------------------------------------+
|                               GDB Toolbar                            |
+-----------------------------------+----------------------------------+
| GUD buffer (I/O of GDB)           | Locals buffer                    |
|                                   |                                  |
|                                   |                                  |
|                                   |                                  |
+-----------------------------------+----------------------------------+
| Source buffer                     | I/O buffer (of debugged program) |
|                                   | (comint-mode)                    |
|                                   |                                  |
|                                   |                                  |
|                                   |                                  |
|                                   |                                  |
|                                   |                                  |
|                                   |                                  |
+-----------------------------------+----------------------------------+
| Stack buffer                      | Breakpoints buffer               |
| RET      gdb-frames-select        | SPC    gdb-toggle-breakpoint     |
|                                   | RET    gdb-goto-breakpoint       |
|                                   | D      gdb-delete-breakpoint     |
+-----------------------------------+----------------------------------+"
  ;;
  (interactive (list (gud-query-cmdline 'gdbmi)))
  ;;
  ;; Let's start with a basic gud-gdb buffer and then modify it a bit.
  (gdb command-line)
  ;;
  (setq gdb-debug-ring nil)
  (set (make-local-variable 'gud-minor-mode) 'gdbmi)
  (set (make-local-variable 'gud-marker-filter) 'gud-gdbmi-marker-filter)
  ;;
  (gud-def gud-step   "-exec-step %p"              "\C-s" 
	   "Step one source line with display.")
  (gud-def gud-stepi  "-exec-step-instruction %p"  "\C-i"
	   "Step one instruction with display.")
  (gud-def gud-next   "-exec-next %p"              "\C-n"
	   "Step one line (skip functions).")
  (gud-def gud-cont   "-exec-continue"             "\C-r"
	   "Continue with display.")
  (gud-def gud-finish "-exec-finish"               "\C-f"
	   "Finish executing current function.")
  (gud-def gud-run    "-exec-run"	     nil    "Run the program.")
  (gud-def gud-break (if (not (string-equal mode-name "Machine"))
			 (gud-call "break %f:%l" arg)
		       (save-excursion
			 (beginning-of-line)
			 (forward-char 2)
			 (gud-call "break *%a" arg)))
	   "\C-b" "Set breakpoint at current line or address.")
  ;;
  (gud-def gud-remove (if (not (string-equal mode-name "Machine"))
			  (gud-call "clear %f:%l" arg)
			(save-excursion
			  (beginning-of-line)
			  (forward-char 2)
			  (gud-call "clear *%a" arg)))
	   "\C-d" "Remove breakpoint at current line or address.")
  ;;
  (gud-def gud-until  (if (not (string-equal mode-name "Machine"))
			  (gud-call "-exec-until %f:%l" arg)
			(save-excursion
			  (beginning-of-line)
			  (forward-char 2)
			  (gud-call "-exec-until *%a" arg)))
	   "\C-u" "Continue to current line or address.")

  ;; For debugging Emacs only.
  (gud-def gud-pp
	   (gud-call
	    (concat
	     "pp1 " (if (eq (buffer-local-value
			     'major-mode (window-buffer)) 'speedbar-mode)
			(gdb-find-watch-expression) "%e")) arg)
	   nil   "Print the emacs s-expression.")

  ;; temporary hack - this will override previous definition.
  (defun gud-stop-subjob ()
    (interactive)
    (gdb-input (list "-exec-interrupt\n" 'ignore)))

  (define-key gud-minor-mode-map [left-margin mouse-1]
    'gdb-mouse-set-clear-breakpoint)
  (define-key gud-minor-mode-map [left-fringe mouse-1]
    'gdb-mouse-set-clear-breakpoint)
   (define-key gud-minor-mode-map [left-margin C-mouse-1]
    'gdb-mouse-toggle-breakpoint-margin)
  (define-key gud-minor-mode-map [left-fringe C-mouse-1]
    'gdb-mouse-toggle-breakpoint-fringe)

  (define-key gud-minor-mode-map [left-margin drag-mouse-1]
    'gdb-mouse-until)
  (define-key gud-minor-mode-map [left-fringe drag-mouse-1]
    'gdb-mouse-until)
  (define-key gud-minor-mode-map [left-margin mouse-3]
    'gdb-mouse-until)
  (define-key gud-minor-mode-map [left-fringe mouse-3]
    'gdb-mouse-until)

  (define-key gud-minor-mode-map [left-margin C-drag-mouse-1]
    'gdb-mouse-jump)
  (define-key gud-minor-mode-map [left-fringe C-drag-mouse-1]
    'gdb-mouse-jump)
  (define-key gud-minor-mode-map [left-fringe C-mouse-3]
    'gdb-mouse-jump)
  (define-key gud-minor-mode-map [left-margin C-mouse-3]
    'gdb-mouse-jump)

  (setq comint-input-sender 'gdbmi-send)
  ;;
  ;; (re-)initialise
  (setq gdb-pc-address (if gdb-show-main "main" nil))
  (setq gdb-previous-frame-address nil
	gdb-memory-address "main"
	gdb-previous-frame nil
	gdb-selected-frame nil
	gdb-frame-number nil
	gdb-var-list nil
	gdb-pending-triggers nil
	gdb-output-sink 'user
	gdb-location-alist nil
	gdb-source-file-list nil
	gdb-last-command nil
	gdb-token-number 0
	gdb-handler-alist '()
	gdb-handler-number nil
	gdb-prompt-name nil
	gdb-first-done-or-error t
	gdb-buffer-fringe-width (car (window-fringes))
	gdb-source-window nil
	gdb-inferior-status nil
	gdb-continuation nil)
  ;;
  (setq gdb-buffer-type 'gdbmi)
  ;;
  (gdb-force-mode-line-update 
   (propertize "initializing..." 'face font-lock-variable-name-face))

  (when gdb-use-separate-io-buffer
    (gdb-get-buffer-create 'gdb-inferior-io)
    (gdb-clear-inferior-io)
    (set-process-filter (get-process "gdb-inferior") 'gdb-inferior-filter)
    (gdb-input
     ;; Needs GDB 6.4 onwards
     (list (concat "-inferior-tty-set "
		   (process-tty-name (get-process "gdb-inferior")) "\n")
	   'ignore)))
  (if (eq window-system 'w32)
      (gdb-input (list "-gdb-set new-console off\n" 'ignore)))
  (gdb-input (list "-gdb-set height 0\n" 'ignore))
  ;; find source file and compilation directory here
  (gdb-input
   ; Needs GDB 6.2 onwards.
   (list "-file-list-exec-source-files\n" 'gdb-get-source-file-list))
  (gdb-input
   ; Needs GDB 6.0 onwards.
   (list "-file-list-exec-source-file\n" 'gdb-get-source-file))
  (gdb-input
   (list "-data-list-register-names\n" 'gdb-get-register-names))
  (gdb-input
   (list "-gdb-show prompt\n" 'gdb-get-prompt))
  ;;
  (run-hooks 'gdbmi-mode-hook))

(defun gdb-mouse-until (event)
  "Continue running until a source line past the current line.
The destination source line can be selected either by clicking with mouse-2
on the fringe/margin or dragging the arrow with mouse-1 (default bindings)."
  (interactive "e")
  (if gud-overlay-arrow-position
      (let ((start (event-start event))
	    (end  (event-end event))
	    (buffer (marker-buffer gud-overlay-arrow-position)) (line))
	(if (not (string-match "Machine" mode-name))
	    (if (equal buffer (window-buffer (posn-window end)))
		(with-current-buffer buffer
		  (when (or (equal start end)
			    (equal (posn-point start)
				   (marker-position
				    gud-overlay-arrow-position)))
		    (setq line (line-number-at-pos (posn-point end)))
		    (gud-call (concat "-exec-until " (number-to-string line))))))
	  (if (equal (marker-buffer gdb-overlay-arrow-position)
		     (window-buffer (posn-window end)))
	      (when (or (equal start end)
			(equal (posn-point start)
			       (marker-position
				gdb-overlay-arrow-position)))
		(save-excursion
		  (goto-line (line-number-at-pos (posn-point end)))
		  (forward-char 2)
		  (gud-call (concat "-exec-until *%a")))))))))

(defun gdb-mouse-jump (event)
  "Set execution address/line.
The destination source line can be selected either by clicking with mouse-2
on the fringe/margin or dragging the arrow with mouse-1 (default bindings).
Unlike gdb-mouse-until the destination address can be before the current
line, and no execution takes place."
  (interactive "e")
  (if gud-overlay-arrow-position
      (let ((start (event-start event))
	    (end  (event-end event))
	    (buffer (marker-buffer gud-overlay-arrow-position)) (line))
	(if (not (string-match "Machine" mode-name))
	    (if (equal buffer (window-buffer (posn-window end)))
		(with-current-buffer buffer
		  (when (or (equal start end)
			    (equal (posn-point start)
				   (marker-position
				    gud-overlay-arrow-position)))
		    (setq line (line-number-at-pos (posn-point end)))
	   (progn (gud-call (concat "tbreak " (number-to-string line)))
		  (gud-call (concat "jump " (number-to-string line)))))))
	  (if (equal (marker-buffer gdb-overlay-arrow-position)
		     (window-buffer (posn-window end)))
	      (when (or (equal start end)
			(equal (posn-point start)
			       (marker-position
				gdb-overlay-arrow-position)))
		(save-excursion
		  (goto-line (line-number-at-pos (posn-point end)))
		  (forward-char 2)
		  (progn
		    (gud-call (concat "tbreak *%a"))
		    (gud-call (concat "jump *%a"))))))))))

(defvar gdb-define-alist nil "Alist of #define directives for GUD tooltips.")

(defun gdb-create-define-alist ()
  "Create an alist of #define directives for GUD tooltips."
  (let* ((file (buffer-file-name))
	 (output
	  (with-output-to-string
	    (with-current-buffer standard-output
	      (call-process shell-file-name
			    (if (file-exists-p file) file nil)
			    (list t nil) nil "-c"
			    (concat gdb-cpp-define-alist-program " "
				    gdb-cpp-define-alist-flags)))))
	(define-list (split-string output "\n" t))
	(name))
    (setq gdb-define-alist nil)
    (dolist (define define-list)
      (setq name (nth 1 (split-string define "[( ]")))
      (push (cons name define) gdb-define-alist))))

(defun gdb-tooltip-print (expr)
  (tooltip-show
   (with-current-buffer (gdb-get-buffer 'gdb-partial-output-buffer)
     (goto-char (point-min))
     (let ((string
	    (if (search-forward "=" nil t)
		(concat expr (buffer-substring (- (point) 2) (point-max)))
	      (buffer-string))))
       ;; remove newline for gud-tooltip-echo-area
       (substring string 0 (- (length string) 1))))
   (or gud-tooltip-echo-area tooltip-use-echo-area)))

;; If expr is a macro for a function don't print because of possible dangerous
;; side-effects. Also printing a function within a tooltip generates an
;; unexpected starting annotation (phase error).
(defun gdb-tooltip-print-1 (expr)
  (with-current-buffer (gdb-get-buffer 'gdb-partial-output-buffer)
    (goto-char (point-min))
    (if (search-forward "expands to: " nil t)
	(unless (looking-at "\\S-+.*(.*).*")
	  (gdb-input
	   (list  (concat "print " expr "\n")
		  `(lambda () (gdb-tooltip-print ,expr))))))))

(defcustom gdb-speedbar-auto-raise nil
  "If non-nil raise speedbar every time display of watch expressions is\
 updated."
  :type 'boolean
  :group 'gud
  :version "22.1")

(defun gdb-speedbar-auto-raise (arg)
  "Toggle automatic raising of the speedbar for watch expressions.
With arg, automatically raise speedbar iff arg is positive."
  (interactive "P")
  (setq gdb-speedbar-auto-raise
	(if (null arg)
	    (not gdb-speedbar-auto-raise)
	  (> (prefix-numeric-value arg) 0)))
  (message (format "Auto raising %sabled"
		   (if gdb-speedbar-auto-raise "en" "dis"))))

(defcustom gdb-use-colon-colon-notation nil
  "If non-nil use FUN::VAR format to display variables in the speedbar."
  :type 'boolean
  :group 'gud
  :version "22.1")

(define-key gud-minor-mode-map "\C-c\C-w" 'gud-watch)
(define-key global-map (concat gud-key-prefix "\C-w") 'gud-watch)

(defun gud-watch (&optional arg event)
  "Watch expression at point.
With arg, enter name of variable to be watched in the minibuffer."
  (interactive (list current-prefix-arg last-input-event))
  (let ((minor-mode (buffer-local-value 'gud-minor-mode gud-comint-buffer)))
    (if (memq minor-mode '(gdbmi gdba))
	(progn
	  (if event (posn-set-point (event-end event)))
	  (require 'tooltip)
	  (save-selected-window
	    (let ((expr
		   (if arg
		       (completing-read "Name of variable: "
					'gud-gdb-complete-command)
		     (if (and transient-mark-mode mark-active)
			 (buffer-substring (region-beginning) (region-end))
		       (tooltip-identifier-from-point (point))))))
	      (speedbar 1)
	      (catch 'already-watched
		(dolist (var gdb-var-list)
		  (unless (string-match "\\." (car var))
		    (if (string-equal expr (nth 1 var))
			(throw 'already-watched nil))))
		(set-text-properties 0 (length expr) nil expr)
		(gdb-input
		 (list (concat"-var-create - * "  expr "\n")
		       `(lambda () (gdb-var-create-handler ,expr))))))))
      (message "gud-watch is a no-op in this mode."))))

(defconst gdb-var-create-regexp
  "name=\"\\(.*?\\)\",numchild=\"\\(.*?\\)\",type=\"\\(.*?\\)\"")

(defun gdb-var-create-handler (expr)
  (goto-char (point-min))
  (if (re-search-forward gdb-var-create-regexp nil t)
      (let ((var (list
		  (match-string 1)
		  (if (and (string-equal gdb-current-language "c")
			   gdb-use-colon-colon-notation gdb-selected-frame)
		      (setq expr (concat gdb-selected-frame "::" expr))
		    expr)
		  (match-string 2)
		  (match-string 3)
		  nil nil)))
	(push var gdb-var-list)
	(unless (string-equal
		 speedbar-initial-expansion-list-name "GUD")
	  (speedbar-change-initial-expansion-list "GUD"))
	(gdb-input
	 (list
	  (concat "-var-evaluate-expression " (car var) "\n")
	  `(lambda () (gdb-var-evaluate-expression-handler
		       ,(car var) nil)))))
    (message-box "No symbol \"%s\" in current context." expr)))

(defun gdb-speedbar-update ()
  (when (and (boundp 'speedbar-frame) (frame-live-p speedbar-frame))
    ;; Dummy command to update speedbar even when idle.
    (gdb-input (list "server pwd\n" 'gdb-speedbar-timer-fn))
    ;; Keep gdb-pending-triggers non-nil till end.
    (push 'gdb-speedbar-timer gdb-pending-triggers)))

(defun gdb-speedbar-timer-fn ()
  (setq gdb-pending-triggers
	(delq 'gdb-speedbar-timer gdb-pending-triggers))
  (speedbar-timer-fn))

(defun gdb-var-evaluate-expression-handler (varnum changed)
  (goto-char (point-min))
  (re-search-forward ".*value=\\(\".*\"\\)" nil t)
  (let ((var (assoc varnum gdb-var-list)))
    (when var
      (if changed (setcar (nthcdr 5 var) 'changed))
      (setcar (nthcdr 4 var) (read (match-string 1)))))
  (gdb-speedbar-update))

; Uses "-var-list-children --all-values".  Needs GDB 6.1 onwards.
(defun gdbmi-var-list-children (varnum)
  (gdb-input
   (list (concat "-var-update " varnum "\n") 'ignore))
  (gdb-input
   (list (concat "-var-list-children --all-values "  
		 varnum "\n")
	     `(lambda () (gdbmi-var-list-children-handler ,varnum)))))

(defconst gdbmi-var-list-children-regexp
  "child={.*?name=\"\\(.+?\\)\",.*?exp=\"\\(.+?\\)\",.*?\
numchild=\"\\(.+?\\)\",.*?value=\\(\".*?\"\\),.*?type=\"\\(.+?\\)\".*?}")

(defun gdbmi-var-list-children-handler (varnum)
  (goto-char (point-min))
  (let ((var-list nil))
    (catch 'child-already-watched
      (dolist (var gdb-var-list)
	(if (string-equal varnum (car var))
	    (progn
	      (push var var-list)
	      (while (re-search-forward gdbmi-var-list-children-regexp nil t)
		(let ((varchild (list (match-string 1)
				      (match-string 2)
				      (match-string 3)
				      (match-string 5)
				      (read (match-string 4))
				      nil)))
		  (if (assoc (car varchild) gdb-var-list)
		      (throw 'child-already-watched nil))
		  (push varchild var-list))))
	  (push var var-list)))
      (setq gdb-var-list (nreverse var-list))))
  (gdb-speedbar-update))

; Uses "-var-update --all-values".  Needs GDB 6.4 onwards.
(defun gdbmi-var-update ()
  (if (not (member 'gdb-var-update gdb-pending-triggers))
      (gdb-input
       (list "-var-update --all-values *\n" 'gdbmi-var-update-handler)))
  (push 'gdb-var-update gdb-pending-triggers))

(defconst gdbmi-var-update-regexp
  "{.*?name=\"\\(.*?\\)\",.*?\\(?:value=\\(\".*?\"\\),\\)?.*?\
in_scope=\"\\(.*?\\)\".*?}")

(defun gdbmi-var-update-handler ()
  (dolist (var gdb-var-list)
    (setcar (nthcdr 5 var) nil))
  (goto-char (point-min))
  (while (re-search-forward gdbmi-var-update-regexp nil t)
    (let* ((varnum (match-string 1))
	   (var (assoc varnum gdb-var-list)))
      (when var
	(if (string-equal (match-string 3) "false")
	    (setcar (nthcdr 5 var) 'out-of-scope)
	  (setcar (nthcdr 5 var) 'changed)
	  (setcar (nthcdr 4 var)
		  (read (match-string 2)))))))
  (setq gdb-pending-triggers
   (delq 'gdb-var-update gdb-pending-triggers))
  (gdb-speedbar-update))

(defun gdb-var-delete ()
  "Delete watch expression at point from the speedbar."
  (interactive)
  (let ((text (speedbar-line-text)))
    (string-match "\\(\\S-+\\)" text)
    (let ((expr (match-string 1 text)) var varnum)
      (catch 'expr-found
	(dolist (var1 gdb-var-list)
	  (when (string-equal expr (nth 1 var1))
	    (setq var var1)
	    (setq varnum (car var1))
	    (throw 'expr-found nil))))
      (unless (string-match "\\." (car var))
	(gdb-input
	 (list (concat "-var-delete " varnum "\n") 'ignore))
	(setq gdb-var-list (delq var gdb-var-list))
	(dolist (varchild gdb-var-list)
	  (if (string-match (concat (car var) "\\.") (car varchild))
	      (setq gdb-var-list (delq varchild gdb-var-list))))))))

(defun gdb-edit-value (text token indent)
  "Assign a value to a variable displayed in the speedbar."
  (let* ((var (nth (- (count-lines (point-min) (point)) 2) gdb-var-list))
	 (varnum (car var)) (value))
    (setq value (read-string "New value: "))
    (gdb-input
     (list (concat "-var-assign " varnum " " value "\n")
	   `(lambda () (gdb-edit-value-handler ,value))))))

(defun gdb-edit-value-handler (value)
  (goto-char (point-min))
  (if (re-search-forward gdb-error-regexp nil t)
      (message-box "Invalid number or expression (%s)" value)))

(defcustom gdb-show-changed-values t
  "If non-nil change the face of out of scope variables and changed values.
Out of scope variables are suppressed with `shadow' face.
Changed values are highlighted with the face `font-lock-warning-face'."
  :type 'boolean
  :group 'gud
  :version "22.1")

(defun gdb-speedbar-expand-node (text token indent)
  "Expand the node the user clicked on.
TEXT is the text of the button we clicked on, a + or - item.
TOKEN is data related to this node.
INDENT is the current indentation depth."
  (cond ((string-match "+" text)        ;expand this node
	 (gdbmi-var-list-children token))
	((string-match "-" text)	;contract this node
	 (dolist (var gdb-var-list)
	   (if (string-match (concat token "\\.") (car var))
	       (setq gdb-var-list (delq var gdb-var-list))))
	 (speedbar-change-expand-button-char ?+)
	 (speedbar-delete-subblock indent))
	(t (error "Ooops...  not sure what to do")))
  (speedbar-center-buffer-smartly))

(defun gdb-get-target-string ()
  (with-current-buffer gud-comint-buffer
    gud-target-name))


;;
;; gdb buffers.
;;
;; Each buffer has a TYPE -- a symbol that identifies the function
;; of that particular buffer.
;;
;; The usual gdb interaction buffer is given the type `gdbmi' and
;; is constructed specially.
;;
;; Others are constructed by gdb-get-buffer-create and
;; named according to the rules set forth in the gdb-buffer-rules-assoc

(defvar gdb-buffer-rules-assoc '())

(defun gdb-get-buffer (key)
  "Return the gdb buffer tagged with type KEY.
The key should be one of the cars in `gdb-buffer-rules-assoc'."
  (save-excursion
    (gdb-look-for-tagged-buffer key (buffer-list))))

(defun gdb-get-buffer-create (key)
  "Create a new gdb  buffer of the type specified by KEY.
The key should be one of the cars in `gdb-buffer-rules-assoc'."
  (or (gdb-get-buffer key)
      (let* ((rules (assoc key gdb-buffer-rules-assoc))
	     (name (funcall (gdb-rules-name-maker rules)))
	     (new (get-buffer-create name)))
	(with-current-buffer new
	  (let ((trigger))
	    (if (cdr (cdr rules))
		(setq trigger (funcall (car (cdr (cdr rules))))))
	    (setq gdb-buffer-type key)
	    (set (make-local-variable 'gud-minor-mode)
		 (with-current-buffer gud-comint-buffer gud-minor-mode))
	    (set (make-local-variable 'tool-bar-map) gud-tool-bar-map)
	    (if trigger (funcall trigger)))
	  new))))

(defun gdb-rules-name-maker (rules) (car (cdr rules)))

(defun gdb-look-for-tagged-buffer (key bufs)
  (let ((retval nil))
    (while (and (not retval) bufs)
      (set-buffer (car bufs))
      (if (eq gdb-buffer-type key)
	  (setq retval (car bufs)))
      (setq bufs (cdr bufs)))
    retval))

;;
;; This assoc maps buffer type symbols to rules.  Each rule is a list of
;; at least one and possible more functions.  The functions have these
;; roles in defining a buffer type:
;;
;;     NAME - Return a name for this  buffer type.
;;
;; The remaining function(s) are optional:
;;
;;     MODE - called in a new buffer with no arguments, should establish
;;	      the proper mode for the buffer.
;;

(defun gdb-set-buffer-rules (buffer-type &rest rules)
  (let ((binding (assoc buffer-type gdb-buffer-rules-assoc)))
    (if binding
	(setcdr binding rules)
      (push (cons buffer-type rules)
	    gdb-buffer-rules-assoc))))

;; GUD buffers are an exception to the rules
(gdb-set-buffer-rules 'gdbmi 'error)

;; Partial-output buffer : This accumulates output from a command executed on
;; behalf of emacs (rather than the user).
;;
(gdb-set-buffer-rules 'gdb-partial-output-buffer
		      'gdb-partial-output-name)

(defun gdb-partial-output-name ()
  (concat "*partial-output-"
	  (gdb-get-target-string)
	  "*"))


(gdb-set-buffer-rules 'gdb-inferior-io
		      'gdb-inferior-io-name
		      'gdb-inferior-io-mode)

(defun gdb-inferior-io-name ()
  (concat "*input/output of "
	  (gdb-get-target-string)
	  "*"))

(defun gdb-display-separate-io-buffer ()
  "Display IO of debugged program in a separate window."
  (interactive)
  (if gdb-use-separate-io-buffer
      (gdb-display-buffer
       (gdb-get-buffer-create 'gdb-inferior-io) t)))

(defconst gdb-frame-parameters
  '((height . 14) (width . 80)
    (unsplittable . t)
    (tool-bar-lines . nil)
    (menu-bar-lines . nil)
    (minibuffer . nil)))

(defun gdb-frame-separate-io-buffer ()
  "Display IO of debugged program in a new frame."
  (interactive)
  (if gdb-use-separate-io-buffer
      (let ((special-display-regexps (append special-display-regexps '(".*")))
	    (special-display-frame-alist gdb-frame-parameters))
	(display-buffer (gdb-get-buffer-create 'gdb-inferior-io)))))

(defvar gdb-inferior-io-mode-map
  (let ((map (make-sparse-keymap)))
    (define-key map "\C-c\C-c" 'gdb-separate-io-interrupt)
    (define-key map "\C-c\C-z" 'gdb-separate-io-stop)
    (define-key map "\C-c\C-\\" 'gdb-separate-io-quit)
    (define-key map "\C-c\C-d" 'gdb-separate-io-eof)
    (define-key map "\C-d" 'gdb-separate-io-eof)
    map))

(define-derived-mode gdb-inferior-io-mode comint-mode "Inferior I/O"
  "Major mode for gdb inferior-io."
  :syntax-table nil :abbrev-table nil
  ;; We want to use comint because it has various nifty and familiar
  ;; features.  We don't need a process, but comint wants one, so create
  ;; a dummy one.
  (make-comint-in-buffer
   "gdb-inferior" (current-buffer) "sleep" nil "1000000000"))

(defun gdb-inferior-filter (proc string)
  (unless (string-equal string "")
    (gdb-display-buffer (gdb-get-buffer-create 'gdb-inferior-io) t))
  (with-current-buffer (gdb-get-buffer-create 'gdb-inferior-io)
    (insert-before-markers string)))

(defun gdb-separate-io-interrupt ()
  "Interrupt the program being debugged."
  (interactive)
  (interrupt-process
   (get-buffer-process gud-comint-buffer) comint-ptyp))

(defun gdb-separate-io-quit ()
  "Send quit signal to the program being debugged."
  (interactive)
  (quit-process
   (get-buffer-process gud-comint-buffer) comint-ptyp))

(defun gdb-separate-io-stop ()
  "Stop the program being debugged."
  (interactive)
  (stop-process
   (get-buffer-process gud-comint-buffer) comint-ptyp))

(defun gdb-separate-io-eof ()
  "Send end-of-file to the program being debugged."
  (interactive)
  (process-send-eof
   (get-buffer-process gud-comint-buffer)))

(defun gdb-clear-inferior-io ()
  (with-current-buffer (gdb-get-buffer-create 'gdb-inferior-io)
    (erase-buffer)))


(defconst breakpoint-xpm-data
  "/* XPM */
static char *magick[] = {
/* columns rows colors chars-per-pixel */
\"10 10 2 1\",
\"  c red\",
\"+ c None\",
/* pixels */
\"+++    +++\",
\"++      ++\",
\"+        +\",
\"          \",
\"          \",
\"          \",
\"          \",
\"+        +\",
\"++      ++\",
\"+++    +++\",
};"
  "XPM data used for breakpoint icon.")

(defconst breakpoint-enabled-pbm-data
  "P1
10 10\",
0 0 0 0 1 1 1 1 0 0 0 0
0 0 0 1 1 1 1 1 1 0 0 0
0 0 1 1 1 1 1 1 1 1 0 0
0 1 1 1 1 1 1 1 1 1 1 0
0 1 1 1 1 1 1 1 1 1 1 0
0 1 1 1 1 1 1 1 1 1 1 0
0 1 1 1 1 1 1 1 1 1 1 0
0 0 1 1 1 1 1 1 1 1 0 0
0 0 0 1 1 1 1 1 1 0 0 0
0 0 0 0 1 1 1 1 0 0 0 0"
  "PBM data used for enabled breakpoint icon.")

(defconst breakpoint-disabled-pbm-data
  "P1
10 10\",
0 0 1 0 1 0 1 0 0 0
0 1 0 1 0 1 0 1 0 0
1 0 1 0 1 0 1 0 1 0
0 1 0 1 0 1 0 1 0 1
1 0 1 0 1 0 1 0 1 0
0 1 0 1 0 1 0 1 0 1
1 0 1 0 1 0 1 0 1 0
0 1 0 1 0 1 0 1 0 1
0 0 1 0 1 0 1 0 1 0
0 0 0 1 0 1 0 1 0 0"
  "PBM data used for disabled breakpoint icon.")

(defvar breakpoint-enabled-icon nil
  "Icon for enabled breakpoint in display margin.")

(defvar breakpoint-disabled-icon nil
  "Icon for disabled breakpoint in display margin.")

(and (display-images-p)
     ;; Bitmap for breakpoint in fringe
     (define-fringe-bitmap 'breakpoint
       "\x3c\x7e\xff\xff\xff\xff\x7e\x3c")
     ;; Bitmap for gud-overlay-arrow in fringe
     (define-fringe-bitmap 'hollow-right-triangle
       "\xe0\x90\x88\x84\x84\x88\x90\xe0"))

(defface breakpoint-enabled
  '((t
     :foreground "red"
     :weight bold))
  "Face for enabled breakpoint icon in fringe."
  :group 'gud)

(defface breakpoint-disabled
  ;; We use different values of grey for different background types,
  ;; so that on low-color displays it will end up as something visible
  ;; if it has to be approximated.
  '((((background dark))  :foreground "grey60")
    (((background light)) :foreground "grey40"))
  "Face for disabled breakpoint icon in fringe."
  :group 'gud)


(defun gdbmi-send (proc string)
  "A comint send filter for gdb."
  (with-current-buffer gud-comint-buffer
    (let ((inhibit-read-only t))
      (remove-text-properties (point-min) (point-max) '(face))))
  ;; mimic <RET> key to repeat previous command in GDB
  (if (not (string-match "^\\s+$" string))
      (setq gdb-last-command string)
    (if gdb-last-command (setq string gdb-last-command)))
  (if gdb-enable-debug
      (push (cons 'mi-send (concat string "\n")) gdb-debug-ring))
  (if (string-match "^-" string)
      ;; MI command
      (progn
	(setq gdb-first-done-or-error t)
	(process-send-string proc (concat string "\n")))
    ;; CLI command
    (if (string-match "\\\\$" string)
	(setq gdb-continuation (concat gdb-continuation string "\n"))
      (setq gdb-first-done-or-error t)
      (process-send-string proc (concat "-interpreter-exec console-quoted \""
					gdb-continuation string "\"\n"))
      (setq gdb-continuation nil))))

(defun gdb-input (item)
  (if gdb-enable-debug (push (cons 'send-item item) gdb-debug-ring))
  (setq gdb-token-number (1+ gdb-token-number))
  (setcar item (concat (number-to-string gdb-token-number) (car item)))
  (push (cons gdb-token-number (car (cdr item))) gdb-handler-alist)
  (process-send-string (get-buffer-process gud-comint-buffer)
		       (car item)))


(defcustom gud-gdbmi-command-name "gdb -async -interp=mi"
  "Default command to execute an executable under the GDB-MI debugger."
  :type 'string
  :group 'gud)

(defun gdb-resync()
  (setq gud-running nil)
  (setq gdb-output-sink 'user)
  (setq gdb-pending-triggers nil))

;; fullname added GDB 6.4+.
(defconst gdb-stopped-regexp
  "\\*stopped,\\(reason=\"\\(.*?\\)\",\\)?\\(exit-code=.*?\\|\
.*?file=\".*?\",fullname=\"\\(.*?\\)\",line=\"\\(.*?\\)\"\\)?.*?\n")

(defconst gdb-console-regexp "~\\(\".*?[^\\]\"\\)\n")

(defconst gdb-internals-regexp "&\\(\".*?\\n\"\\)\n")

(defconst gdb-gdb-regexp "(gdb) \n")

(defconst gdb-running-regexp (concat "\\^running\n" gdb-gdb-regexp))

(defconst gdb-error-regexp "\\^error,msg=\\(\".+\"\\)")

(defconst gdb-done-or-error-regexp
 (concat "\\(\\([0-9]*\\)\\^done,*\n*\\|\\([0-9]*\\)" gdb-error-regexp "\\)"))

(defun gdbmi-update ()
  "Update buffers showing status of debug session."

  ;; This call should be a temporary hack.  Eventually GDB should infer
  ;; the frame from gdb-stopped-regexp et al.
  (gdbmi-get-selected-frame)

  (gdbmi-invalidate-frames)
  ;; Regenerate breakpoints buffer in case it has been inadvertantly deleted.
  (gdb-get-buffer-create 'gdb-breakpoints-buffer)
  (gdbmi-invalidate-breakpoints)
  (gdb-get-changed-registers)
  (gdbmi-invalidate-registers)
  (gdbmi-invalidate-locals)
  (when (and (boundp 'speedbar-frame) (frame-live-p speedbar-frame))
    (dolist (var gdb-var-list)
      (setcar (nthcdr 5 var) nil))
    (gdbmi-var-update)))

;; GUD displays the selected GDB frame.  This might might not be the current
;; GDB frame (after up, down etc).  If no GDB frame is visible but the last
;; visited breakpoint is, use that window.
(defun gdb-display-source-buffer (buffer)
  (let* ((last-window (if gud-last-last-frame
			 (get-buffer-window
			  (gud-find-file (car gud-last-last-frame)))))
	 (source-window (or last-window
			    (if (and gdb-source-window
				     (window-live-p gdb-source-window))
				gdb-source-window))))
    (when source-window
      (setq gdb-source-window source-window)
      (set-window-buffer source-window buffer))
    source-window))

(defun gud-gdbmi-marker-filter (string)
  "Filter GDB/MI output."
  (when gdb-enable-debug
    (push (cons 'recv string) gdb-debug-ring)
    (if (> (length gdb-debug-ring) gdb-debug-ring-max)
	(setcdr (nthcdr (1- gdb-debug-ring-max) gdb-debug-ring) nil)))
  ;; Recall the left over gud-marker-acc from last time
  (setq gud-marker-acc (concat gud-marker-acc string))
  ;; Start accumulating output for the GUD buffer
  (let ((output "") (output-record) (preamble))

    (when (string-match gdb-running-regexp gud-marker-acc) 
      (setq gud-marker-acc (substring gud-marker-acc (match-end 0))
	    gud-running t)
      (setq gdb-inferior-status "running")
      (gdb-force-mode-line-update
       (propertize gdb-inferior-status 'face font-lock-type-face)))

    (when (string-match gdb-stopped-regexp gud-marker-acc)
      (let ((reason (match-string 2 gud-marker-acc))
	    (file (match-string 4 gud-marker-acc)))
	(when file
	  (setq
	   ;; Extract the frame position from the marker.
	   gud-last-frame (cons file
				(string-to-number
				 (match-string 5 gud-marker-acc))))
	  (gdbmi-update))

	(setq gud-marker-acc
	      (concat (substring gud-marker-acc 0 (match-beginning 0))
		      (substring gud-marker-acc (match-end 0)))
	      gud-running nil)
	(setq gdb-inferior-status (if reason reason "unknown"))
	(gdb-force-mode-line-update
	 (propertize gdb-inferior-status 'face font-lock-warning-face))))

    ;; process command outputs one by one.
    (while (string-match gdb-gdb-regexp gud-marker-acc)

      (setq output-record (substring gud-marker-acc 0 (match-beginning 0))
	    gud-marker-acc (substring gud-marker-acc (match-end 0)))

      (if (string-match gdb-done-or-error-regexp output-record)
	  
	  ;; Identify the handler from token number
	  (let ((match (or (match-string 2 output-record)
			   (match-string 3 output-record))))
	    
	    (if (string-equal match "")
		;; Output from command entered by user
		(progn
		  (setq gdb-output-sink 'user)
		  (setq gdb-handler-number nil)
		  ;; MI error - send to minibuffer
		  (if (match-string 3 output-record)
		      (message (read (match-string 4 output-record)))))
	      ;; Output from command from frontend.
	      (setq gdb-handler-number (string-to-number match))
	      (setq gdb-output-sink 'emacs))

	    (gdb-clear-partial-output)
	    ;; preamble should just be console, log stream or inferior output
	    (setq preamble (substring output-record 0 (match-beginning 0)))
	    (setq output-record (substring output-record (match-end 0)))
	    (when gdb-first-done-or-error
	      (gdbmi-update)
	      (setq gdb-first-done-or-error nil)))

	(setq preamble output-record
	      output-record ""
	      gdb-handler-number nil
              gud-running nil))

      ;; FIXME: Order of console, log stream and inferior output might be lost
      ;; with what follows:

      ;; Remove the trimmings from the console stream and send to GUD buffer
      ;; (frontend MI commands should not print to this stream)
      (while (string-match gdb-console-regexp preamble) 
	(setq output
	      (gdbmi-concat-output
	       output
	       (read (match-string 1 preamble)))

	      preamble
	      (concat (substring preamble 0 (match-beginning 0))
		      (substring preamble (match-end 0)))))

      ;; Remove the trimmings from log stream containing debugging messages
      ;; being produced by GDB's internals, use warning face and send to GUD
      ;; buffer.
      (while (string-match gdb-internals-regexp preamble) 
	(setq output
	      (gdbmi-concat-output
	       output
	       (let ((error-message
		      (read (match-string 1 preamble))))
		 (put-text-property
		  0 (length error-message)
		  'face font-lock-warning-face
		  error-message)
		 error-message))

	      preamble (substring preamble (match-end 0))))

      ;; Preamble here should just be inferior output.
      (setq output (gdbmi-concat-output output preamble))
      (setq output (gdbmi-concat-output output output-record))

      (when gdb-handler-number
	(with-current-buffer
	    (gdb-get-buffer-create 'gdb-partial-output-buffer)
	  (funcall (cdr (assoc gdb-handler-number gdb-handler-alist))))
	(setq gdb-handler-alist
	      (assq-delete-all gdb-handler-number gdb-handler-alist))))

    (unless gdb-handler-alist
      (setq output (gdbmi-concat-output output gud-marker-acc))
      (unless gud-running (setq output (concat output gdb-prompt-name)))
      (setq gud-marker-acc "")
      (setq gdb-output-sink 'user))

   output))

(defun gdbmi-concat-output (so-far new)
  (let ((sink gdb-output-sink))
    (cond
     ((eq sink 'user) (concat so-far new))
     ((eq sink 'emacs)
      (gdb-append-to-partial-output new)
      so-far))))

(defun gdb-append-to-partial-output (string)
  (with-current-buffer (gdb-get-buffer-create 'gdb-partial-output-buffer)
    (goto-char (point-max))
    (insert string)))

(defun gdb-clear-partial-output ()
  (with-current-buffer (gdb-get-buffer-create 'gdb-partial-output-buffer)
    (erase-buffer)))

;; NAME is the function name. DEMAND-PREDICATE tests if output is really needed.
;; GDB-COMMAND is a string of such.  OUTPUT-HANDLER is the function bound to the
;; current input.

(defmacro def-gdb-auto-update-trigger (name demand-predicate gdb-command
					    output-handler)
  `(defun ,name (&optional ignored)
     (if (and ,demand-predicate
	      (not (member ',name
			   gdb-pending-triggers)))
	 (progn
	   (gdb-input
	    (list ,gdb-command ',output-handler))
	   (push ',name gdb-pending-triggers)))))

(defmacro def-gdb-auto-update-handler (name trigger buf-key custom-defun)
  `(defun ,name ()
     (setq gdb-pending-triggers
      (delq ',trigger
	    gdb-pending-triggers))
     (let ((buf (gdb-get-buffer ',buf-key)))
       (and buf
	    (with-current-buffer buf
	      (let* ((window (get-buffer-window buf 0))
		     (p (window-point window))
		    (buffer-read-only nil))
		(erase-buffer)
		(insert-buffer-substring (gdb-get-buffer-create
					  'gdb-partial-output-buffer))
		(set-window-point window p)))))
     ;; put customisation here
     (,custom-defun)))


;; Breakpoint buffer : This displays the output of `-break-list'.
;;
(gdb-set-buffer-rules 'gdb-breakpoints-buffer
		      'gdb-breakpoints-buffer-name
		      'gdb-breakpoints-mode)

(def-gdb-auto-update-trigger gdbmi-invalidate-breakpoints
  (gdb-get-buffer 'gdb-breakpoints-buffer)
  "-break-list\n"
  gdb-break-list-handler)

(defconst gdb-break-list-regexp
"number=\"\\(.*?\\)\",type=\"\\(.*?\\)\",disp=\"\\(.*?\\)\",enabled=\"\\(.\\)\",\
addr=\"\\(.*?\\)\",\
\\(?:func=\"\\(.*?\\)\",file=\"\\(.*?\\)\",fullname=\".*?\",line=\"\\(.*?\\)\",\
\\|\\(?:what=\"\\(.*?\\)\",\\)*\\)times=\"\\(.*?\\)\"")

(defun gdb-break-list-handler ()
  (setq gdb-pending-triggers (delq 'gdbmi-invalidate-breakpoints
				  gdb-pending-triggers))
  (let ((breakpoint) (breakpoints-list))
    (with-current-buffer (gdb-get-buffer-create 'gdb-partial-output-buffer)
      (goto-char (point-min))
      (while (re-search-forward gdb-break-list-regexp nil t)
	(let ((breakpoint (list (match-string 1)
				(match-string 2)
				(match-string 3)
				(match-string 4)
				(match-string 5)
				(match-string 6)
				(match-string 7)
				(match-string 8)
				(match-string 9)
				(match-string 10))))
	  (push breakpoint breakpoints-list))))
    (let ((buf (gdb-get-buffer 'gdb-breakpoints-buffer)))
      (and buf (with-current-buffer buf
		 (let ((p (point))
		       (buffer-read-only nil))
		   (erase-buffer)
		   (insert "Num Type           Disp Enb Hits Addr       What\n")
		   (dolist (breakpoint breakpoints-list)
		     (insert
		      (concat
		       (nth 0 breakpoint) "   "
		       (nth 1 breakpoint) "     "
		       (nth 2 breakpoint)  " "
		       (propertize (nth 3 breakpoint) 
			  'face (if (eq (string-to-char (nth 3 breakpoint)) ?y)
				    font-lock-warning-face
				  font-lock-type-face)) "   "
		       (nth 9 breakpoint) " "
		       (nth 4 breakpoint) " "
		       (if (nth 5 breakpoint)
			   (concat "in " (nth 5 breakpoint) " at " (nth 6 breakpoint) ":" (nth 7 breakpoint) "\n")
			 (concat (nth 8 breakpoint) "\n")))))
		   (goto-char p))))))
  (gdb-break-list-custom))

;; Put breakpoint icons in relevant margins (even those set in the GUD buffer).
(defun gdb-break-list-custom ()
  (let ((flag) (bptno))
    ;; Remove all breakpoint-icons in source buffers but not assembler buffer.
    (dolist (buffer (buffer-list))
      (with-current-buffer buffer
	(if (and (eq gud-minor-mode 'gdbmi)
		 (not (string-match "\\`\\*.+\\*\\'" (buffer-name))))
	    (gdb-remove-breakpoint-icons (point-min) (point-max)))))
    (with-current-buffer (gdb-get-buffer 'gdb-breakpoints-buffer)
      (save-excursion
	(goto-char (point-min))
	(while (< (point) (- (point-max) 1))
	  (forward-line 1)
	  (if (looking-at "[^\t].*?breakpoint")
	      (progn
		(looking-at "\\([0-9]+\\)\\s-+\\S-+\\s-+\\S-+\\s-+\\(.\\)")
		(setq bptno (match-string 1))
		(setq flag (char-after (match-beginning 2)))
		(beginning-of-line)
		(if (re-search-forward " in \\(.*\\) at\\s-+" nil t)
		    (progn
		      (let ((buffer-read-only nil))
			(add-text-properties (match-beginning 1) (match-end 1)
					     '(face font-lock-function-name-face)))
		      (looking-at "\\(\\S-+\\):\\([0-9]+\\)")
		      (let ((line (match-string 2)) (buffer-read-only nil)
			    (file (match-string 1)))
			(add-text-properties (line-beginning-position)
					     (line-end-position)
			 '(mouse-face highlight
			   help-echo "mouse-2, RET: visit breakpoint"))
			(unless (file-exists-p file)
			   (setq file (cdr (assoc bptno gdb-location-alist))))
			(if (and file
				 (not (string-equal file "File not found")))
			    (with-current-buffer
				(find-file-noselect file 'nowarn)
			      (set (make-local-variable 'gud-minor-mode)
				   'gdbmi)
			      (set (make-local-variable 'tool-bar-map)
				   gud-tool-bar-map)
			      ;; Only want one breakpoint icon at each location.
			      (save-excursion
				(goto-line (string-to-number line))
				(gdb-put-breakpoint-icon (eq flag ?y) bptno)))
			  (gdb-input
			   (list (concat "list "
					 (match-string-no-properties 3) ":1\n")
				 'ignore))
			  (gdb-input
			   (list "-file-list-exec-source-file\n"
				 `(lambda () (gdbmi-get-location
					      ,bptno ,line ,flag))))))))))))
      (end-of-line))))

(defvar gdb-source-file-regexp "fullname=\"\\(.*?\\)\"")

(defun gdbmi-get-location (bptno line flag)
  "Find the directory containing the relevant source file.
Put in buffer and place breakpoint icon."
  (goto-char (point-min))
  (catch 'file-not-found
    (if (re-search-forward gdb-source-file-regexp nil t)
	(delete (cons bptno "File not found") gdb-location-alist)
	(push (cons bptno (match-string 1)) gdb-location-alist)
      (gdb-resync)
      (unless (assoc bptno gdb-location-alist)
	(push (cons bptno "File not found") gdb-location-alist)
	(message-box "Cannot find source file for breakpoint location.
Add directory to search path for source files using the GDB command, dir."))
      (throw 'file-not-found nil))
    (with-current-buffer
	(find-file-noselect (match-string 1))
      (save-current-buffer
	(set (make-local-variable 'gud-minor-mode) 'gdbmi)
	(set (make-local-variable 'tool-bar-map) gud-tool-bar-map))
      ;; only want one breakpoint icon at each location
      (save-excursion
	(goto-line (string-to-number line))
	(gdb-put-breakpoint-icon (eq flag ?y) bptno)))))

(add-hook 'find-file-hook 'gdb-find-file-hook)

(defun gdb-find-file-hook ()
  "Set up buffer for debugging if file is part of the source code
of the current session."
  (if (and (buffer-name gud-comint-buffer)
	   ;; in case gud or gdb-ui is just loaded
	   gud-comint-buffer
	   (memq (buffer-local-value 'gud-minor-mode gud-comint-buffer)
	       '(gdba gdbmi)))
      (if (member buffer-file-name gdb-source-file-list)
	  (with-current-buffer (find-buffer-visiting buffer-file-name)
	    (set (make-local-variable 'gud-minor-mode)
		 (buffer-local-value 'gud-minor-mode gud-comint-buffer))
	    (set (make-local-variable 'tool-bar-map) gud-tool-bar-map)))))

(defun gdb-mouse-set-clear-breakpoint (event)
  "Set/clear breakpoint in left fringe/margin with mouse click."
  (interactive "e")
  (mouse-minibuffer-check event)
  (let ((posn (event-end event)))
    (if (numberp (posn-point posn))
	(with-selected-window (posn-window posn)
	  (save-excursion
	    (goto-char (posn-point posn))
	    (if (or (posn-object posn)
		    (eq (car (fringe-bitmaps-at-pos (posn-point posn)))
			'breakpoint))
		(gud-remove nil)
	      (gud-break nil)))))))

(defun gdb-mouse-toggle-breakpoint-margin (event)
  "Enable/disable breakpoint in left margin with mouse click."
  (interactive "e")
  (mouse-minibuffer-check event)
  (let ((posn (event-end event)))
    (if (numberp (posn-point posn))
	(with-selected-window (posn-window posn)
	  (save-excursion
	    (goto-char (posn-point posn))
	    (if	(posn-object posn)
		(gud-basic-call
		 (let ((bptno (get-text-property
			       0 'gdb-bptno (car (posn-string posn)))))
		   (concat
		    (if (get-text-property
			 0 'gdb-enabled (car (posn-string posn)))
			"-break-disable "
		      "-break-enable ")
		    bptno "\n")))))))))

(defun gdb-mouse-toggle-breakpoint-fringe (event)
  "Enable/disable breakpoint in left fringe with mouse click."
  (interactive "e")
  (mouse-minibuffer-check event)
  (let* ((posn (event-end event))
	 (pos (posn-point posn))
	 obj)
    (when (numberp pos)
      (with-selected-window (posn-window posn)
	(save-excursion
	  (set-buffer (window-buffer (selected-window)))
	  (goto-char pos)
	  (dolist (overlay (overlays-in pos pos))
	    (when (overlay-get overlay 'put-break)
	      (setq obj (overlay-get overlay 'before-string))))
	  (when (stringp obj)
	    (gud-basic-call
	     (concat
	      (if (get-text-property 0 'gdb-enabled obj)
		  "-break-disable "
		"-break-enable ")
	       (get-text-property 0 'gdb-bptno obj) "\n"))))))))

(defun gdb-breakpoints-buffer-name ()
  (with-current-buffer gud-comint-buffer
    (concat "*breakpoints of " (gdb-get-target-string) "*")))

(defun gdb-display-breakpoints-buffer ()
  "Display status of user-settable breakpoints."
  (interactive)
  (gdb-display-buffer
   (gdb-get-buffer-create 'gdb-breakpoints-buffer) t))

(defun gdb-frame-breakpoints-buffer ()
  "Display status of user-settable breakpoints in a new frame."
  (interactive)
  (let ((special-display-regexps (append special-display-regexps '(".*")))
	(special-display-frame-alist gdb-frame-parameters))
    (display-buffer (gdb-get-buffer-create 'gdb-breakpoints-buffer))))

(defvar gdb-breakpoints-mode-map
  (let ((map (make-sparse-keymap))
	(menu (make-sparse-keymap "Breakpoints")))
    (define-key menu [quit] '("Quit"   . gdb-delete-frame-or-window))
    (define-key menu [goto] '("Goto"   . gdb-goto-breakpoint))
    (define-key menu [delete] '("Delete" . gdb-delete-breakpoint))
    (define-key menu [toggle] '("Toggle" . gdb-toggle-breakpoint))
    (suppress-keymap map)
    (define-key map [menu-bar breakpoints] (cons "Breakpoints" menu))
    (define-key map " " 'gdb-toggle-breakpoint)
    (define-key map "D" 'gdb-delete-breakpoint)
    (define-key map "q" 'gdb-delete-frame-or-window)
    (define-key map "\r" 'gdb-goto-breakpoint)
    (define-key map [mouse-2] 'gdb-goto-breakpoint)
    (define-key map [follow-link] 'mouse-face)
    map))

(defun gdb-delete-frame-or-window ()
  "Delete frame if there is only one window.  Otherwise delete the window."
  (interactive)
  (if (one-window-p) (delete-frame)
    (delete-window)))

(defun gdb-breakpoints-mode ()
  "Major mode for gdb breakpoints.

\\{gdb-breakpoints-mode-map}"
  (kill-all-local-variables)
  (setq major-mode 'gdb-breakpoints-mode)
  (setq mode-name "Breakpoints")
  (use-local-map gdb-breakpoints-mode-map)
  (setq buffer-read-only t)
  (run-mode-hooks 'gdb-breakpoints-mode-hook)
  'gdbmi-invalidate-breakpoints)

(defconst gdbmi-breakpoint-regexp
  "\\([0-9]+\\).*?\\(?:point\\|catch\\s-+\\S-+\\)\\s-+\\S-+\\s-+\\(.\\)\\s-+")

(defun gdb-toggle-breakpoint ()
  "Enable/disable breakpoint at current line."
  (interactive)
  (save-excursion
    (beginning-of-line 1)
    (if (looking-at gdbmi-breakpoint-regexp)
	(gud-basic-call
	 (concat (if (eq ?y (char-after (match-beginning 2)))
		     "-break-disable "
		   "-break-enable ")
		 (match-string 1)))
      (error "Not recognized as break/watchpoint line"))))

(defun gdb-delete-breakpoint ()
  "Delete the breakpoint at current line."
  (interactive)
  (save-excursion
    (beginning-of-line 1)
    (if (looking-at gdbmi-breakpoint-regexp)
	(gud-basic-call (concat "-break-delete " (match-string 1)))
      (error "Not recognized as break/watchpoint line"))))

(defun gdb-goto-breakpoint (&optional event)
  "Display the breakpoint location specified at current line."
  (interactive (list last-input-event))
  (if event (posn-set-point (event-end event)))
  ;; Hack to stop gdb-goto-breakpoint displaying in GUD buffer.
  (let ((window (get-buffer-window gud-comint-buffer)))
    (if window (save-selected-window  (select-window window))))
  (save-excursion
    (beginning-of-line 1)
    (if (looking-at "\\([0-9]+\\) .+ in .+ at\\s-+\\(\\S-+\\):\\([0-9]+\\)")
	(let ((bptno (match-string 1))
	      (file  (match-string 2))
	      (line  (match-string 3)))
	  (save-selected-window
	    (let* ((buffer (find-file-noselect
			 (if (file-exists-p file) file
			   (cdr (assoc bptno gdb-location-alist)))))
		   (window (or (gdb-display-source-buffer buffer)
			       (display-buffer buffer))))
	      (setq gdb-source-window window)
	      (with-current-buffer buffer
		(goto-line (string-to-number line))
		(set-window-point window (point))))))
      (error "Not recognized as break/watchpoint line"))))


;; Frames buffer.  This displays a perpetually correct bactrack trace.
;;
(gdb-set-buffer-rules 'gdb-stack-buffer
		      'gdb-stack-buffer-name
		      'gdb-frames-mode)

(def-gdb-auto-update-trigger gdbmi-invalidate-frames
  (gdb-get-buffer 'gdb-stack-buffer)
  "-stack-list-frames\n"
  gdb-stack-list-frames-handler)

(defconst gdb-stack-list-frames-regexp
"level=\"\\(.*?\\)\",addr=\"\\(.*?\\)\",func=\"\\(.*?\\)\",\
\\(?:file=\".*?\",fullname=\"\\(.*?\\)\",line=\"\\(.*?\\)\"\\|\
from=\"\\(.*?\\)\"\\)")

(defun gdb-stack-list-frames-handler ()
  (setq gdb-pending-triggers (delq 'gdbmi-invalidate-frames
				  gdb-pending-triggers))
  (let ((frame nil)
	(call-stack nil))
    (with-current-buffer (gdb-get-buffer-create 'gdb-partial-output-buffer)
      (goto-char (point-min))
      (while (re-search-forward gdb-stack-list-frames-regexp nil t)
	(let ((frame (list (match-string 1)
			   (match-string 2)
			   (match-string 3)
			   (match-string 4)
			   (match-string 5)
			   (match-string 6))))
	  (push frame call-stack))))
    (let ((buf (gdb-get-buffer 'gdb-stack-buffer)))
      (and buf (with-current-buffer buf
		 (let ((p (point))
		       (buffer-read-only nil))
		   (erase-buffer)
		   (insert "Level\tAddr\tFunc\tFile:Line\n")
		   (dolist (frame (nreverse call-stack))
		     (insert
		      (concat
		       (nth 0 frame) "\t"
		       (nth 1 frame) "\t"
		       (propertize (nth 2 frame)
				   'face font-lock-function-name-face) "\t"
		       (if (nth 3 frame)
			   (concat "at "(nth 3 frame) ":" (nth 4 frame) "\n")
			 (concat "from " (nth 5 frame) "\n")))))
		   (goto-char p))))))
  (gdb-stack-list-frames-custom))

(defun gdb-stack-list-frames-custom ()
  (with-current-buffer (gdb-get-buffer 'gdb-stack-buffer)
    (save-excursion
      (let ((buffer-read-only nil))
	(goto-char (point-min))
	(forward-line 1)
	(while (< (point) (point-max))
	  (add-text-properties (point-at-bol) (point-at-eol)
			     '(mouse-face highlight
			       help-echo "mouse-2, RET: Select frame"))
	  (beginning-of-line)
	  (when (and (looking-at "^[0-9]+\\s-+\\S-+\\s-+\\(\\S-+\\)")
		     (equal (match-string 1) gdb-selected-frame))
		(if (> (car (window-fringes)) 0)
		    (progn
		      (or gdb-stack-position
			  (setq gdb-stack-position (make-marker)))
		      (set-marker gdb-stack-position (point)))
		  (set-marker gdb-stack-position nil)
		  (let ((bl (point-at-bol)))
		    (put-text-property bl (+ bl 4)
				       'face '(:inverse-video t)))))
	  (forward-line 1))))))

(defun gdb-stack-buffer-name ()
  (with-current-buffer gud-comint-buffer
    (concat "*stack frames of " (gdb-get-target-string) "*")))

(defun gdb-display-stack-buffer ()
  "Display backtrace of current stack."
  (interactive)
  (gdb-display-buffer
   (gdb-get-buffer-create 'gdb-stack-buffer) t))

(defun gdb-frame-stack-buffer ()
  "Display backtrace of current stack in a new frame."
  (interactive)
  (let ((special-display-regexps (append special-display-regexps '(".*")))
	(special-display-frame-alist gdb-frame-parameters))
    (display-buffer (gdb-get-buffer-create 'gdb-stack-buffer))))

(defvar gdb-frames-mode-map
  (let ((map (make-sparse-keymap)))
    (suppress-keymap map)
    (define-key map "q" 'kill-this-buffer)
    (define-key map "\r" 'gdb-frames-select)
    (define-key map [mouse-2] 'gdb-frames-select)
    (define-key map [follow-link] 'mouse-face)
    map))

(defun gdb-frames-mode ()
  "Major mode for gdb frames.

\\{gdb-frames-mode-map}"
  (kill-all-local-variables)
  (setq major-mode 'gdb-frames-mode)
  (setq mode-name "Frames")
  (setq gdb-stack-position nil)
  (add-to-list 'overlay-arrow-variable-list 'gdb-stack-position)
  (setq buffer-read-only t)
  (use-local-map gdb-frames-mode-map)
  (run-mode-hooks 'gdb-frames-mode-hook)
  'gdbmi-invalidate-frames)

(defun gdb-get-frame-number ()
  (save-excursion
    (end-of-line)
    (let* ((pos (re-search-backward "^\\([0-9]+\\)" nil t))
	   (n (or (and pos (match-string-no-properties 1)) "0")))
      n)))

(defun gdb-frames-select (&optional event)
  "Select the frame and display the relevant source."
  (interactive (list last-input-event))
  (if event (posn-set-point (event-end event)))
  (gud-basic-call (concat "-stack-select-frame " (gdb-get-frame-number))))


;; Locals buffer.
;; uses "-stack-list-locals --simple-values". Needs GDB 6.1 onwards.
(gdb-set-buffer-rules 'gdb-locals-buffer
		      'gdb-locals-buffer-name
		      'gdb-locals-mode)

(def-gdb-auto-update-trigger gdbmi-invalidate-locals
  (gdb-get-buffer 'gdb-locals-buffer)
  "-stack-list-locals --simple-values\n"
  gdb-stack-list-locals-handler)

(defconst gdb-stack-list-locals-regexp
  (concat "name=\"\\(.*?\\)\",type=\"\\(.*?\\)\""))

(defvar gdb-locals-watch-map
  (let ((map (make-sparse-keymap)))
    (suppress-keymap map)
    (define-key map "\r" 'gud-watch)
    (define-key map [mouse-2] 'gud-watch)
    map)
 "Keymap to create watch expression of a complex data type local variable.")

(defvar gdb-edit-locals-map-1
  (let ((map (make-sparse-keymap)))
    (suppress-keymap map)
    (define-key map "\r" 'gdb-edit-locals-value)
    (define-key map [mouse-2] 'gdb-edit-locals-value)
    map)
 "Keymap to edit value of a simple data type local variable.")

(defun gdb-edit-locals-value (&optional event)
  "Assign a value to a variable displayed in the locals buffer."
  (interactive (list last-input-event))
  (save-excursion
    (if event (posn-set-point (event-end event)))
    (beginning-of-line)
    (let* ((var (current-word))
	   (value (read-string (format "New value (%s): " var))))
      (gud-basic-call
       (concat  gdb-server-prefix"set variable " var " = " value)))))

;; Dont display values of arrays or structures.
;; These can be expanded using gud-watch.
(defun gdb-stack-list-locals-handler nil
  (setq gdb-pending-triggers (delq 'gdbmi-invalidate-locals
				  gdb-pending-triggers))
   (let (local locals-list)
    (goto-char (point-min))
    (while (re-search-forward gdb-stack-list-locals-regexp nil t)
      (let ((local (list (match-string 1)
			 (match-string 2)
			 nil)))
	(if (looking-at ",value=\\(\".*\"\\)}")
	    (setcar (nthcdr 2 local) (read (match-string 1))))
	(push local locals-list)))
    (let ((buf (gdb-get-buffer 'gdb-locals-buffer)))
      (and buf (with-current-buffer buf
		 (let* ((window (get-buffer-window buf 0))
			(start (window-start window))
			(p (window-point window))
			(buffer-read-only nil) (name) (value))
		   (erase-buffer)
		   (dolist (local locals-list)
		     (setq name (car local))
		     (setq value (nth 2 local))
		     (if (or (not value)
			     (string-match "\\0x" value))
		       (add-text-properties 0 (length name)
			    `(mouse-face highlight
			      help-echo "mouse-2: create watch expression"
			      local-map ,gdb-locals-watch-map)
			    name)
			 (add-text-properties 0 (length value)
			      `(mouse-face highlight
			        help-echo "mouse-2: edit value"
			        local-map ,gdb-edit-locals-map-1)
			      value))
		       (insert 
			(concat name "\t" (nth 1 local)
				"\t" (nth 2 local) "\n")))
		   (set-window-start window start)
		   (set-window-point window p)))))))

(defvar gdb-locals-mode-map
  (let ((map (make-sparse-keymap)))
    (suppress-keymap map)
    (define-key map "q" 'kill-this-buffer)
     map))

(defun gdb-locals-mode ()
  "Major mode for gdb locals.

\\{gdb-locals-mode-map}"
  (kill-all-local-variables)
  (setq major-mode 'gdb-locals-mode)
  (setq mode-name (concat "Locals:" gdb-selected-frame))
  (setq buffer-read-only t)
  (use-local-map gdb-locals-mode-map)
  (set (make-local-variable 'font-lock-defaults)
       '(gdb-locals-font-lock-keywords))
  (run-mode-hooks 'gdb-locals-mode-hook)
  'gdbmi-invalidate-locals)

(defun gdb-locals-buffer-name ()
  (with-current-buffer gud-comint-buffer
    (concat "*locals of " (gdb-get-target-string) "*")))

(defun gdb-display-locals-buffer ()
  "Display local variables of current stack and their values."
  (interactive)
  (gdb-display-buffer
   (gdb-get-buffer-create 'gdb-locals-buffer) t))

(defun gdb-frame-locals-buffer ()
  "Display local variables of current stack and their values in a new frame."
  (interactive)
  (let ((special-display-regexps (append special-display-regexps '(".*")))
	(special-display-frame-alist gdb-frame-parameters))
    (display-buffer (gdb-get-buffer-create 'gdb-locals-buffer))))


;; Registers buffer.
;;
(gdb-set-buffer-rules 'gdb-registers-buffer
		      'gdb-registers-buffer-name
		      'gdb-registers-mode)

(def-gdb-auto-update-trigger gdbmi-invalidate-registers
  (gdb-get-buffer 'gdb-registers-buffer)
  "-data-list-register-values x\n"
  gdb-data-list-register-values-handler)

(defconst gdb-data-list-register-values-regexp
  "number=\"\\(.*?\\)\",value=\"\\(.*?\\)\"")

(defun gdb-data-list-register-values-handler ()
  (setq gdb-pending-triggers (delq 'gdbmi-invalidate-registers
				   gdb-pending-triggers))
  (goto-char (point-min))
  (if (re-search-forward gdb-error-regexp nil t)
      (progn
	(let ((match nil))
	  (setq match (match-string 1))
	  (with-current-buffer (gdb-get-buffer 'gdb-registers-buffer)
	    (let ((buffer-read-only nil))
	      (erase-buffer)
	      (insert match)
	      (goto-char (point-min))))))
    (let ((register-list (reverse gdb-register-names))
	  (register nil) (register-string nil) (register-values nil))
      (goto-char (point-min))
      (while (re-search-forward gdb-data-list-register-values-regexp nil t)
	(setq register (pop register-list))
	(setq register-string (concat register "\t" (match-string 2) "\n"))
	(if (member (match-string 1) gdb-changed-registers)
	    (put-text-property 0 (length register-string)
			       'face 'font-lock-warning-face
			       register-string))
	(setq register-values
	      (concat register-values register-string)))
      (let ((buf (gdb-get-buffer 'gdb-registers-buffer)))
	(with-current-buffer buf
	  (let ((p (window-point (get-buffer-window buf 0)))
		(buffer-read-only nil))
	    (erase-buffer)
	    (insert register-values)
	    (set-window-point (get-buffer-window buf 0) p))))))
  (gdb-data-list-register-values-custom))

(defun gdb-data-list-register-values-custom ()
  (with-current-buffer (gdb-get-buffer 'gdb-registers-buffer)
    (save-excursion
      (let ((buffer-read-only nil)
	    bl)
	(goto-char (point-min))
	(while (< (point) (point-max))
	  (setq bl (line-beginning-position))
	  (when (looking-at "^[^\t]+")
	    (put-text-property bl (match-end 0)
			       'face font-lock-variable-name-face))
	  (forward-line 1))))))

(defvar gdb-registers-mode-map
  (let ((map (make-sparse-keymap)))
    (suppress-keymap map)
    (define-key map "q" 'kill-this-buffer)
     map))

(defun gdb-registers-mode ()
  "Major mode for gdb registers.

\\{gdb-registers-mode-map}"
  (kill-all-local-variables)
  (setq major-mode 'gdb-registers-mode)
  (setq mode-name "Registers:")
  (setq buffer-read-only t)
  (use-local-map gdb-registers-mode-map)
  (run-mode-hooks 'gdb-registers-mode-hook)
  'gdbmi-invalidate-registers)

(defun gdb-registers-buffer-name ()
  (with-current-buffer gud-comint-buffer
    (concat "*registers of " (gdb-get-target-string) "*")))

(defun gdb-display-registers-buffer ()
  "Display integer register contents."
  (interactive)
  (gdb-display-buffer
   (gdb-get-buffer-create 'gdb-registers-buffer) t))

(defun gdb-frame-registers-buffer ()
  "Display integer register contents in a new frame."
  (interactive)
  (let ((special-display-regexps (append special-display-regexps '(".*")))
	(special-display-frame-alist gdb-frame-parameters))
    (display-buffer (gdb-get-buffer-create 'gdb-registers-buffer))))

;; Needs GDB 6.4 onwards (used to fail with no stack).
(defun gdb-get-changed-registers ()
  (if (and (gdb-get-buffer 'gdb-registers-buffer)
	   (not (member 'gdb-get-changed-registers gdb-pending-triggers)))
      (progn
	(gdb-input
	 (list
	  "-data-list-changed-registers\n"
	  'gdb-get-changed-registers-handler))
	(push 'gdb-get-changed-registers gdb-pending-triggers))))

(defconst gdb-data-list-register-names-regexp "\"\\(.*?\\)\"")

(defun gdb-get-changed-registers-handler ()
  (setq gdb-pending-triggers
	(delq 'gdb-get-changed-registers gdb-pending-triggers))
  (setq gdb-changed-registers nil)
  (goto-char (point-min))
  (while (re-search-forward gdb-data-list-register-names-regexp nil t)
    (push (match-string 1) gdb-changed-registers)))

(defun gdb-get-register-names ()
  "Create a list of register names."
  (goto-char (point-min))
  (setq gdb-register-names nil)
  (while (re-search-forward gdb-data-list-register-names-regexp nil t)
    (push (match-string 1) gdb-register-names)))


(defun gdb-get-source-file-list ()
  "Create list of source files for current GDB session.
If buffers already exist for any of these files, gud-minor-mode
is set in them."
  (goto-char (point-min))
  (while (re-search-forward gdb-source-file-regexp nil t)
    (push (match-string 1) gdb-source-file-list))
  (dolist (buffer (buffer-list))
    (with-current-buffer buffer
      (when (member buffer-file-name gdb-source-file-list)
	(set (make-local-variable 'gud-minor-mode) 'gdbmi)
	(set (make-local-variable 'tool-bar-map) gud-tool-bar-map)
	(when gud-tooltip-mode
	  (make-local-variable 'gdb-define-alist)
	  (gdb-create-define-alist)
	  (add-hook 'after-save-hook 'gdb-create-define-alist nil t)))))
  (gdb-force-mode-line-update
   (propertize "ready" 'face font-lock-variable-name-face)))

(defun gdbmi-get-selected-frame ()
  (if (not (member 'gdbmi-get-selected-frame gdb-pending-triggers))
      (progn
	(gdb-input
	 (list "-stack-info-frame\n" 'gdbmi-frame-handler))
	(push 'gdbmi-get-selected-frame
	       gdb-pending-triggers))))

(defun gdbmi-frame-handler ()
  (setq gdb-pending-triggers
	(delq 'gdbmi-get-selected-frame gdb-pending-triggers))
  (goto-char (point-min))
  (when (re-search-forward gdb-stack-list-frames-regexp nil t)
    (setq gdb-frame-number (match-string 1))
    (setq gdb-pc-address (match-string 2))
    (setq gdb-selected-frame (match-string 3))
    (setq gud-last-frame
	  (cons (match-string 4) (string-to-number (match-string 5))))
    (gud-display-frame)
    (if (gdb-get-buffer 'gdb-locals-buffer)
	(with-current-buffer (gdb-get-buffer 'gdb-locals-buffer)
	  (setq mode-name (concat "Locals:" gdb-selected-frame))))
    (if (gdb-get-buffer 'gdb-assembler-buffer)
	(with-current-buffer (gdb-get-buffer 'gdb-assembler-buffer)
	  (setq mode-name (concat "Machine:" gdb-selected-frame))))
    (if gud-overlay-arrow-position
	(let ((buffer (marker-buffer gud-overlay-arrow-position))
	      (position (marker-position gud-overlay-arrow-position)))
	  (when buffer
	    (with-current-buffer buffer
	      (setq fringe-indicator-alist
		    (if (string-equal gdb-frame-number "0")
			nil
		      '((overlay-arrow . hollow-right-triangle))))
	      (setq gud-overlay-arrow-position (make-marker))
	      (set-marker gud-overlay-arrow-position position)))))))

(defvar gdb-prompt-name-regexp "value=\"\\(.*?\\)\"")

(defun gdb-get-prompt ()
  "Find prompt for GDB session."
  (goto-char (point-min))
  (setq gdb-prompt-name nil)
  (re-search-forward gdb-prompt-name-regexp nil t)
  (setq gdb-prompt-name (match-string 1)))
	       
;;;; Window management
(defun gdb-display-buffer (buf dedicated &optional size)
  (let ((answer (get-buffer-window buf 0))
	(must-split nil))
    (if answer
	(display-buffer buf nil 0)	;Raise the frame if necessary.
      ;; The buffer is not yet displayed.
      (pop-to-buffer gud-comint-buffer)	;Select the right frame.
      (let ((window (get-lru-window)))
	(if (and window
	    (not (eq window (get-buffer-window gud-comint-buffer))))
	    (progn
	      (set-window-buffer window buf)
	      (setq answer window))
	  (setq must-split t)))
      (if must-split
	  (let* ((largest (get-largest-window))
		 (cur-size (window-height largest))
		 (new-size (and size (< size cur-size) (- cur-size size))))
	    (setq answer (split-window largest new-size))
	    (set-window-buffer answer buf)
	    (set-window-dedicated-p answer dedicated)))
      answer)))


;;; Shared keymap initialization:

(let ((menu (make-sparse-keymap "GDB-Windows")))
  (define-key gud-menu-map [displays]
    `(menu-item "GDB-Windows" ,menu
		:visible (eq gud-minor-mode 'gdbmi)))
  (define-key menu [gdb] '("Gdb" . gdb-display-gdb-buffer))
;  (define-key menu [threads] '("Threads" . gdb-display-threads-buffer))
;  (define-key menu [memory] '("Memory" . gdb-display-memory-buffer))
;  (define-key menu [disassembly]
;    '("Disassembly" . gdb-display-assembler-buffer))
  (define-key menu [registers] '("Registers" . gdb-display-registers-buffer))
  (define-key menu [inferior]
    '(menu-item "Separate IO" gdb-display-separate-io-buffer
		:enable gdb-use-separate-io-buffer))
  (define-key menu [locals] '("Locals" . gdb-display-locals-buffer))
  (define-key menu [frames] '("Stack" . gdb-display-stack-buffer))
  (define-key menu [breakpoints]
    '("Breakpoints" . gdb-display-breakpoints-buffer)))

(let ((menu (make-sparse-keymap "GDB-Frames")))
  (define-key gud-menu-map [frames]
    `(menu-item "GDB-Frames" ,menu
		:visible (eq gud-minor-mode 'gdbmi)))
  (define-key menu [gdb] '("Gdb" . gdb-frame-gdb-buffer))
;  (define-key menu [threads] '("Threads" . gdb-frame-threads-buffer))
;  (define-key menu [memory] '("Memory" . gdb-frame-memory-buffer))
;  (define-key menu [disassembly] '("Disassembly" . gdb-frame-assembler-buffer))
  (define-key menu [registers] '("Registers" . gdb-frame-registers-buffer))
  (define-key menu [inferior]
    '(menu-item "Separate IO" gdb-frame-separate-io-buffer
		:enable gdb-use-separate-io-buffer))
  (define-key menu [locals] '("Locals" . gdb-frame-locals-buffer))
  (define-key menu [frames] '("Stack" . gdb-frame-stack-buffer))
  (define-key menu [breakpoints]
    '("Breakpoints" . gdb-frame-breakpoints-buffer)))

(let ((menu (make-sparse-keymap "GDB-MI")))
  (define-key gud-menu-map [mi]
    `(menu-item "GDB-MI" ,menu :visible (eq gud-minor-mode 'gdbmi)))
  (define-key menu [gdb-use-separate-io]
  '(menu-item "Separate IO" gdb-use-separate-io-buffer
	      :help "Toggle separate IO for debugged program."
	      :button (:toggle . gdb-use-separate-io-buffer)))
  (define-key menu [gdb-many-windows]
  '(menu-item "Display Other Windows" gdb-many-windows
	      :help "Toggle display of locals, stack and breakpoint information"
	      :button (:toggle . gdb-many-windows)))
  (define-key menu [gdb-restore-windows]
  '(menu-item "Restore Window Layout" gdb-restore-windows
	      :help "Restore standard layout for debug session.")))

(defun gdb-frame-gdb-buffer ()
  "Display GUD buffer in a new frame."
  (interactive)
  (let ((special-display-regexps (append special-display-regexps '(".*")))
	(special-display-frame-alist
	 (remove '(menu-bar-lines) (remove '(tool-bar-lines)
					   gdb-frame-parameters)))
	(same-window-regexps nil))
    (display-buffer gud-comint-buffer)))

(defun gdb-display-gdb-buffer ()
  "Display GUD buffer."
  (interactive)
  (let ((same-window-regexps nil))
    (pop-to-buffer gud-comint-buffer)))

(defun gdb-set-window-buffer (name)
  (set-window-buffer (selected-window) (get-buffer name))
  (set-window-dedicated-p (selected-window) t))

(defun gdb-setup-windows ()
  "Layout the window pattern for `gdb-many-windows'."
  (gdb-display-locals-buffer)
  (gdb-display-stack-buffer)
  (delete-other-windows)
  (gdb-display-breakpoints-buffer)
  (delete-other-windows)
  ; Don't dedicate.
  (pop-to-buffer gud-comint-buffer)
  (split-window nil ( / ( * (window-height) 3) 4))
  (split-window nil ( / (window-height) 3))
  (split-window-horizontally)
  (other-window 1)
  (gdb-set-window-buffer (gdb-locals-buffer-name))
  (other-window 1)
  (switch-to-buffer
       (if gud-last-last-frame
	   (gud-find-file (car gud-last-last-frame))
	 (if gdb-main-file
	     (gud-find-file gdb-main-file)
	   ;; Put buffer list in window if we
	   ;; can't find a source file.
	   (list-buffers-noselect))))
  (setq gdb-source-window (selected-window))
  (when gdb-use-separate-io-buffer
    (split-window-horizontally)
    (other-window 1)
    (gdb-set-window-buffer
     (gdb-get-buffer-create 'gdb-inferior-io)))
  (other-window 1)
  (gdb-set-window-buffer (gdb-stack-buffer-name))
  (split-window-horizontally)
  (other-window 1)
  (gdb-set-window-buffer (gdb-breakpoints-buffer-name))
  (other-window 1))

(defcustom gdb-many-windows nil
  "Nil means just pop up the GUD buffer unless `gdb-show-main' is t.
In this case it starts with two windows: one displaying the GUD
buffer and the other with the source file with the main routine
of the debugged program.  Non-nil means display the layout shown for
`gdbmi'."
  :type 'boolean
  :group 'gud
  :version "22.1")

(defun gdb-many-windows (arg)
  "Toggle the number of windows in the basic arrangement.
With arg, display additional buffers iff arg is positive."
  (interactive "P")
  (setq gdb-many-windows
	(if (null arg)
	    (not gdb-many-windows)
	  (> (prefix-numeric-value arg) 0)))
  (message (format "Display of other windows %sabled"
		   (if gdb-many-windows "en" "dis")))
  (if (and gud-comint-buffer
	   (buffer-name gud-comint-buffer))
      (condition-case nil
	  (gdb-restore-windows)
	(error nil))))

(defun gdb-restore-windows ()
  "Restore the basic arrangement of windows used by gdba.
This arrangement depends on the value of `gdb-many-windows'."
  (interactive)
  (pop-to-buffer gud-comint-buffer)	;Select the right window and frame.
    (delete-other-windows)
  (if gdb-many-windows
      (gdb-setup-windows)
    (when (or gud-last-last-frame gdb-show-main)
      (split-window)
      (other-window 1)
      (switch-to-buffer
       (if gud-last-last-frame
	   (gud-find-file (car gud-last-last-frame))
	 (gud-find-file gdb-main-file)))
      (setq gdb-source-window (selected-window))
      (other-window 1))))

(defun gdb-reset ()
  "Exit a debugging session cleanly.
Kills the gdb buffers, and resets variables and the source buffers."
  (dolist (buffer (buffer-list))
    (unless (eq buffer gud-comint-buffer)
      (with-current-buffer buffer
	(if (eq gud-minor-mode 'gdbmi)
	    (if (string-match "\\`\\*.+\\*\\'" (buffer-name))
		(kill-buffer nil)
	      (gdb-remove-breakpoint-icons (point-min) (point-max) t)
	      (setq gud-minor-mode nil)
	      (kill-local-variable 'tool-bar-map)
	      (kill-local-variable 'gdb-define-alist))))))
  (setq gdb-overlay-arrow-position nil)
  (setq overlay-arrow-variable-list
	(delq 'gdb-overlay-arrow-position overlay-arrow-variable-list))
  (setq fringe-indicator-alist '((overlay-arrow . right-triangle)))
  (setq gdb-stack-position nil)
  (setq overlay-arrow-variable-list
	(delq 'gdb-stack-position overlay-arrow-variable-list))
  (if (boundp 'speedbar-frame) (speedbar-timer-fn))
  (setq gud-running nil)
  (setq gdb-active-process nil)
  (remove-hook 'after-save-hook 'gdb-create-define-alist t))

(defun gdb-get-source-file ()
  "Find the source file where the program starts and display it with related
buffers, if required."
  (goto-char (point-min))
  (if (re-search-forward gdb-source-file-regexp nil t)
      (setq gdb-main-file (match-string 1)))
 (if gdb-many-windows
      (gdb-setup-windows)
   (gdb-get-buffer-create 'gdb-breakpoints-buffer)
   (if gdb-show-main
       (let ((pop-up-windows t))
	 (display-buffer (gud-find-file gdb-main-file))))))

;;from put-image
(defun gdb-put-string (putstring pos &optional dprop &rest sprops)
  "Put string PUTSTRING in front of POS in the current buffer.
PUTSTRING is displayed by putting an overlay into the current buffer with a
`before-string' string that has a `display' property whose value is
PUTSTRING."
  (let ((string (make-string 1 ?x))
	(buffer (current-buffer)))
    (setq putstring (copy-sequence putstring))
    (let ((overlay (make-overlay pos pos buffer))
	  (prop (or dprop
		    (list (list 'margin 'left-margin) putstring))))
      (put-text-property 0 1 'display prop string)
      (if sprops
	  (add-text-properties 0 1 sprops string))
      (overlay-put overlay 'put-break t)
      (overlay-put overlay 'before-string string))))

;;from remove-images
(defun gdb-remove-strings (start end &optional buffer)
  "Remove strings between START and END in BUFFER.
Remove only strings that were put in BUFFER with calls to `gdb-put-string'.
BUFFER nil or omitted means use the current buffer."
  (unless buffer
    (setq buffer (current-buffer)))
  (dolist (overlay (overlays-in start end))
    (when (overlay-get overlay 'put-break)
	  (delete-overlay overlay))))

(defun gdb-put-breakpoint-icon (enabled bptno)
  (let ((start (- (line-beginning-position) 1))
	(end (+ (line-end-position) 1))
	(putstring (if enabled "B" "b"))
	(source-window (get-buffer-window (current-buffer) 0)))
    (add-text-properties
     0 1 '(help-echo "mouse-1: clear bkpt, mouse-3: enable/disable bkpt")
     putstring)
    (if enabled
	(add-text-properties
	 0 1 `(gdb-bptno ,bptno gdb-enabled t) putstring)
      (add-text-properties
       0 1 `(gdb-bptno ,bptno gdb-enabled nil) putstring))
    (gdb-remove-breakpoint-icons start end)
    (if (display-images-p)
	(if (>= (or left-fringe-width
		    (if source-window (car (window-fringes source-window)))
		    gdb-buffer-fringe-width) 8)
	    (gdb-put-string
	     nil (1+ start)
	     `(left-fringe breakpoint
			   ,(if enabled
				'breakpoint-enabled
			      'breakpoint-disabled))
	     'gdb-bptno bptno
	     'gdb-enabled enabled)
	  (when (< left-margin-width 2)
	    (save-current-buffer
	      (setq left-margin-width 2)
	      (if source-window
		  (set-window-margins
		   source-window
		   left-margin-width right-margin-width))))
	  (put-image
	   (if enabled
	       (or breakpoint-enabled-icon
		   (setq breakpoint-enabled-icon
			 (find-image `((:type xpm :data
					      ,breakpoint-xpm-data
					      :ascent 100 :pointer hand)
				       (:type pbm :data
					      ,breakpoint-enabled-pbm-data
					      :ascent 100 :pointer hand)))))
	     (or breakpoint-disabled-icon
		 (setq breakpoint-disabled-icon
		       (find-image `((:type xpm :data
					    ,breakpoint-xpm-data
					    :conversion disabled
					    :ascent 100 :pointer hand)
				     (:type pbm :data
					    ,breakpoint-disabled-pbm-data
					    :ascent 100 :pointer hand))))))
	   (+ start 1)
	   putstring
	   'left-margin))
      (when (< left-margin-width 2)
	(save-current-buffer
	  (setq left-margin-width 2)
	  (let ((window (get-buffer-window (current-buffer) 0)))
	    (if window
	      (set-window-margins
	       window left-margin-width right-margin-width)))))
      (gdb-put-string
       (propertize putstring
		   'face (if enabled 'breakpoint-enabled 'breakpoint-disabled))
       (1+ start)))))

(defun gdb-remove-breakpoint-icons (start end &optional remove-margin)
  (gdb-remove-strings start end)
  (if (display-images-p)
      (remove-images start end))
  (when remove-margin
    (setq left-margin-width 0)
    (let ((window (get-buffer-window (current-buffer) 0)))
      (if window
	  (set-window-margins
	   window left-margin-width right-margin-width)))))

;; For debugging Emacs only (assumes that usual stack buffer already exists).
(defun gdbmi-xbacktrace ()
  "Generate a full lisp level backtrace with arguments."
  (interactive)
  (with-current-buffer (get-buffer-create "xbacktrace")
    (erase-buffer))
  (let ((frames nil)
	(frame-number gdb-frame-number))
    (with-current-buffer (gdb-get-buffer 'gdb-stack-buffer)
      (save-excursion
	(goto-char (point-min))
	(while (search-forward "Ffuncall" nil t)
	  (goto-char (line-beginning-position))
	  (looking-at "^\\([0-9]+\\)")
	  (push (match-string-no-properties 1) frames)
	  (forward-line 1))))
    (dolist (frame frames)
      (gdb-enqueue-input (list (concat "frame " frame "\n")
			       'ignore))
    (gdb-enqueue-input (list "ppargs\n" 'gdb-get-arguments)))
    (gdb-enqueue-input (list (concat "frame " frame-number "\n")
			     'ignore))))
    
(defun gdb-get-arguments ()
  (with-current-buffer "xbacktrace"
    (insert-buffer-substring (gdb-get-buffer 'gdb-partial-output-buffer))))

(defun gdb-frame-xbacktrace-buffer ()
  "Display GUD buffer in a new frame."
  (interactive)
  (let ((special-display-regexps (append special-display-regexps '(".*")))
	(special-display-frame-alist gdb-frame-parameters)
	(same-window-regexps nil))
    (display-buffer (get-buffer-create "xbacktrace"))))

(provide 'gdb-mi)
;;; gdb-mi.el ends here
