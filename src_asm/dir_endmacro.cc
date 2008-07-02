#include "asm.hh"

unsigned dir_endmacro (shevek::istring &args, bool write, Label *current_label)
{
	(void)current_label;
	(void)write;
	error ("endmacro is not yet implemented");
	return 0;
}
