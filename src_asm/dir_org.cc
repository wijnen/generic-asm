#include "asm.hh"

void dir_org (shevek::ristring &args, bool first, std::list <Label>::iterator current_label)
{
	(void)first;
	(void)current_label;
	Expr::valid_int i = read_expr (args.rest (), ";");
	if (!i.valid)
		error ("invalid expression in org");	// TODO
	absolute_addr = true;
	addr = i.value;
	if (writing && listfile)
		*listfile << "\t\t\t";
}
