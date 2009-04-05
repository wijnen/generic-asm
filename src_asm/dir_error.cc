#include "asm.hh"

void dir_error (shevek::istring &args, bool first, Label *current_label)
{
	(void)first;
	(void)current_label;
	error (args.rest ());
	if (writing && listfile)
		*listfile << "\t\t\t";
}
