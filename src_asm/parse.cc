#include "asm.hh"
#include <shevek/debug.hh>

unsigned parse (input_line &input, bool output, bool first_pass, bool report)
{
	current_stack = &input.stack;
	unsigned undef = 0;
	bool make_label = false;
	std::string label;
	Label *new_label = NULL;
	int old_label_value = 0;
	bool old_label_valid = false;
	shevek::istring l (input.data);
	if (l (" %r/[a-zA-Z_.][a-zA-Z_.0-9]*/:", label))
	{
		make_label = true;
		new_label = find_label (label);
		if (first_pass && new_label)
		{
			error (shevek::ostring ("Duplicate definition "
						"of label %s", label));
			current_stack = &new_label->definition->stack;
			error ("Previous definition was here");
			current_stack = &input.stack;
		}
		if (!new_label)
		{
			labels.push_back (Label ());
			new_label = &labels.back ();
			new_label->name = label;
			new_label->definition = &input;
			new_label->value = addr;
			new_label->valid = true;
		}
		else
		{
			old_label_valid = true;
			old_label_value = new_label->value;
		}
	}
	l (" ");
	if (l.rest ().empty ())
	{
		if (output && listfile)
			*listfile << std::setw (4) << std::setfill ('0')
				<< addr << " \t\t\t" << std::dec
				<< std::setw (6) << std::setfill (' ')
				<< input.stack.back ().first << std::hex
				<< "  " << input.data << '\n';
		return 0;
	}
	l.push ();
	// Check if it's a directive
	unsigned i;
	for (i = 0; i < num_elem (directives); ++i)
	{
		std::list <std::string>::iterator k;
		for (k = directives[i].nick.begin ();
				k != directives[i].nick.end (); ++k)
		{
			if (l (escape (*k)))
			{
				if (output && listfile)
				{
					*listfile << std::setw (4)
						<< std::setfill ('0')
						<< addr << ' ';
				}
				unsigned lineno = input.stack.back ().first;
				std::string line = input.data;
				undef += directives[i].function
					(l, output, first_pass, new_label);
				if (output && listfile)
				{
					*listfile << std::dec << std::setw (6)
						<< std::setfill (' ')
						<< lineno << std::hex << "  "
						<< line << '\n';
				}
				return undef;
			}
		}
	}
	for (std::list <Source>::iterator
			s = sources.begin (); s != sources.end (); ++s)
	{
		// Restore start of line (after label).
		l.pop ();
		l.push ();
		// Set all params to unused.
		Param::reset ();
		std::list <std::pair <std::string, std::map <std::string,
			Param>::reverse_iterator> >::iterator p;
		for (p = s->parts.begin (); p != s->parts.end (); ++p)
		{
			p->second->second.is_active = true;
			if (!l (escape (p->first)))
				break;
			if (p->second->second.is_enum)
			{
				std::map <std::string, unsigned>::iterator v;
				for (v = p->second->second.enum_values
						.begin (); v != p->second
						->second.enum_values.end ();
						++v)
				{
					if (!l (escape (v->first)))
						continue;
					p->second->second.value = v->second;
					break;
				}
				if (v == p->second->second.enum_values.end ())
					break;
			}
			else
			{
				std::string::size_type pos = 0;
				bool valid;
				p->second->second.value = read_expr
					(l.rest (), false, pos, &valid);
				if (pos == std::string::npos)
				{
					dbg ("failed to read expression");
					break;
				}
				if (!valid)
				{
					dbg ("invalid expression found");
					if (report)
						error ("invalid expression "
								"is here");
					++undef;
				}
				l.skip (pos);
			}
		}
		if (p != s->parts.end () || !l (escape (s->post))
				|| (!l (" %") && !l (" ;")))
			continue;
		if (make_label)
		{
			if (old_label_valid
					&& new_label->value != old_label_value)
				error (shevek::ostring ("Value of label %s "
							"changed from %x to "
							"%x", new_label->name,
							old_label_value,
							new_label->value));
		}
		if (output)
		{
			if (listfile)
				*listfile << std::setw (4)
					<< std::setfill ('0') << addr << ' ';
			write_out (*s);
			if (listfile)
			{
				int size = 5 + 3 * s->targets.size ();
				int numtabs = (31 - size) / 8;
				*listfile << std::string (numtabs, '\t')
					<< std::dec << std::setw (6)
					<< std::setfill (' ')
					<< input.stack.back ().first
					<< std::hex << "  " << input.data
					<< '\n';
			}
		}
		else
			addr += s->targets.size ();
		return undef;
	}
	error (shevek::ostring ("Syntax error: %s", l.rest ()));
	return undef;
}
