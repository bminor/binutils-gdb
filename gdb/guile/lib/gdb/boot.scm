;; Bootstrap the Scheme side of the gdb module.
;;
;; Copyright (C) 2014 Free Software Foundation, Inc.
;;
;; This file is part of GDB.
;;
;; This program is free software; you can redistribute it and/or modify
;; it under the terms of the GNU General Public License as published by
;; the Free Software Foundation; either version 3 of the License, or
;; (at your option) any later version.
;;
;; This program is distributed in the hope that it will be useful,
;; but WITHOUT ANY WARRANTY; without even the implied warranty of
;; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;; GNU General Public License for more details.
;;
;; You should have received a copy of the GNU General Public License
;; along with this program.  If not, see <http://www.gnu.org/licenses/>.

;; This file is loaded with scm_c_primitive_load, which is ok, but files
;; loaded with it are not compiled.  So we do very little here, and do
;; most of the initialization elsewhere.

;; guile-data-directory is provided by the C code.
(add-to-load-path (guile-data-directory))
(load-from-path "gdb.scm")

;; Now that the Scheme side support is loaded, initialize it.
(let ((init-proc (@@ (gdb init) %initialize!)))
  (init-proc))
