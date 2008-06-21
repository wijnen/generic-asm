#include "asm.hh"
#include <iostream>

void write_out (Source const &s)
{
	for (std::list <std::string>::const_iterator
			i = s.targets.begin (); i != s.targets.end (); ++i)
	{
		int byte = read_expr (*i);
		if ((byte < -0x80) || (byte >= 0x100))
			error (shevek::ostring ("byte %x out of range", byte));
		std::cout << (char)byte;
	}
}
