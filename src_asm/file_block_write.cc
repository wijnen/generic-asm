#include "asm.hh"

void File::Block::write_binary ()
{
	std::list <Space>::iterator si = spaces.begin ();
	unsigned size;
	unsigned old_addr = addr;
	while (true)
	{
		addr = old_addr;
		if (!absolute)
		{
			if (si == spaces.end ())
				shevek_error ("out of memory");
			address = si->start;
		}
		for (std::list <Part>::iterator i = parts.begin (); i != parts.end (); ++i)
		{
			unsigned num_true = 0, num_false = 0;
			Expr::valid_int vi ("!!");
			switch (i->type)
			{
			case Part::IF:
				vi = i->expr.compute ();
				if (!vi.valid)
					shevek_error ("invalid if expression");
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
					write_byte (i->expr.compute (), 0);
					++addr;
				}
				break;
			case Part::CODE:
				if (!num_false)
				{
					for (unsigned t = 0; t < i->name.size (); ++t)
					{
						vi.valid = true;
						vi.value = i->name[t];
						write_byte (vi, t);
					}
				}
				addr += i->name.size ();
				break;
			case Part::COMMENT:
				break;
			default:
				shevek_error ("invalid case reached");
			}
		}
		// Clean up.
		files.back ().blocks.back ().clean (false);
		size = addr - old_addr;
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

void File::Block::write_object (std::string &script, std::string &code)
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
				script += shevek::rostring ("%s=%s\n", i->name, i->label->value.print ());
			else
				script += shevek::rostring ("%s=%s\n", i->name, i->expr.print ());
			break;
		case Part::BYTE:
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
			shevek_error ("invalid case reached");
		}
	}
}
