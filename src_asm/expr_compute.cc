#include "asm.hh"

Expr::valid_int Expr::compute (valid_int self) const
{
	std::list <Label>::iterator l;
	std::list <valid_int> c;
	for (std::list <Expr>::const_iterator i = children.begin (); i != children.end (); ++i)
	{
		c.push_back (i->compute (self));
		dbg (i->print () << " = " << c.back ().valid << ',' << c.back ().value);
	}
	switch (type)
	{
	case NUM:
		return value;
	case OPER:
		dbg (print ());
		dbg (oper->code << oper->run (c).valid << ',' << oper->run (c).value);
		return oper->run (c);
	case PARAM:
		if (param.empty ())
			return self;
		else
			return param.back ().compute (self);	// Constraints don't need to be checked here; it's done in parse and print.
	case LABEL:
		if (label == "$")
		{
			if (absolute_addr && (self.invalid.empty () || self.invalid.front () != "$?"))
				return valid_int (addr);
			return valid_int ("$");
		}
		l = find_label (label);
		if (l == labels.end ())
		{
			return valid_int (label);
		}
		else
		{
			valid_int vi = l->value.compute (valid_int (":"));
			if (!vi.valid)
				vi.invalid.push_back (l->name);
			return vi;
		}
	case ISLABEL:
	{
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
