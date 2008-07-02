#include "asm.hh"

unsigned dir_else (shevek::istring &args, bool write, Label *current_label)
{
	(void)current_label;
	(void)write;
	error ("else is not yet implemented");
	return 0;
}
