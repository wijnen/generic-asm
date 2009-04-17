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
		return oper->print (children);
	case PARAM:
	{
		if (children.empty ())
			return "#";
		std::string ret;
		for (std::list <std::string>::const_iterator i = constraints.begin (); i != constraints.end (); ++i)
		{
			ret += *i + ';';
		}
		return '[' + ret + std::string (shevek::rostring ("%d;%d", children.back ().value.value, children.front ().value.value)) + ']';
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
