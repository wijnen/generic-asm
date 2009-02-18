#include "asm.hh"

static std::list <int> defb_expr (shevek::istring &args, unsigned &undef)
{
	std::list <int> ret;
	args (" ");
	if (args.rest ().empty ())
	{
		error ("empty expression in defb");
		return std::list <int> ();
	}
	Glib::ustring r = args.rest ();
	if (r[0] == '\'' || r[0] == '"')
	{
		Glib::ustring::size_type p = 1;
		while (p < r.size () && r[p] != r[0])
		{
			ret.push_back (r[p]);
			++p;
		}
		if (p >= r.size ())
		{
			error ("unterminated string in defb");
			return std::list <int> ();
		}
		args.skip (p + 1);
	}
	else
	{
		Glib::ustring::size_type pos = 0;
		bool valid;
		int v = read_expr (args.rest (), false, pos, &valid);
		if (pos == Glib::ustring::npos)
		{
			error ("invalid expression in defb");
			return std::list <int> ();
		}
		if (!valid)
			++undef;
		args.skip (pos);
		ret.push_back (v);
	}
	return ret;
}

unsigned dir_defb (shevek::istring &args, bool write, bool first, Label *current_label)
{
	(void)first;
	(void)current_label;
	unsigned undef = 0;
	unsigned s = 0;
	while (true)
	{
		std::list <int> v = defb_expr (args, undef);
		if (v.empty ())
			break;
		if (write)
		{
			std::list <int>::iterator i;
			unsigned p;
			for (i = v.begin (), p = 0; i != v.end (); ++i, ++p)
				write_byte (*i, s + p);
		}
		s += v.size ();
		if (!args (" ,"))
			break;
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
