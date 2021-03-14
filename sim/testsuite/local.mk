## See sim/Makefile.am.
#
# Copyright (C) 1997-2021 Free Software Foundation, Inc.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

# Tweak the site.exp so it works with plain `runtest` from user.
EXTRA_DEJAGNU_SITE_CONFIG = site-srcdir.exp

site-srcdir.exp: Makefile
	echo "set srcdir \"$(srcdir)/testsuite\"" > $@

check-DEJAGNU: site.exp
	LC_ALL=C; export LC_ALL; \
	EXPECT=${EXPECT} ; export EXPECT ; \
	runtest=$(RUNTEST); \
	if $(SHELL) -c "$$runtest --version" > /dev/null 2>&1; then \
	  $$runtest $(RUNTESTFLAGS); \
	else echo "WARNING: could not find \`runtest'" 1>&2; :;\
	fi

MOSTLYCLEANFILES += \
	site-srcdir.exp testrun.log testrun.sum

include %D%/common/local.mk
