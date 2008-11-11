#include "asm.hh"

unsigned dir_defb (shevek::istring &args, bool write, bool first,
		Label *current_label)
{
	(void)current_label;
	unsigned undef = 0;
	unsigned s = 0;
	while (true)
	{
		bool valid;
		Glib::ustring::size_type pos = 0;
		int v = read_expr (args.rest (), false, pos, &valid);
		if (pos == Glib::ustring::npos)
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
			write_byte (v, s);
		++s;
	}
	addr += s;
	if (write && listfile)
	{
		int size = 5 + 3 * s;
		int numtabs = (31 - size) / 8;
		if (numtabs <= 0)
			numtabs = 1;
		*listfile << Glib::ustring (numtabs, '\t');
	}
	return undef;
}
