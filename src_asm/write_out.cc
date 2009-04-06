#include "asm.hh"

void write_out (Source const &s)
{
	unsigned num = 0;
	for (std::list <Glib::ustring>::const_iterator i = s.targets.begin (); i != s.targets.end (); ++i)
	{
		write_byte (read_expr (*i, "#"), num);
		++num;
	}
}
