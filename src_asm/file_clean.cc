#include "asm.hh"

void File::Block::clean (bool file)
{
	for (std::list <Part>::iterator i = parts.begin (); i != parts.end (); ++i)
	{
		if (i->type != Part::DEFINE)
			continue;
		if (file)
			i->label->definition = NULL;
		if (i->name[0] != '.' && (!file || i->name[0] != '@'))
			continue;
		i->expr = i->label->value;
		i->expr.check (file);
		labels.erase (i->label);
		i->label = labels.end ();
	}
}

void File::clean ()
{
	for (std::list <Block>::iterator b = blocks.begin (); b != blocks.end (); ++b)
		b->clean (true);
}
