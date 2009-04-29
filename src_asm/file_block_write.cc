#include "asm.hh"

#define error(x) do { std::cerr << "Error: " << (x) << '\n'; ++errors; } while (0)

static void clean_addr (Expr &e, bool constraint = false)
{
	for (std::list <Expr>::iterator i = e.children.begin (); i != e.children.end (); ++i)
		clean_addr (*i, constraint);
	for (std::list <Expr>::iterator i = e.constraints.begin (); i != e.constraints.end (); ++i)
		clean_addr (*i, true);
	if (e.type == Expr::LABEL && e.label == "$$")
	{
		e.label = "$";
		if (constraint)
			error ("constraint constains '$'");
	}
}

void Block::lock ()
{
	std::list <Space>::iterator si = spaces.begin ();
	unsigned size;
	absolute_addr = true;	// This is true, because we place the blocks at their final address here.
	while (true)
	{
		if (!absolute)
		{
			if (si == spaces.end ())
				error ("out of memory");
			address = si->start;
			dbg ("trying address " << address);
		}
		addr = address;
		size = 0;
		for (std::list <Part>::iterator i = parts.begin (); i != parts.end (); ++i)
		{
			unsigned num_true = 0, num_false = 0;
			Expr::valid_int vi ("!!");
			switch (i->type)
			{
			case Part::IF:
				clean_addr (i->expr);
				vi = i->expr.compute (Expr::valid_int (">"));
				if (!vi.valid)
					error ("invalid if expression");
				if (num_false == 0 && vi.value)
					++num_true;
				else
					++num_false;
				break;
			case Part::ELSE:
				switch (num_false)
				{
				case 0:
					++num_true;
					--num_false;
					break;
				case 1:
					--num_true;
					++num_false;
					break;
				default:
					break;
				}
				break;
			case Part::ENDIF:
				if (num_false)
					--num_false;
				else
					--num_true;
				break;
			case Part::DEFINE:
				if (i->have_expr)
				{
					i->expr.simplify (true);
					clean_addr (i->expr);
				}
				if (i->label != labels.end ())
				{
					i->label->value.simplify (true);
					clean_addr (i->label->value);
				}
				break;
			case Part::BYTE:
				clean_addr (i->expr);
				if (!num_false)
				{
					Expr::valid_int vi = i->expr.compute (Expr::valid_int ("}{"));
					if (!vi.valid)
					{
						for (std::list <std::string>::iterator j = vi.invalid.begin (); j != vi.invalid.end (); ++j)
							error ("use of undefined label " + *j);
					}
					++size;
				}
				break;
			case Part::CODE:
				if (!num_false)
					size += i->name.size ();
				break;
			case Part::COMMENT:
				break;
			default:
				error ("invalid case reached");
			}
		}
		// Clean up.
		blocks.back ().clean (false);
		if (!absolute && size > si->size)
		{
			// It doesn't fit, try next chunk.
			++si;
			continue;
		}
		break;
	}
	// Cut this chunk out of available chunks.
	dbg ("placed block at " << address << "+" << size);
	std::list <Space>::iterator next;
	for (si = spaces.begin (), next = si; si != spaces.end (); si = next)
	{
		++next;
		if (si->start >= address + size || si->start + si->size <= address)
			continue;
		if (si->start >= address && si->start + si->size <= address + size)
			spaces.erase (si);
		else if (si->start < address && si->start + si->size > address + size)
		{
			spaces.insert (next, Space ());
			--next;
			next->start = address + size;
			next->size = si->size - (address - si->start) - size;
			si->size = address - si->start;
		}
		else if (si->start < address)
			si->size = address - si->start;
		else
			si->start = address + size;
	}
}

void Block::write_binary ()
{
	absolute_addr = true;	// This is true, because the blocks have been placed.
	addr = address;
	unsigned offset = 0;
	for (std::list <Part>::iterator i = parts.begin (); i != parts.end (); ++i)
	{
		unsigned num_true = 0, num_false = 0;
		Expr::valid_int vi ("!!");
		switch (i->type)
		{
		case Part::IF:
			vi = i->expr.compute (Expr::valid_int (">"));
			if (!vi.valid)
				error ("invalid if expression");
			if (num_false == 0 && vi.value)
				++num_true;
			else
				++num_false;
			break;
		case Part::ELSE:
			switch (num_false)
			{
			case 0:
				++num_true;
				--num_false;
				break;
			case 1:
				--num_true;
				++num_false;
				break;
			default:
				break;
			}
			break;
		case Part::ENDIF:
			if (num_false)
				--num_false;
			else
				--num_true;
			break;
		case Part::DEFINE:
			// Definitions are not written to binary output.
			break;
		case Part::BYTE:
			if (!num_false)
			{
				Expr::valid_int vi = i->expr.compute (Expr::valid_int ("}"));
				write_byte (vi, offset++);
			}
			break;
		case Part::CODE:
			if (!num_false)
			{
				for (unsigned t = 0; t < i->name.size (); ++t)
				{
					vi.valid = true;
					vi.value = i->name[t];
					write_byte (vi, offset++);
				}
			}
			break;
		case Part::COMMENT:
			break;
		default:
			error ("invalid case reached");
		}
	}
}

void Block::write_object (std::string &script, std::string &code)
{
	script += '<';
	if (absolute)
		script += shevek::rostring ("0x%x", address);
	script += '\n';
	for (std::list <Part>::iterator i = parts.begin (); i != parts.end (); ++i)
	{
		switch (i->type)
		{
		case Part::IF:
			clean_addr (i->expr);
			script += shevek::rostring ("?%s\n", i->expr.print ());
			break;
		case Part::ELSE:
			script += ":\n";
			break;
		case Part::ENDIF:
			script += ";\n";
			break;
		case Part::DEFINE:
			if (i->label != labels.end ())
			{
				clean_addr (i->label->value);
				script += shevek::rostring ("%s=%s\n", i->name, i->label->value.print ());
			}
			else
			{
				clean_addr (i->expr);
				script += shevek::rostring ("%s=%s\n", i->name, i->expr.print ());
			}
			break;
		case Part::BYTE:
			clean_addr (i->expr);
			script += shevek::rostring ("+%s\n", i->expr.print ());
			break;
		case Part::CODE:
			script += shevek::rostring (">%d\n", i->name.size ());
			code += i->name;
			break;
		case Part::COMMENT:
			script += shevek::rostring ("#%s\n", i->name);
			break;
		default:
			error ("invalid case reached");
		}
	}
}
