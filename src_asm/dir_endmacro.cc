#include "asm.hh"

void dir_endmacro (shevek::ristring &args, bool first, std::list <Label>::iterator current_label)
{
	(void)first;
	(void)args;
	(void)current_label;
	error ("endmacro is not yet implemented");
	if (writing && listfile)
		*listfile << "\t\t\t";
}
