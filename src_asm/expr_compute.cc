#include "asm.hh"

void ExprElem::compute (std::stack <Expr::valid_int> &stack) const
{
	std::list <Label>::iterator l;
	switch (type)
	{
	case ExprElem::NUM:
		dbg ("computing num " << value.valid << ':' << value.value);
		stack.push (value);
		break;
	case ExprElem::OPER:
		dbg ("computing operator " << oper->name);
		oper->run (stack);
		break;
	case ExprElem::PARAM:
		dbg ("computing param");
		if (param.empty ())
			abort ();
		stack.push (param.back ().compute ()); //TODO: check constraints.
		break;
	case ExprElem::LABEL:
		dbg ("computing label " << label);
		l = find_label (label);
		if (l == labels.end ())
		{
			dbg ("invalid");
			stack.push (Expr::valid_int ());
		}
		else
		{
			dbg ("possibly valid");
			stack.push (l->value.compute ());
		}
		break;
	case ExprElem::ISLABEL:
	{
		dbg ("computing label test " << label);
		l = find_label (label);
		Expr::valid_int i;	// Use an invalid value by default.  It may be defined later.
		if (l != labels.end ())
			i = Expr::valid_int (1);
		stack.push (i);
		break;
	}
	default:
		error (shevek::rostring ("invalid element type %d", type));
	}
}

Expr::valid_int Expr::compute () const
{
	dbg ("computing");
	if (list.empty ())
		return Expr::valid_int ();
	std::stack <valid_int> stack;
	for (std::list <ExprElem>::const_iterator i = list.begin (); i != list.end (); ++i)
		i->compute (stack);
	if (stack.size () != 1)
	{
		dbg (stack.size ());
		error ("bug in assembler: not 1 value returned by expression");
		return Expr::valid_int ();
	}
	return stack.top ();
}
