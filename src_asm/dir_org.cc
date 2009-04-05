#include "asm.hh"

void dir_org (shevek::istring &args, bool first, Label *current_label)
{
	(void)first;
	(void)current_label;
	Expr::valid_int i = read_expr (args.rest (), ";");
	if (!i.valid)
		error ("invalid expression in org");
	addr = i.value;
	if (writing && listfile)
		*listfile << "\t\t\t";
}
