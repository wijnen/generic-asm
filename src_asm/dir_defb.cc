#include "asm.hh"

static std::list <Expr> defb_expr (shevek::ristring &args)
{
	std::list <Expr> ret;
	args (" ");
	if (args.rest ().empty ())
	{
		error ("empty expression in defb");
		return std::list <Expr> ();
	}
	std::string r = args.rest ();
	if (r[0] == '\'' || r[0] == '"')
	{
		std::string::size_type p = 1;
		while (p < r.size () && r[p] != r[0])
		{
			ret.push_back (Expr (Expr::NUM, Expr::valid_int (r[p])));
			++p;
		}
		if (p >= r.size ())
		{
			error ("unterminated string in defb");
			return std::list <Expr> ();
		}
		args.skip (p + 1);
	}
	else
	{
		std::string::size_type pos = 0;
		Expr e = Expr::read (args.rest (), false, pos);
		if (pos == std::string::npos)
		{
			error ("incorrect expression in defb");
			return std::list <Expr> ();
		}
		args.skip (pos);
		ret.push_back (e);
	}
	return ret;
}

void dir_defb (shevek::ristring &args, bool first, std::list <Label>::iterator current_label)
{
	(void)first;
	(void)current_label;
	unsigned s = 0;
	while (true)
	{
		std::list <Expr> e = defb_expr (args);
		if (e.empty ())
			break;
		std::list <Expr>::iterator i;
		unsigned p;
		for (i = e.begin (), p = 0; i != e.end (); ++i, ++p)
		{
			write_expr (*i);
		}
		s += e.size ();
		if (!args (" ,"))
			break;
	}
	addr += s;
	if (writing && listfile)
	{
		int size = 5 + 3 * s;
		int numtabs = (31 - size) / 8;
		if (numtabs <= 0)
			numtabs = 1;
		*listfile << std::string (numtabs, '\t');
	}
}
