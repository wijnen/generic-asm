AUTOMAKE_OPTIONS = foreign subdir-objects

bin_PROGRAMS = genasm
dist_bin_SCRIPTS = asm-z80 asm-hc08

genasm_SOURCES = \
	src_asm/assemble.cc \
	src_asm/data.cc \
	src_asm/dir_defb.cc \
	src_asm/dir_comment.cc \
	src_asm/dir_else.cc \
	src_asm/dir_endif.cc \
	src_asm/dir_endmacro.cc \
	src_asm/dir_equ.cc \
	src_asm/dir_error.cc \
	src_asm/dir_if.cc \
	src_asm/dir_incbin.cc \
	src_asm/dir_include.cc \
	src_asm/dir_macro.cc \
	src_asm/dir_org.cc \
	src_asm/dir_seek.cc \
	src_asm/directives.cc \
	src_asm/disassemble.cc \
	src_asm/error.cc \
	src_asm/escape.cc \
	src_asm/expr_compute.cc \
	src_asm/expr_check.cc \
	src_asm/expr_handle_oper.cc \
	src_asm/expr_print.cc \
	src_asm/expr_read.cc \
	src_asm/expr_simplify.cc \
	src_asm/file_block_write.cc \
	src_asm/file_clean.cc \
	src_asm/find.cc \
	src_asm/find_label.cc \
	src_asm/getline.cc \
	src_asm/hex.cc \
	src_asm/input_line.cc \
	src_asm/main.cc \
	src_asm/operators.cc \
	src_asm/param_reset.cc \
	src_asm/parse.cc \
	src_asm/read_definitions.cc \
	src_asm/read_expr.cc \
	src_asm/read_file.cc \
	src_asm/read_filename.cc \
	src_asm/subst_args.cc \
	src_asm/write_byte.cc \
	src_asm/write_expr.cc \
	src_asm/write_out.cc \
	asm.hh
genasm_CPPFLAGS = $(SHEVEK_CFLAGS) -I. -Wall -Werror -Wextra -Wmissing-declarations -Wredundant-decls -O -Wunused
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
