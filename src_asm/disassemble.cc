#include "asm.hh"

static bool fit (int ch, Expr expr, std::list <std::list <Param>::iterator> &param)
{
	(void)param;
	ch &= 0xff;
	// Try the simple option first: the expression is a constant.
	Expr::valid_int i = expr.compute (Expr::valid_int ("<<"));
	if (i.valid && (i.value & 0xff) == ch)
		return true;
	// TODO
	return false;
}

void disassemble (std::istream &in)
{
	std::string defb;
	if (directives[0].nick.empty ())
	{
		std::cerr << "Warning: defb has no defined nickname; using \"defb\".\n";
		defb = "defb";
	}
	else
		defb = directives[0].nick.front ();
	*outfile << std::hex;
	unsigned max = 1;
	for (std::list <Source>::iterator s = sources.begin (); s != sources.end (); ++s)
	{
		if (s->targets.size () > max)
			max = s->targets.size ();
	}
	std::string buffer;
	while (!buffer.empty () || in)
	{
		while (buffer.size () < max && in)
		{
			char c;
			in >> c;
			buffer += c;
		}
		std::list <Source>::iterator s;
		for (s = sources.begin (); s != sources.end (); ++s)
		{
			if (buffer.size () < s->targets.size ())
				continue;
			std::list <std::list <Param>::iterator> p;
			for (std::list <std::pair <std::string, std::list <Param>::iterator> >::iterator pi = s->parts.begin (); pi != s->parts.end (); ++pi)
				p.push_back (pi->second);
			std::list <std::string>::iterator t;
			unsigned i;
			for (i = 0, t = s->targets.begin (); t != s->targets.end (); ++i, ++t)
			{
				std::string::size_type pos = 0;
				if (!fit (buffer[i], Expr::read (*t, true, pos), p))
					break;
			}
			if (t == s->targets.end ())
			{
				// TODO: print output
				break;
			}
		}
		if (s == sources.end ())
		{
			*outfile << '\t' << defb << " 0x" << (buffer[0] & 0xff) << '\n';
			buffer = buffer.substr (1);
		}
	}
}
