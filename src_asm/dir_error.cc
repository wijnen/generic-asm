#include "asm.hh"

unsigned dir_error (shevek::istring &args, bool write, Label *current_label)
{
	(void)current_label;
	(void)write;
	error (args.rest ());
	return 0;
}
