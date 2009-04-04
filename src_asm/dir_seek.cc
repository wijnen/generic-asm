#include "asm.hh"

void dir_seek (shevek::istring &args, bool write, bool first, Label *current_label)
{
	(void)first;
	(void)args;
	(void)current_label;
	error ("seek is not yet implemented");
	if (write && listfile)
		*listfile << "\t\t\t";
}
