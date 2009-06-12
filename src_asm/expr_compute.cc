#include "asm.hh"

Expr::valid_int Expr::compute (valid_int self) const
{
	std::list <Label>::iterator l;
	std::list <valid_int> c;
	for (std::list <Expr>::const_iterator i = children.begin (); i != children.end (); ++i)
	{
		c.push_back (i->compute (self));
	}
	switch (type)
	{
	case NUM:
		return value;
	case OPER:
	{
		valid_int ret = oper->run (c);
		return ret;
	}
	case PARAM:
		if (children.empty ())
		{
			return self;
		}
		else
		{
			if (!c.front ().valid)
				return c.front ();
			if (c.front ().value & ~c.back ().value)
				error (shevek::rostring ("value 0x%x not allowed by mask %x", c.front ().value, c.back ().value));
			for (std::list <Expr>::const_iterator i = constraints.begin (); i != constraints.end (); ++i)
			{
				valid_int vi = i->compute (c.front ());
				if (!vi.valid)
					return vi;	// Nothing wrong yet, but if it remains so, the invalidity will become a problem.
				else if (!vi.value)
					error (shevek::rostring ("value 0x%x fails constraint %s", c.front ().value, i->print ()));
			}
			return c.front ();
		}
	case LABEL:
		if (label == "$$" || label == "$")
		{
			if (absolute_addr && (self.invalid.empty () || self.invalid.front () != "$?"))
			{
				return valid_int (addr);
			}
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
