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
	{
		dbg (print ());
		valid_int ret = oper->run (c);
		dbg (oper->code << ret.valid << ',' << ret.value);
		return ret;
	}
	case PARAM:
		if (children.empty ())
		{
			dbg ('#' << self.valid << ',' << self.value);
			return self;
		}
		else
		{
			std::list <Expr>::const_iterator e = children.end ();
			--e;
			dbg (e->print ());
			Expr::valid_int ret = e->compute (self);
			if (!ret.valid)
				return ret;
			--e;
			dbg (e->print ());
			int mask = e->value.value;
			if (ret.value & ~mask)
				error (shevek::rostring ("value 0x%x not allowed by mask %x", ret.value, mask));
			for (std::list <Expr>::const_iterator i = children.begin (); i != e; ++i)
			{
				valid_int vi = i->compute (self);
				if (!vi.valid)
					return Expr::valid_int ("#param#");	// Nothing wrong yet, if it remains so, the invalidity will become a problem.
				else if (!vi.value)
					error (shevek::rostring ("value 0x%x fails constraint %s", ret.value, i->print ()));
			}
			return ret;
		}
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
