#include "asm.hh"

void Param::reset ()
{
	for (std::map <std::string, Param>::reverse_iterator
			i = params.rbegin (); i != params.rend (); ++i)
		i->second.is_active = false;
}
