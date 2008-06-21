#include "asm.hh"

std::map <std::string, Param>::reverse_iterator
	Param::find (std::string const &name)
{
	for (std::map <std::string, Param>::reverse_iterator
			i = params.rbegin (); i != params.rend (); ++i)
		if (i->first == name)
			return i;
	return params.rend ();
}
