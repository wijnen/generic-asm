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
			ret.push_back (*i);
			if (i->code == '+')
			{
				std::list <element>::iterator j, next;
				for (j = c.begin (), next = j; j != c.end (); j = next, ++next)
				{
					if (j->type != ExprElem::NUM || !j->value.valid || j->value.value != 0)
						continue;
					c.erase (j); // TODO: remove operator.
				}
			}
			if (i->code == '*')
			{
				std::list <element>::iterator j, next;
				for (j = c.begin (), next = j; j != c.end (); j = next, ++next)
				{
					if (j->type != ExprElem::NUM || !j->value.valid || j->value.value != 1)
						continue;
					c.erase (j); // TODO: remove operator.
				}
			}
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
