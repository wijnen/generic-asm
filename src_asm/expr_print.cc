#include "asm.hh"

std::string Expr::print ()
{
	switch (type)
	{
	case NUM:
		dbg ("printing " << value.value);
		if (!value.valid)
			return "{invalid}";
		return shevek::rostring ("%d", value.value);
	case OPER:
		dbg ("printing " << oper->name);
		return oper->print (children);
	case PARAM:
	{
		if (param.empty ())
			shevek_error ("BUG: printing empty parameter");
		if (param.size () == 1 && param.front ().type == Expr::NUM && !param.front ().value.valid)
			return "#";
		std::string ret;
		std::string sep;
		for (std::list <Expr>::iterator i = param.begin (); i != param.end (); ++i)
		{
			ret += sep + i->print ();
			sep = ";";
		}
		return '[' + ret + ']';
	}
	case LABEL:
		dbg ("printing " << label);
		return label;
	case ISLABEL:
		return '?' + label;
	default:
		shevek_error ("invalid case reached");
		return std::string ();
	}
}
