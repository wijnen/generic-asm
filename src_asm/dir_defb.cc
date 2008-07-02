#include "asm.hh"

unsigned dir_defb (shevek::istring &args, bool write, Label *current_label)
{
	(void)current_label;
	unsigned undef = 0;
	while (true)
	{
		bool valid;
		std::string::size_type pos = 0;
		int v = read_expr (args.rest (), false, pos, &valid);
		if (pos == std::string::npos)
		{
			error ("invalid expression");
			break;
		}
		args.skip (pos);
		if (!args (" ,"))
			break;
		if (!valid)
			++undef;
		if (write)
			write_byte (v);
		++addr;
	}
	return undef;
}
