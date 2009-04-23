#include "asm.hh"

static void store (std::list <std::list <Param>::iterator> &src, std::list <std::pair <int, int> > &dst)
{
	dst.clear ();
	for (std::list <std::list <Param>::iterator>::iterator i = src.begin (); i != src.end (); ++i)
		dst.push_back (std::make_pair ((*i)->value.value.value, (*i)->dismask));
}

static void restore (std::list <std::pair <int, int> > &src, std::list <std::list <Param>::iterator> &dst)
{
	std::list <std::pair <int, int> >::iterator s;
	std::list <std::list <Param>::iterator>::iterator d;
	for (s = src.begin (), d = dst.begin (); s != src.end (); ++s, ++d)
	{
		(*d)->value.value.value = s->first;
		(*d)->dismask = s->second;
	}
}

static void too_complex (Expr &e)
{
	error ("too complex expression for disassembly in definitions file: " + e.print ());
}

static int make_mask (Expr e)
{
	switch (e.type)
	{
	case Expr::NUM:
		if (e.value.valid)
			return e.value.value;
		// Fall through.
	case Expr::LABEL:
		return ~0;
	case Expr::ISLABEL:
		return 1;
	case Expr::PARAM:
		return e.param->mask;
	case Expr::OPER:
		switch (e.oper->code)
		{
		case '!':
		case '[':
		case ']':
		case '<':
		case '>':
		case '=':
		case '1':
		case '7':
		case '6':
		case '\\':
			return 1;
		case '~':
		case '_':
		case '+':
		case '-':
		case '*':
		case '/':
		case '%':
			return ~0;
		case '{':
		{
			Expr::valid_int vi = e.children.back ().compute (Expr::valid_int ("{{"));
			if (!vi.valid)
				return ~0;
			return make_mask (e.children.front ()) << vi.value;
		}
		case '}':
		{
			Expr::valid_int vi = e.children.back ().compute (Expr::valid_int ("}}"));
			if (!vi.valid)
				return ~0;
			return make_mask (e.children.front ()) >> vi.value;
		}
		case '&':
			return make_mask (e.children.front ()) & make_mask (e.children.back ());
		case '|':
		case '^':
			return make_mask (e.children.front ()) | make_mask (e.children.back ());
		case ':':
		{
			Expr::valid_int vi = e.children.front ().compute (Expr::valid_int ("::?"));
			if (!vi.valid)
				return ~0;
			return vi.value ? make_mask (*++e.children.begin ()) : make_mask (e.children.back ());
		}
		default:
			shevek_error ("invalid case reached");
			return 0;
		}
	default:
		shevek_error ("invalid case reached");
		return 0;
	}
}

