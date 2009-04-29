#include "asm.hh"

std::string Expr::print () const
{
	switch (type)
	{
	case NUM:
		if (!value.valid)
			return "{invalid}";
		return shevek::rostring ("%d", value.value);
	case OPER:
		return oper->print (children, oper->priority);
	case PARAM:
	{
		if (children.empty ())
			return "#";
		std::string ret;
		for (std::list <Expr>::const_iterator i = constraints.begin (); i != constraints.end (); ++i)
		{
			ret += i->print () + ';';
		}
		return '[' + ret + std::string (shevek::rostring ("%d;%d", children.back ().value.value, children.front ().print ())) + ']';
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
