#include "asm.hh"

void dir_if (shevek::ristring &args, bool first, std::list <Label>::iterator current_label)
{
	(void)first;
	(void)args;
	(void)current_label;
	error ("if is not yet implemented");
	if (writing && listfile)
		*listfile << "\t\t\t";
}
