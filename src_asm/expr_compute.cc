#include "asm.hh"
#include <iostream>

int Expr::compute (bool *valid)
{
	if (valid)
		*valid = true;
	std::stack <int> stack;
	for (std::list <ExprElem>::const_iterator
			i = list.begin (); i != list.end (); ++i)
	{
		switch (i->type)
		{
			int a, b, c;
		case ExprElem::NUM:
			stack.push (i->value);
			break;
		case ExprElem::OPER:
			std::cerr << "running operator " << i->oper->name
				<< std::endl;
			i->oper->run (stack);
			break;
		case ExprElem::PARAM:
			if (!i->param->second.is_active)
			{
				error (shevek::ostring
						("inactive param %s used",
						 i->param->first));
				if (valid)
					*valid = false;
				else
					error ("invalid expression");
				stack.push (0);
			}
			else
				stack.push (i->param->second.value);
			break;
		case ExprElem::LABEL:
			Label *l;
			l = find_label (i->label);
			if (!l || !l->valid)
			{
				if (valid)
					*valid = false;
				else
					error ("invalid expression");
				stack.push (0);
			}
			else
				stack.push (l->value);
			break;
		}
	}
	if (stack.size () != 1)
	{
		error ("bug in assembler: not 1 value returned by expression");
		if (valid)
			*valid = false;
		else
			error ("invalid expression");
		return 0;
	}
	return stack.top ();
}
