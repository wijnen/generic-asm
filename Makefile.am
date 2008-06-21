AUTOMAKE_OPTIONS = foreign subdir-objects

bin_PROGRAMS = genasm
dist_bin_SCRIPTS = asm-z80 asm-hc08

genasm_SOURCES = asm.hh \
	src_asm/data.cc \
	src_asm/error.cc \
	src_asm/escape.cc \
	src_asm/expr_compute.cc \
	src_asm/expr_handle_oper.cc \
	src_asm/expr_read.cc \
	src_asm/find.cc \
	src_asm/find_label.cc \
	src_asm/getline.cc \
	src_asm/main.cc \
	src_asm/operators.cc \
	src_asm/param_reset.cc \
	src_asm/parse.cc \
	src_asm/read_definitions.cc \
	src_asm/read_expr.cc \
	src_asm/subst_args.cc \
	src_asm/write_out.cc
genasm_CPPFLAGS = $(SHEVEK_CFLAGS) -I.
genasm_LDADD = $(SHEVEK_LIBS)

asm-%: asm-%.in Makefile
	sed '1s:NAME:$(bindir)/genasm:' < $< > $@

CLEANFILES = asm-z80 asm-hc08

EXTRA_DIST = asm-z80.in asm-hc08.in

JUNK = aclocal.m4 AUTHORS Changelog configure depcomp install-sh Makefile.in \
       missing NEWS README

autoclean: maintainer-clean
 ifneq (,$(wildcard ${JUNK}))
	rm -r ${JUNK}
 endif
