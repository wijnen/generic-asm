#include "asm.hh"

unsigned dir_endif (shevek::istring &args, bool write, Label *current_label)
{
	(void)current_label;
	(void)write;
	error ("endif is not yet implemented");
	return 0;
}
