#include "asm.hh"

Expr::valid_int Expr::compute (valid_int self) const
{
	std::list <Label>::iterator l;
	std::list <valid_int> c;
	for (std::list <Expr>::const_iterator i = children.begin (); i != children.end (); ++i)
		c.push_back (i->compute (self));
	switch (type)
	{
	case NUM:
		dbg ("computing num " << value.valid << ':' << value.value);
		return value;
	case OPER:
		dbg ("computing operator " << oper->name);
		return oper->run (c);
	case PARAM:
		dbg ("computing param");
		if (param.empty ())
			return valid_int ("[#]");
		else
			return param.back ().compute (self);	// Constraints don't need to be checked here; it's done in parse and print.
	case LABEL:
		dbg ("computing label " << label);
		l = find_label (label);
		if (l == labels.end ())
		{
			dbg ("invalid");
			return valid_int (label);
		}
		else
		{
			dbg ("possibly valid");
			valid_int vi = l->value.compute (valid_int (":"));
			if (!vi.valid)
				vi.invalid.push_back (l->name);
			return vi;
		}
	case ISLABEL:
	{
		dbg ("computing label test " << label);
		l = find_label (label);
		valid_int i ("?!");	// Use an invalid value by default.  It may be defined later.
		if (l != labels.end ())
			i = valid_int (1);
		return i;
	}
	default:
		error (shevek::rostring ("invalid element type %d", type));
		return valid_int ("!@#$");
	}
}
