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
			return shevek::rostring ("#%x#", param->mask);
		std::string ret;
		std::string sep;
		for (std::list <Expr>::const_iterator i = children.begin (); i != children.end (); ++i)
		{
			ret += sep + i->print ();
			sep = ";";
		}
		return '[' + ret + ']';
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