static bool fit (int ch, int mask, Expr expr, std::list <std::list <Param>::iterator> &param)
{
	ch &= mask;
	expr.simplify ();
	int shift;
	std::list <std::pair <int, int> > oldparams;
	switch (expr.type)
	{
	case Expr::NUM:
		if (!expr.value.valid)
		{
			error ("invalid expression in definitions file");
			return false;
		}
		return (expr.value.value & mask) == ch;
	case Expr::LABEL:
		error ("label in definions file: " + expr.label);
		return false;
	case Expr::ISLABEL:
		// No labels are defined.
		return ch == 0;
	case Expr::PARAM:
		if (expr.param->is_enum)
		{
			bool have_one = false;
			for (std::map <std::string, Expr::valid_int>::iterator i = expr.param->enum_values.begin (); i != expr.param->enum_values.end (); ++i)
			{
				if (!i->second.valid)
					continue;
				if ((i->second.value & mask) != ch)
					i->second.valid = false;
				else
					have_one = true;
			}
			return have_one;
		}
		else
		{
			int overlap = expr.param->dismask & mask;
			if (((expr.param->value.value.value ^ ch) & overlap) != 0)
				return false;
			expr.param->dismask |= mask;
			expr.param->value.value.value &= ~mask;
			expr.param->value.value.value |= ch;
			return true;
		}
	case Expr::OPER:
		switch (expr.oper->code)
		{
		case '!':
			if (ch != 0 && ch != 1)
				return false;
			return fit (!ch, 1, expr.children.front (), param);
		case '~':
			return fit (~ch, mask, expr.children.front (), param);
		case '_':
			return fit (-ch, mask, expr.children.front (), param);
		case '+':
			if (expr.children.front ().type != Expr::NUM)
			{
				too_complex (expr);
				return false;
			}
			return fit (ch - expr.children.front ().value.value, ~0, expr.children.back (), param);
		case '-':
			if (expr.children.front ().type != Expr::NUM)
			{
				too_complex (expr);
				return false;
			}
			return fit (expr.children.front ().value.value - ch, ~0, expr.children.back (), param);
		case '*':
			// TODO: detect powers of 2
			if (expr.children.front ().type != Expr::NUM)
			{
				too_complex (expr);
				return false;
			}
			return fit (ch / expr.children.front ().value.value, ~0, expr.children.back (), param);
		case '/':
			// TODO: detect powers of 2
			if (expr.children.front ().type != Expr::NUM)
			{
				if (expr.children.back ().type != Expr::NUM)
				{
					too_complex (expr);
					return false;
				}
				return fit (ch * expr.children.back ().value.value, mask, expr.children.front (), param);
			}
			return fit (expr.children.front ().value.value / ch, ~0, expr.children.back (), param);
		case '%':
		case '[':
		case ']':
		case '<':
		case '>':
		case '=':
		case '1':
			too_complex (expr);
			return false;
		case '{':
			if (expr.children.back ().type != Expr::NUM)
			{
				too_complex (expr);
				return false;
			}
			shift = expr.children.back ().value.value;
			if ((ch & ((1 << shift) - 1)) != 0)
				return false;
			return fit (ch >> shift, mask >> shift, expr.children.front (), param);
		case '}':
			if (expr.children.back ().type != Expr::NUM)
			{
				too_complex (expr);
				return false;
			}
			shift = expr.children.back ().value.value;
			return fit (ch << shift, mask << shift, expr.children.front (), param);
		case '7':
			if (ch != 0 && ch != 1)
				return false;
			store (param, oldparams);
			if (!fit (1, 1, expr.children.front (), param))
				return false;
			if (fit (1, 1, expr.children.back (), param))
				return true;
			restore (oldparams, param);
			return false;
		case '6':
			if (ch != 0 && ch != 1)
				return false;
			store (param, oldparams);
			if (fit (1, 1, expr.children.front (), param))
				return true;
			if (!fit (1, 1, expr.children.back (), param))
				return false;
			restore (oldparams, param);
			return true;
		case '\\':
		{
			if (ch != 0 && ch != 1)
				return false;
			bool a, b;
			store (param, oldparams);
			a = fit (1, 1, expr.children.front (), param);
			b = fit (1, 1, expr.children.back (), param);
			if (!a ^ !b)
				return true;
			restore (oldparams, param);
			return false;
		}
		case '&':
			if (expr.children.front ().type != Expr::NUM)
			{
				too_complex (expr);
				return false;
			}
			if ((~expr.children.front ().value.value & ch) != 0)
			{
				return false;
			}
			return fit (ch, mask & expr.children.front ().value.value, expr.children.back (), param);
		case '|':
		case '^':
		{
			if (expr.children.front ().type == Expr::NUM)
			{
				if (expr.oper->code == '^')
					return fit (ch ^ expr.children.front ().value.value, mask, expr.children.back (), param);
				if ((expr.children.front ().value.value & ~ch) & mask)
					return false;
				return fit (ch, mask & ~expr.children.front ().value.value, expr.children.back (), param);
			}
			int ma = make_mask (expr.children.front ());
			int mb = make_mask (expr.children.back ());
			// Overlapping bits are too complex.
			if (ma & mb)
			{
				too_complex (expr);
				return false;
			}
			// Can we actually set all bits we need?
			if (ch & ~(ma | mb))
				return false;
			store (param, oldparams);
			if (fit (ch, ma, expr.children.front (), param) && fit (ch, mb, expr.children.back (), param))
				return true;
			restore (oldparams, param);
			return false;
		}
		case ':':
		{
			Expr::valid_int vi = expr.children.front ().compute (Expr::valid_int ("??:"));
			if (!vi.valid)
			{
				too_complex (expr);
				return false;
			}
			return vi.value ? fit (ch, mask, *++expr.children.begin (), param) : fit (ch, mask, expr.children.back (), param);
		}
		}
	}
	return false;
}

