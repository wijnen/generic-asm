#include "asm.hh"
#include <iostream>
#include <shevek/debug.hh>

int Expr::compute (bool *valid)
{
	dbg ("computing");
	if (valid)
		*valid = true;
	std::stack <int> stack;
	for (std::list <ExprElem>::const_iterator
			i = list.begin (); i != list.end (); ++i)
	{
		Label *l;
		switch (i->type)
		{
		case ExprElem::NUM:
			dbg ("computing num " << i->value);
			stack.push (i->value);
			break;
		case ExprElem::OPER:
			dbg ("computing operator " << i->oper->name);
			i->oper->run (stack);
			break;
		case ExprElem::PARAM:
			if (!i->param->second.is_active)
			{
				error (shevek::ostring ("inactive param %s used", i->param->first));
				if (valid)
					*valid = false;
				stack.push (0);
			}
			else
			{
				dbg ("computing param " << i->param->first);
				stack.push (i->param->second.value);
			}
			break;
		case ExprElem::LABEL:
			dbg ("computing label " << i->label);
			l = find_label (i->label);
			if (!l || !l->valid)
			{
				dbg ("invalid");
				if (valid)
					*valid = false;
				else
					error ("invalid expression");
				stack.push (0);
			}
			else
			{
				dbg ("valid");
				stack.push (l->value);
			}
			break;
		case ExprElem::ISLABEL:
			l = find_label (i->label);
			stack.push (l ? 1 : 0);
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
