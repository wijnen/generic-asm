#include "asm.hh"

static void fill_check (Expr &e, bool file)
{
	if (e.type == Expr::PARAM)
		return;
	for (std::list <Expr>::iterator i = e.children.begin (); i != e.children.end (); ++i)
		fill_check (*i, file);
	if (e.type == Expr::ISLABEL && (e.label[0] == '.' || (file && e.label[0] == '@')))
	{
		e.type = Expr::NUM;
		e.value.valid = true;
		e.value.value = find_label (e.label) != labels.end ();
		e.label.clear ();
	}
}

static void undefined_check (Expr &e, bool file)
{
	if (e.type == Expr::PARAM)
		return;
	for (std::list <Expr>::iterator i = e.children.begin (); i != e.children.end (); ++i)
		undefined_check (*i, file);
	if (e.type == Expr::LABEL && (e.label[0] == '.' || (file && e.label[0] == '@')))
		error ("undefined local label " + e.label);
}

void Expr::check (bool file)
{
	fill_check (*this, file);
	simplify ();
	undefined_check (*this, file);
}
