AUTOMAKE_OPTIONS = foreign

bin_PROGRAMS = asm

asm_SOURCES = asm.cc
asm_CPPFLAGS = $(SHEVEK_CFLAGS)
asm_LDADD = $(SHEVEK_LIBS)

JUNK = aclocal.m4 AUTHORS Changelog configure COPYING depcomp INSTALL \
       install-sh Makefile.in missing NEWS README

autoclean: maintainer-clean
 ifneq (,$(wildcard ${JUNK}))
	rm -r ${JUNK}
 endif
