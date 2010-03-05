#include "asm.hh"

void write_out (Source const &s)
{
	unsigned num = 0;
	for (std::list <std::string>::const_iterator i = s.targets.begin (); i != s.targets.end (); ++i, ++num)
	{
		dbg ("writing " << *i);
		std::string::size_type pos = 0;
		Expr e = Expr::read (*i, true, pos);
		if (pos == std::string::npos || (pos != i->size () && (*i)[pos] != '#'))
		{
			error ("unable to parse write expression");
			continue;
		}
		e.simplify ();
		write_expr (e);
	}
}
