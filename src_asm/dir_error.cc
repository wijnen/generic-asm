#include "asm.hh"

unsigned dir_error (shevek::istring &args, bool write, bool first, Label *current_label)
{
	(void)first;
	(void)current_label;
	error (args.rest ());
	if (write && listfile)
		*listfile << "\t\t\t";
	return 0;
}
