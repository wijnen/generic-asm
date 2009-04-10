#include "asm.hh"

void dir_org (shevek::ristring &args, bool first, std::list <Label>::iterator current_label)
{
	(void)current_label;
	Expr::valid_int i = read_expr (args.rest (), ";");
	if (!i.valid)
		error ("invalid expression in org");	// TODO: allow defining non-absolute blocks.
	absolute_addr = true;
	addr = i.value;
	if (first)
	{
		if (!files.back ().blocks.empty () && files.back ().blocks.back ().parts.empty ())
			files.back ().blocks.pop_back ();
		files.back ().blocks.push_back (File::Block ());
		files.back ().blocks.back ().absolute = true;
		files.back ().blocks.back ().address = i.value;
	}
	if (writing && listfile)
		*listfile << "\t\t\t";
}
