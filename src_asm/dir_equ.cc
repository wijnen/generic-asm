#include "asm.hh"

void dir_equ (shevek::ristring &args, bool first, std::list <Label>::iterator current_label)
{
	startfunc;
	if (current_label == labels.end ())
	{
		error ("equ used without a label");
		return;
	}
	std::string::size_type pos = 0;
	if (first)
		current_label->value = Expr::read (args.rest (), false, pos);
	if (pos == std::string::npos)
	{
		error ("incorrect expression in equ");
		return;
	}
	args.skip (pos);
	args (" ");
	if (!args (";") && !args.rest ().empty ())
		error (shevek::rostring ("junk at end of expression: %s", args.rest ()));
	if (writing && listfile)
		*listfile << "\t\t\t";
}
