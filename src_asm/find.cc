#include "asm.hh"

std::map <Glib::ustring, Param>::reverse_iterator
	Param::find (Glib::ustring const &name)
{
	for (std::map <Glib::ustring, Param>::reverse_iterator
			i = params.rbegin (); i != params.rend (); ++i)
		if (i->first == name)
			return i;
	return params.rend ();
}
