#include "asm.hh"

unsigned dir_if (shevek::istring &args, bool write, bool first, Label *current_label)
{
	(void)first;
	(void)args;
	(void)current_label;
	error ("if is not yet implemented");
	if (write && listfile)
		*listfile << "\t\t\t";
	return 0;
}
