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
	{
		Expr e;
		e.list.push_back (ExprElem (ExprElem::NUM, Expr::valid_int (current_label->name)));
		current_label->value = e;
		e = Expr::read (args.rest (), false, pos);
		if (pos == std::string::npos)
		{
			error ("incorrect expression in equ");
			return;
		}
		args.skip (pos);
		args (" ");
		if (!args (";") && !args.rest ().empty ())
			error (shevek::rostring ("junk at end of expression: %s", args.rest ()));
		Expr::valid_int vi = e.compute (Expr::valid_int (":=="));
		for (std::list <std::string>::iterator i = vi.invalid.begin (); i != vi.invalid.end (); ++i)
		{
			dbg (current_label->name << " -> " << *i);
			if (*i == current_label->name)
			{
				error ("label " + current_label->name + " recursively defined");
				return;
			}
		}
		current_label->value = e;
	}
	if (writing && listfile)
		*listfile << "\t\t\t";
}
