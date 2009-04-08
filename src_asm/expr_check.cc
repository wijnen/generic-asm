#include "asm.hh"

void Expr::check (bool file)
{
	for (std::list <ExprElem>::iterator i = list.begin (); i != list.end (); ++i)
	{
		if (i->type != ExprElem::ISLABEL)
			continue;
		if (i->label[0] != '.' && (!file || i->label[0] != '@'))
			continue;
		i->type = ExprElem::NUM;
		i->value.valid = true;
		i->value.value = find_label (i->label) != labels.end ();
		i->label.clear ();
	}
	simplify ();
	for (std::list <ExprElem>::iterator i = list.begin (); i != list.end (); ++i)
	{
		if (i->type != ExprElem::LABEL)
			continue;
		if (i->label[0] != '.' && (!file || i->label[0] != '@'))
			continue;
		error ("undefined local label " + i->label);
	}
}
