#include "asm.hh"

Directive directives[12] = {
	Directive ("org", &dir_org),	// must be the first one.
	Directive ("equ", &dir_equ),	// must be the second one.
	Directive ("defb", &dir_defb),	// must be the third one.
	Directive ("include", &dir_include),
	Directive ("incbin", &dir_incbin),
	Directive ("macro", &dir_macro),
	Directive ("endmacro", &dir_endmacro),
	Directive ("if", &dir_if),
	Directive ("else", &dir_else),
	Directive ("endif", &dir_endif),
	Directive ("error", &dir_error),
	Directive (";", &dir_comment)
};
