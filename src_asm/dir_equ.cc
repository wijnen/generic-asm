#include "asm.hh"
#include <shevek/debug.hh>

void dir_equ (shevek::istring &args, bool write, bool first, Label *current_label)
{
	startfunc;
	(void)first;
	(void)current_label;
	if (!current_label)
	{
		error ("equ used without a label");
		return;
	}
	Glib::ustring::size_type pos = 0;
	current_label->value = read_expr (args.rest (), false, pos);
	if (pos == Glib::ustring::npos)
	{
		error ("incorrect expression in equ");
		return;
	}
	args.skip (pos);
	args (" ");
	if (!args (";") && !args.rest ().empty ())
		error (shevek::ostring ("junk at end of expression: %s", args.rest ()));
	if (write && listfile)
		*listfile << "\t\t\t";
}
