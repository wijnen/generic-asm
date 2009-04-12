#include "asm.hh"

void Block::clean (bool file)
{
	for (std::list <Part>::iterator i = parts.begin (); i != parts.end (); ++i)
	{
		if (i->have_expr)
		{
			i->expr.simplify ();
			Expr::valid_int vi = i->expr.compute (Expr::valid_int ("><"));
			for (std::list <std::string>::iterator j = vi.invalid.begin (); j != vi.invalid.end (); ++j)
			{
				if ((*j)[0] == '.' || (file && (*j)[0] == '@'))
					error ("use of undefined local label " + *j);
			}
		}
		if (i->type != Part::DEFINE)
			continue;
		if (file)
			i->label->definition = NULL;
		if (i->name[0] != '.' && (!file || i->name[0] != '@'))
			continue;
		i->expr = i->label->value;
		i->have_expr = true;
		i->expr.check (file);
		labels.erase (i->label);
		i->label = labels.end ();
	}
	std::list <Part>::iterator i, next;
	for (i = parts.begin (), next = i; i != parts.end (); i = next)
	{
		++next;
		if (i->type == Part::DEFINE && (i->name[0] == '.' || (file && i->name[0] == '@')))
			parts.erase (i);
	}
}