static void do_disassemble (unsigned max, bool write, std::string const &data, std::string const &defb)
{
	std::string buffer;
	std::string::size_type datapos = 0, datasize = data.size ();
	while (!buffer.empty () || datapos < datasize)
	{
		while (buffer.size () < max && datapos < datasize)
			buffer += data[datapos++];
		if (buffer.empty ())
			break;
		std::list <Source>::iterator s;
		for (s = sources.begin (); s != sources.end (); ++s)
		{
			if (buffer.size () < s->targets.size ())
				continue;
			std::list <std::list <Param>::iterator> p;
			Param::reset ();
			for (std::list <std::pair <std::string, std::list <Param>::iterator> >::iterator pi = s->parts.begin (); pi != s->parts.end (); ++pi)
			{
				p.push_back (pi->second);
				pi->second->is_active = true;
				pi->second->value.value.value = 0;
				pi->second->value.value.valid = false;
				pi->second->dismask = 0;
				for (std::map <std::string, Expr::valid_int>::iterator j = pi->second->enum_values.begin (); j != pi->second->enum_values.end (); ++j)
					j->second.valid = true;
			}
			std::list <std::string>::iterator t;
			unsigned i;
			for (i = 0, t = s->targets.begin (); t != s->targets.end (); ++i, ++t)
			{
				std::string::size_type pos = 0;
				if (!fit (buffer[i] & 0xff, ~0, Expr::read (*t, true, pos), p))
				{
					break;
				}
			}
			if (t != s->targets.end ())
				continue;
			// Check constraints of the parameters.
			std::list <std::list <Param>::iterator>::iterator pi;
			for (pi = p.begin (); pi != p.end (); ++pi)
			{
				(*pi)->value.value.valid = true;
				(*pi)->value.value.value &= (*pi)->dismask;
				if ((*pi)->value.value.value & ~(*pi)->mask)
					break;
				std::list <std::string>::iterator c;
				for (c = (*pi)->constraints.begin (); c != (*pi)->constraints.end (); ++c)
				{
					std::string::size_type pos = 0;
					Expr ce = Expr::read (*c, true, pos);
					Expr::valid_int vi = ce.compute ((*pi)->value.value);
					if (!vi.valid)
					{
						error ("invalid constraint: " + *c);
						break;
					}
					else if (vi.value)
					{
						continue;
					}
					(*pi)->value.value.value -= 0x100;
					vi = ce.compute ((*pi)->value.value);
					if (!vi.valid)
					{
						error ("invalid constraint(1): " + *c);
						break;
					}
					else if (vi.value)
					{
						continue;
					}
					(*pi)->value.value.value |= (*pi)->mask & ~(*pi)->dismask;
					vi = ce.compute ((*pi)->value.value);
					if (!vi.valid)
					{
						error ("invalid constraint(2): " + *c);
						break;
					}
					else if (vi.value)
					{
						continue;
					}
					(*pi)->value.value.value &= 0xff;
					vi = ce.compute ((*pi)->value.value);
					if (!vi.valid)
					{
						error ("invalid constraint(3): " + *c);
						break;
					}
					else if (vi.value)
					{
						continue;
					}
					(*pi)->value.value.value |= ~0xff;
					vi = ce.compute ((*pi)->value.value);
					if (!vi.valid)
					{
						error ("invalid constraint(4): " + *c);
						break;
					}
					else if (vi.value)
					{
						continue;
					}
					dbg (shevek::rostring ("fail constraint %s for 0x%x", *c, (*pi)->value.value.value));
					break;
				}
				if (c != (*pi)->constraints.end ())
					break;
			}
			if (pi != p.end ())
				continue;
			for (std::list <std::pair <std::string, std::list <Param>::iterator> >::iterator pi = s->parts.begin (); pi != s->parts.end (); ++pi)
			{
				if (pi->second->is_enum)
				{
					for (std::map <std::string, Expr::valid_int>::iterator j = pi->second->enum_values.begin (); j != pi->second->enum_values.end (); ++j)
					{
						if (j->second.valid)
						{
							pi->second->value.value = j->second;
							break;
						}
					}
				}
			}
			// Check if the values that were found are indeed valid.
			for (i = 0, t = s->targets.begin (); t != s->targets.end (); ++i, ++t)
			{
				std::string::size_type pos = 0;
				Expr::valid_int vi = Expr::read (*t, true, pos).compute (Expr::valid_int ("%*"));
				if (!vi.valid || (vi.value & 0xff) != (buffer[i] & 0xff))
				{
					break;
				}
			}
			if (t != s->targets.end ())
				continue;
			if (write)
			{
				for (std::list <Label>::iterator l = labels.begin (); l != labels.end (); ++l)
				{
					if (l->value.value.value == (int)addr)
					{
						*outfile << l->name << ":\n";
						l->priority = -1;
					}
				}
				*outfile << '\t';
				for (std::list <std::pair <std::string, std::list <Param>::iterator> >::iterator pi = s->parts.begin (); pi != s->parts.end (); ++pi)
				{
					*outfile << pi->first;
					if (pi->second->is_enum)
					{
						for (std::map <std::string, Expr::valid_int>::iterator j = pi->second->enum_values.begin (); j != pi->second->enum_values.end (); ++j)
						{
							if (j->second.valid)
							{
								*outfile << j->first;
								break;
							}
						}
					}
					else
					{
						std::list <Label>::iterator l;
						for (l = labels.begin (); l != labels.end (); ++l)
						{
							if (l->value.value.value == pi->second->value.value.value)
								break;
						}
						if (l == labels.end ())
							*outfile << "0x" << pi->second->value.value.value;
						else
							*outfile << l->name;
					}
				}
				*outfile << s->post << '\n';
			}
			else
			{
				for (std::list <std::pair <std::string, std::list <Param>::iterator> >::iterator pi = s->parts.begin (); pi != s->parts.end (); ++pi)
				{
					if (pi->second->prefix.empty ())
						continue;
					int priority;
					std::string prefix;
					if (pi->second->value.value.value <= (int)addr && !pi->second->rprefix.empty ())
					{
						prefix = pi->second->rprefix;
						priority = pi->second->priority - 1;
					}
					else
					{
						prefix = pi->second->prefix;
						priority = pi->second->priority;
					}
					std::string name = shevek::rostring ("%s%x", prefix, pi->second->value.value.value);
					std::list <Label>::iterator li;
					for (li = labels.begin (); li != labels.end (); ++li)
					{
						if (li->value.value.value == pi->second->value.value.value)
							break;
					}
					if (li != labels.end ())
					{
						if (li->priority > priority)
							li->name = name;
						continue;
					}
					Label l;
					l.name = name;
					l.definition = NULL;
					l.value = pi->second->value;
					l.priority = pi->second->priority;
					labels.push_back (l);
				}
			}
			buffer = buffer.substr (s->targets.size ());
			addr += s->targets.size ();
			break;
		}
		if (s == sources.end ())
		{
			if (write)
				*outfile << '\t' << defb << " 0x" << (buffer[0] & 0xff) << '\n';
			buffer = buffer.substr (1);
			++addr;
		}
	}
}

