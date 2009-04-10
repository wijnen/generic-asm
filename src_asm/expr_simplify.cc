#include "asm.hh"

struct element : public ExprElem
{
	std::list <element> children;
	element (ExprElem const &that) : ExprElem (that) {}
	void add (std::list <ExprElem> &list)
	{
		for (std::list <element>::iterator c = children.begin (); c != children.end (); ++c)
			c->add (list);
		list.push_back (*this);
	}
};

void Expr::simplify ()
{
	std::stack <valid_int> stack;
	std::list <element> ret;
	for (std::list <ExprElem>::iterator i = list.begin (); i != list.end (); ++i)
	{
		i->compute (stack);
		switch (i->type)
		{
		case ExprElem::OPER:
		{
			std::list <element> c;
			while (ret.size () >= stack.size ())
			{
				c.push_front (ret.back ());
				ret.pop_back ();
			}
			if (i->oper->code == '-')
			{
				if (c.back ().type == ExprElem::NUM && c.back ().value.valid)
				{
					i->oper = plus_oper;
					c.back ().value.value = -c.back ().value.value;
				}
			}
			if (i->oper->code == '+')
			{
				element *f = &c.front ();
				element *b = &c.back ();
				if (f->type == ExprElem::NUM && f->value.valid && f->value.value == 0)
				{
					*i = c.back ();
					c.clear ();
				}
				else if (b->type == ExprElem::NUM && b->value.valid && b->value.value == 0)
				{
					*i = c.front ();
					c.clear ();
				}
				else if (b->type == ExprElem::NUM && b->value.valid)
				{
					element e = c.front ();
					c.front () = c.back ();
					c.back () = e;
				}
				else if (b->type == ExprElem::OPER && b->oper->code == '+' && b->children.front ().type == ExprElem::NUM && b->children.front ().value.valid)
				{
					if (f->type == ExprElem::NUM && f->value.valid)
					{
						c.front ().value.value += c.back ().children.front ().value.value;
						c.back () = c.back ().children.back ();
					}
					else
					{
						element e = c.front ();
						c.front () = c.back ().children.front ();
						c.back ().children.front () = e;
					}
				}
			}
			else if (i->oper->code == '*')
			{
				if (c.front ().type == ExprElem::NUM && c.front ().value.valid && c.front ().value.value == 1)
				{
					*i = c.back ();
					c.clear ();
				}
				else if (c.back ().type == ExprElem::NUM && c.back ().value.valid && c.back ().value.value == 1)
				{
					*i = c.front ();
					c.clear ();
				}
				else if (c.back ().type == ExprElem::NUM && c.back ().value.valid)
				{
					element e = c.front ();
					c.front () = c.back ();
					c.back () = e;
				}
				else if (c.back ().type == ExprElem::OPER && c.back ().oper->code == '*' && c.back ().children.front ().type == ExprElem::NUM && c.back ().children.front ().value.valid)
				{
					if (c.front ().type == ExprElem::NUM && c.front ().value.valid)
					{
						c.front ().value.value *= c.back ().children.front ().value.value;
						c.back () = c.back ().children.back ();
					}
					else
					{
						element e = c.front ();
						c.front () = c.back ().children.front ();
						c.back ().children.front () = e;
					}
				}
			}
			ret.push_back (*i);
			ret.back ().children = c;
			break;
		}
		case ExprElem::NUM:
		case ExprElem::LABEL:
		case ExprElem::ISLABEL:
		case ExprElem::PARAM:
			ret.push_back (*i);
			break;
		}
		if (stack.top ().valid)
		{
			ret.pop_back ();
			ret.push_back (ExprElem (ExprElem::NUM, stack.top ()));
		}
	}
	list.clear ();
	ret.front ().add (list);
}
