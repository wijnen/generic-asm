#include "asm.hh"

unsigned dir_org (shevek::istring &args, bool write, Label *current_label)
{
	(void)current_label;
	(void)write;
	addr = read_expr (args.rest (), ";");
	return 0;
}