void disassemble (std::istream &in)
{
	absolute_addr = true;
	*outfile << std::hex;
	std::string defb, org, equ;
	if (directives[0].nick.empty ())
	{
		std::cerr << "Warning: org has no defined nickname; using \"org\".\n";
		org = "org";
	}
	else
		org = directives[0].nick.front ();
	if (directives[1].nick.empty ())
	{
		std::cerr << "Warning: equ has no defined nickname; using \"equ\".\n";
		equ = "equ";
	}
	else
		equ = directives[1].nick.front ();
	if (directives[2].nick.empty ())
	{
		std::cerr << "Warning: defb has no defined nickname; using \"defb\".\n";
		defb = "defb";
	}
	else
		defb = directives[2].nick.front ();
	unsigned max = 1;
	for (std::list <Source>::iterator s = sources.begin (); s != sources.end (); ++s)
	{
		if (s->targets.size () > max)
			max = s->targets.size ();
	}
	std::ostringstream data;
	data << in.rdbuf ();
	int startaddr = addr;
	do_disassemble (max, false, data.str (), defb);
	addr = startaddr;
	*outfile << shevek::rostring ("\t%s 0x%x\n", org, addr);
	do_disassemble (max, true, data.str (), defb);
	bool first = true;
	for (std::list <Label>::iterator l = labels.begin (); l != labels.end (); ++l)
	{
		if (l->priority >= 0)
		{
			if (first)
			{
				*outfile << '\n';
				first = false;
			}
			*outfile << std::string (shevek::rostring ("%s:\t%s 0x%x\n", l->name, equ, l->value.value.value));
		}
	}
}
