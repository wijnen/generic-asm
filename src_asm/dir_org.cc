#include "asm.hh"

void dir_org (shevek::ristring &args, bool first, std::list <Label>::iterator current_label)
{
	(void)current_label;
	(void)first;
	Expr::valid_int i = read_expr (args.rest (), ";");
	if (!i.valid)
		error ("invalid expression in org");	// TODO: allow defining non-absolute blocks.
	absolute_addr = true;
	addr = i.value;
	if (writing)
	{
		if (!blocks.empty () && blocks.back ().parts.empty ())
			blocks.pop_back ();
		blocks.push_back (Block ());
		blocks.back ().absolute = true;
		blocks.back ().address = i.value;
		if (listfile)
			*listfile << "\t\t\t";
	}
}
