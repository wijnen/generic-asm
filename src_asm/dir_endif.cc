#include "asm.hh"

unsigned dir_endif (shevek::istring &args, bool write, bool first,
		Label *current_label)
{
	(void)current_label;
	error ("endif is not yet implemented");
	if (write && listfile)
		*listfile << "\t\t\t";
	return 0;
}