#include "asm.hh"

void ExprElem::print (std::stack <std::string> &stack)
{
	switch (type)
	{
	case NUM:
		dbg ("printing " << value.value);
		if (!value.valid)
			shevek_error ("unexpected invalid number");
		stack.push (shevek::rostring ("%d", value.value));
		break;
	case OPER:
		dbg ("printing " << oper->name);
		oper->print (stack);
		break;
	case PARAM:
		if (param.empty ())
			abort ();
		stack.push (param.back ().print ()); // TODO: use constraints.
		break;
	case LABEL:
		dbg ("printing " << label);
		stack.push (label);
		break;
	case ISLABEL:
		stack.push ('?' + label);
		break;
	default:
		shevek_error ("invalid case reached");
	}
}

std::string Expr::print ()
{
	std::stack <std::string> ret;
	for (std::list <ExprElem>::iterator i = list.begin (); i != list.end (); ++i)
		i->print (ret);
	return ret.empty () ? std::string ("#") : ret.top ();
}


