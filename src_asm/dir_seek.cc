#include "asm.hh"

unsigned dir_seek (shevek::istring &args, bool write, Label *current_label)
{
	(void)current_label;
	(void)write;
	error ("seek is not yet implemented");
	return 0;
}
