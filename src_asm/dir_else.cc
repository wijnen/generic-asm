#include "asm.hh"

void dir_else (shevek::istring &args, bool first, Label *current_label)
{
	(void)first;
	(void)args;
	(void)current_label;
	error ("else is not yet implemented");
	if (writing && listfile)
		*listfile << "\t\t\t";
}
