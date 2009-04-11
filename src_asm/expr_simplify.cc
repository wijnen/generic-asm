#include "asm.hh"

static void simplify_oper (Expr &self, int zero)
{
	Expr *f = &self.children.front ();
	Expr *b = &self.children.back ();
	if (b->type == Expr::NUM && b->value.valid)
	{
		Expr e = self.children.front ();
		self.children.front () = self.children.back ();
		self.children.back () = e;
	}
	if (f->type == Expr::NUM && f->value.valid && f->value.value == zero)
	{
		self = self.children.back ();
		return;
	}
	if (b->type == Expr::OPER && b->oper->code == self.oper->code && b->children.front ().type == Expr::NUM && b->children.front ().value.valid)
	{
		if (f->type == Expr::NUM && f->value.valid)
		{
			if (self.oper->code == '+')
				self.children.front ().value.value += self.children.back ().children.front ().value.value;
			else
				self.children.front ().value.value *= self.children.back ().children.front ().value.value;
			self.children.back () = self.children.back ().children.back ();
		}
		else
		{
			Expr e = self.children.front ();
			self.children.front () = self.children.back ().children.front ();
			self.children.back ().children.front () = e;
		}
	}
}

void Expr::simplify ()
{
	if (type == NUM)
		return;
	valid_int i = compute (valid_int (">?<"));
	if (i.valid)
	{
		*this = Expr (NUM, i.value);
		return;
	}

	for (std::list <Expr>::iterator i = children.begin (); i != children.end (); ++i)
		i->simplify ();

	switch (type)
	{
	case OPER:
	{
		if (oper->code == '-')
		{
			if (children.back ().type == Expr::NUM && children.back ().value.valid)
			{
				oper = plus_oper;
				children.back ().value.value *= -1;
			}
		}
		if (oper->code == '+')
			simplify_oper (*this, 0);
		else if (oper->code == '*')
			simplify_oper (*this, 1);
		break;
	}
	case LABEL:
	{
		std::list <Label>::iterator l = find_label (label);
		if (l == labels.end ())
			return;
		*this = l->value;
		break;
	}
	case NUM:
	case ISLABEL:
	case PARAM:
		break;
	}
}
