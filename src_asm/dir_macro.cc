#include "asm.hh"

unsigned dir_macro (shevek::istring &args, bool write, bool first,
		Label *current_label)
{
	(void)current_label;
	error ("macro is not yet implemented");
	if (write && listfile)
		*listfile << "\t\t\t";
	return 0;
}