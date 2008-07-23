#include "asm.hh"

unsigned dir_org (shevek::istring &args, bool write, bool first,
		Label *current_label)
{
	(void)current_label;
	addr = read_expr (args.rest (), ";");
	if (write && listfile)
		*listfile << "\t\t\t";
	return 0;
}
