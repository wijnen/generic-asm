#include "asm.hh"

static void simplify_oper (Expr &self, int zero)
{
	Expr *f = &self.children.front ();
	Expr *b = &self.children.back ();
	if ((b->type == Expr::NUM && b->value.valid) || f->type == Expr::OPER)
	{
		Expr e = self.children.front ();
		self.children.front () = self.children.back ();
		self.children.back () = e;
		f = &self.children.front ();
		b = &self.children.back ();
	}
	if (zero >= 0 && f->type == Expr::NUM && f->value.valid && f->value.value == zero)
	{
		Expr e = self.children.back ();
		self = e;
		return;
	}
	if (b->type == Expr::OPER && b->oper->code == self.oper->code && b->children.front ().type == Expr::NUM && b->children.front ().value.valid)
	{
		if (f->type == Expr::NUM && f->value.valid)
		{
			switch (self.oper->code)
			{
			case '+':
				self.children.front ().value.value += self.children.back ().children.front ().value.value;
				break;
			case '*':
				self.children.front ().value.value *= self.children.back ().children.front ().value.value;
				break;
			case '&':
				self.children.front ().value.value &= self.children.back ().children.front ().value.value;
				break;
			case '|':
				self.children.front ().value.value |= self.children.back ().children.front ().value.value;
				break;
			case '^':
				self.children.front ().value.value ^= self.children.back ().children.front ().value.value;
				break;
			default:
				shevek_error ("BUG: invalid case reached");
			}
			Expr e = self.children.back ().children.back ();
			self.children.back () = e;
		}
		else
		{
			Expr e = self.children.front ();
			self.children.front () = self.children.back ().children.front ();
			self.children.back ().children.front () = e;
		}
	}
}

void Expr::simplify (bool set_addr)
{
	if (type == NUM)
		return;
	valid_int i = compute (valid_int (set_addr ? "$=" : "$?"));
	if (i.valid)
	{
		*this = Expr (NUM, i.value);
		return;
	}

	if (type == OPER && oper->code == '-')
	{
		oper = plus_oper;
		Expr e = children.back ();
		children.back () = Expr (OPER, valid_int (")("), pre_minus_oper);
		children.back ().children.push_back (e);
	}

	for (std::list <Expr>::iterator i = children.begin (); i != children.end (); ++i)
		i->simplify ();

	if (type == OPER && oper->code == '_')
	{
		if (children.back ().type == OPER && children.back ().oper->code == '_')
		{
			Expr e = children.back ().children.back ();
			*this = e;
			for (std::list <Expr>::iterator i = children.begin (); i != children.end (); ++i)
				i->simplify ();
		}
		else if (children.back ().type == OPER && children.back ().oper->code == '+')
		{
			Expr e1 = children.back ().children.front ();
			Expr e2 = children.back ().children.back ();
			children.back ().children.front () = Expr (OPER, valid_int ("-+-"), pre_minus_oper);
			children.back ().children.back () = Expr (OPER, valid_int ("-+-"), pre_minus_oper);
			children.back ().children.front ().children.push_back (e1);
			children.back ().children.back ().children.push_back (e2);
			Expr e = children.back ();
			*this = e;
			for (std::list <Expr>::iterator i = children.begin (); i != children.end (); ++i)
				i->simplify ();
		}
	}

	i = compute (valid_int (set_addr ? "$=" : "$?"));
	if (i.valid)
	{
		*this = Expr (NUM, i.value);
		return;
	}

	switch (type)
	{
	case OPER:
	{
		if (oper->code == '+')
		{
			simplify_oper (*this, 0);
			if (type == OPER && oper->code == '+' && children.front ().type == LABEL && children.front ().label == "$$" && children.back ().type == OPER && children.back ().oper->code == '_' && children.back ().children.back ().type == LABEL && children.back ().children.back ().label == "$$")
				*this = Expr (NUM, valid_int (0));
		}
		else if (oper->code == '*')
			simplify_oper (*this, 1);
		else if (oper->code == '&')
			simplify_oper (*this, -1);
		else if (oper->code == '|')
			simplify_oper (*this, -1);
		else if (oper->code == '^')
			simplify_oper (*this, -1);
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
