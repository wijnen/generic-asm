#include "asm.hh"

std::list <Param>::iterator Param::find (Glib::ustring const &name)
{
	for (std::list <Param>::iterator i = params.begin (); i != params.end (); ++i)
		if (i->name == name)
			return i;
	return params.end ();
}
