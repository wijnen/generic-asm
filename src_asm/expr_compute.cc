#include "asm.hh"
#include <iostream>
#include <shevek/debug.hh>

Expr::valid_int Expr::compute ()
{
	dbg ("computing");
	std::stack <valid_int> stack;
	for (std::list <ExprElem>::const_iterator
			i = list.begin (); i != list.end (); ++i)
	{
		Label *l;
		switch (i->type)
		{
		case ExprElem::NUM:
			dbg ("computing num " << i->value.valid << ':' << i->value.value);
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
				valid_int i;
				i.valid = false;
				i.value = 0;
				stack.push (i);
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
			if (!l)
			{
				dbg ("invalid");
				valid_int i;
				i.valid = false;
				i.value = 0;
				stack.push (i);
			}
			else
			{
				dbg ("possibly valid");
				stack.push (l->value);
			}
			break;
		case ExprElem::ISLABEL:
			l = find_label (i->label);
			Expr::valid_int i;
			i.valid = true;
			i.value = l ? 1 : 0;
			stack.push (i);
			break;
		}
	}
	if (stack.size () != 1)
	{
		error ("bug in assembler: not 1 value returned by expression");
		Expr::valid_int i;
		i.value = 0;
		i.valid = false;
		return i;
	}
	return stack.top ();
}
