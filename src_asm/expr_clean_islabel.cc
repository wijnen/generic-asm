#include "asm.hh"

void Expr::clean_islabel ()
{
	for (std::list <Expr>::iterator i = children.begin (); i != children.end (); ++i)
		i->clean_islabel ();
	if (type != ISLABEL)
		return;
	type = NUM;
	value.valid = true;
	value.value = find_label (label) != labels.end ();
	label.clear ();
}
