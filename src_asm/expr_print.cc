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
	{
		if (param.empty ())
			shevek_error ("BUG: printing empty parameter");
		std::string ret;
		std::string sep;
		for (std::list <Expr>::iterator i = param.begin (); i != param.end (); ++i)
		{
			ret += sep + i->print ();
			sep = ";";
		}
		stack.push ('[' + ret + ']');
		break;
	}
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

std::string ExprElem::dump ()
{
	switch (type)
	{
	case NUM:
		if (!value.valid)
			return "invalid_number";
		return shevek::rostring ("%d", value.value);
	case OPER:
		return oper->name;
	case PARAM:
	{
		if (param.empty ())
			return "empty_param";
		std::string ret = "[ ";
		for (std::list <Expr>::iterator i = param.begin (); i != param.end (); ++i)
			ret += i->dump () + "; ";
		return ret + " ]";
	}
	case LABEL:
		return label;
	case ISLABEL:
		return '?' + label;
	default:
		shevek_error ("invalid case reached");
		return std::string ();
	}
}

std::string Expr::print ()
{
	std::stack <std::string> ret;
	for (std::list <ExprElem>::iterator i = list.begin (); i != list.end (); ++i)
		i->print (ret);
	return ret.empty () ? std::string ("#") : ret.top ();
}

std::string Expr::dump ()
{
	std::string ret;
	std::string sep = "";
	for (std::list <ExprElem>::iterator i = list.begin (); i != list.end (); ++i)
	{
		ret += sep + i->dump ();
		sep = " ";
	}
	return ret;
}


