#include "asm.hh"

static std::list <Expr::valid_int> defb_expr (shevek::istring &args)
{
	std::list <Expr::valid_int> ret;
	args (" ");
	if (args.rest ().empty ())
	{
		error ("empty expression in defb");
		return std::list <Expr::valid_int> ();
	}
	Glib::ustring r = args.rest ();
	if (r[0] == '\'' || r[0] == '"')
	{
		Glib::ustring::size_type p = 1;
		while (p < r.size () && r[p] != r[0])
		{
			Expr::valid_int i;
			i.valid = true;
			i.value = r[p];
			ret.push_back (i);
			++p;
		}
		if (p >= r.size ())
		{
			error ("unterminated string in defb");
			return std::list <Expr::valid_int> ();
		}
		args.skip (p + 1);
	}
	else
	{
		Glib::ustring::size_type pos = 0;
		Expr::valid_int v = read_expr (args.rest (), false, pos);
		if (pos == Glib::ustring::npos)
		{
			error ("incorrect expression in defb");
			return std::list <Expr::valid_int> ();
		}
		args.skip (pos);
		ret.push_back (v);
	}
	return ret;
}

void dir_defb (shevek::istring &args, bool write, bool first, Label *current_label)
{
	(void)first;
	(void)current_label;
	unsigned s = 0;
	while (true)
	{
		std::list <Expr::valid_int> v = defb_expr (args);
		if (v.empty ())
			break;
		if (write)
		{
			std::list <Expr::valid_int>::iterator i;
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
}
