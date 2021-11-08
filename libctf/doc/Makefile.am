## Process this file with automake to produce Makefile.in.
#
#   Copyright (C) 2019-2021 Free Software Foundation, Inc.
#
# This file is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; see the file COPYING.  If not see
# <http://www.gnu.org/licenses/>.
#

AUTOMAKE_OPTIONS = info-in-builddir foreign no-texinfo.tex

info_TEXINFOS = ctf-spec.texi
libctf_TEXINFOS = $(info_TEXINFOS)

AM_MAKEINFOFLAGS = --no-split

DISTCLEANFILES = texput.log
MAINTAINERCLEANFILES = ctf-spec.info

html-local: ctf-spec/index.html
ctf-spec/index.html: ctf-spec.texi
	$(AM_V_at)$(MAKEINFOHTML) $(AM_MAKEINFOHTMLFLAGS) $(MAKEINFOFLAGS) \
	  --split=node -I$(srcdir) $(srcdir)/ctf-spec.texi
