#include "asm.hh"

void ExprElem::compute (std::stack <Expr::valid_int> &stack, Expr::valid_int self) const
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
			stack.push (Expr::valid_int ("[#]"));
		else
			stack.push (param.back ().compute (self));	// Constraints don't need to be checked here; it's done in parse and print.
		break;
	case ExprElem::LABEL:
		dbg ("computing label " << label);
		l = find_label (label);
		if (l == labels.end ())
		{
			dbg ("invalid");
			stack.push (Expr::valid_int (label));
		}
		else
		{
			dbg ("possibly valid");
			Expr::valid_int vi = l->value.compute (Expr::valid_int (":"));
			if (!vi.valid)
				vi.invalid.push_back (l->name);
			stack.push (vi);
		}
		break;
	case ExprElem::ISLABEL:
	{
		dbg ("computing label test " << label);
		l = find_label (label);
		Expr::valid_int i ("?!");	// Use an invalid value by default.  It may be defined later.
		if (l != labels.end ())
			i = Expr::valid_int (1);
		stack.push (i);
		break;
	}
	default:
		error (shevek::rostring ("invalid element type %d", type));
	}
}

Expr::valid_int Expr::compute (valid_int self) const
{
	dbg ("computing");
	if (list.empty ())
		return self;
	std::stack <valid_int> stack;
	for (std::list <ExprElem>::const_iterator i = list.begin (); i != list.end (); ++i)
		i->compute (stack, self);
	if (stack.size () != 1)
	{
		dbg (stack.size ());
		error ("bug in assembler: not 1 value returned by expression");
		return Expr::valid_int ("!");
	}
	return stack.top ();
}
