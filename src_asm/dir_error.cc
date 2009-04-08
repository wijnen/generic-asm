#include "asm.hh"

void dir_error (shevek::ristring &args, bool first, std::list <Label>::iterator current_label)
{
	(void)first;
	(void)current_label;
	error (args.rest ());
	if (writing && listfile)
		*listfile << "\t\t\t";
}
