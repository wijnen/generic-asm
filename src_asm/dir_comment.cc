#include "asm.hh"

unsigned dir_comment (shevek::istring &args, bool write, bool first, Label *current_label)
{
	(void)first;
	(void)current_label;
	(void)args;
	if (write && listfile)
		*listfile << "\t\t\t";
	return 0;
}
