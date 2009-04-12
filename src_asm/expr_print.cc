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
		if (param.empty ())
			return "#";
		std::string ret;
		std::string sep;
		for (std::list <Expr>::const_iterator i = param.begin (); i != param.end (); ++i)
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
