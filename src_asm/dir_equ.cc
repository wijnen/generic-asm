#include "asm.hh"

unsigned dir_equ (shevek::istring &args, bool write, bool first,
		Label *current_label)
{
	(void)current_label;
	if (!current_label)
	{
		error ("equ used without a label");
		return 1;
	}
	std::string::size_type pos = 0;
	current_label->value
		= read_expr (args.rest (), false, pos, &current_label->valid);
	if (pos == std::string::npos)
	{
		error ("invalid expression");
		return 1;
	}
	args.skip (pos);
	args (" ");
	if (!args.rest ().empty ())
		error (shevek::ostring ("junk at end of expression: %s",
					args.rest ()));
	if (write && listfile)
		*listfile << "\t\t\t";
	return current_label->valid ? 0 : 1;
}
