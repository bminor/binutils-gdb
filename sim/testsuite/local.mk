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
EXTRA_DEJAGNU_SITE_CONFIG = site-sim-config.exp

# Custom verbose test variables that automake doesn't provide (yet?).
AM_V_RUNTEST = $(AM_V_RUNTEST_@AM_V@)
AM_V_RUNTEST_ = $(AM_V_RUNTEST_@AM_DEFAULT_V@)
AM_V_RUNTEST_0 =  @echo "  RUNTEST  $(RUNTESTFLAGS)";
AM_V_RUNTEST_1 =

site-sim-config.exp: Makefile
	$(AM_V_GEN)( \
	echo "set builddir \"$(builddir)\""; \
	echo "set srcdir \"$(srcdir)/testsuite\""; \
	) > $@

check-DEJAGNU: site.exp
	$(AM_V_RUNTEST)LC_ALL=C; export LC_ALL; \
	EXPECT=${EXPECT} ; export EXPECT ; \
	runtest=$(RUNTEST); \
	if $(SHELL) -c "$$runtest --version" > /dev/null 2>&1; then \
	  $$runtest $(RUNTESTFLAGS); \
	else echo "WARNING: could not find \`runtest'" 1>&2; :;\
	fi

MOSTLYCLEANFILES += \
	site-sim-config.exp testrun.log testrun.sum

include %D%/common/local.mk
