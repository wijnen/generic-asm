#include "asm.hh"

std::list <Label>::iterator find_label (std::string name)
{
	std::list <Label>::iterator i;
	for (i = labels.begin (); i != labels.end (); ++i)
	{
		if (i->name == name)
			break;
	}
	return i;
}
