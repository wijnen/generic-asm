#include "asm.hh"

void dir_comment (shevek::istring &args, bool first, Label *current_label)
{
	(void)first;
	(void)current_label;
	(void)args;
	if (writing && listfile)
		*listfile << "\t\t\t";
}
