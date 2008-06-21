#include "asm.hh"

Label *find_label (std::string name)
{
	for (std::list <Label>::iterator
		i = labels.begin (); i != labels.end (); ++i)
	{
		if (i->name == name)
			return &*i;
	}
	return NULL;
}
