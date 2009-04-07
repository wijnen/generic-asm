#include "asm.hh"

void write_out (Source const &s)
{
	unsigned num = 0;
	for (std::list <Glib::ustring>::const_iterator i = s.targets.begin (); i != s.targets.end (); ++i)
	{
		Glib::ustring::size_type pos = 0;
		Expr e = Expr::read (*i, true, pos);
		if (pos == Glib::ustring::npos || (pos != i->size () && (*i)[pos] != '#'))
		{
			error ("unable to parse write expression");
			++num;
			continue;
		}
		write_expr (e);
		++num;
	}
}
