#include "asm.hh"

void Param::reset ()
{
	for (std::list <Param>::iterator i = params.begin (); i != params.end (); ++i)
		i->is_active = false;
}
