#include "asm.hh"

void dir_comment (shevek::ristring &args, bool first, std::list <Label>::iterator current_label)
{
	(void)first;
	(void)current_label;
	(void)args;
	if (writing && listfile)
		*listfile << "\t\t\t";
}
