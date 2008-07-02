#include "asm.hh"

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
		}
		if (!new_label)
		{
			labels.push_back (Label ());
			new_label = &labels.back ();
			new_label->name = label;
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
	l.push ();
	for (std::list <Source>::iterator
			s = sources.begin (); s != sources.end (); ++s)
	{
		// Restore start of line (after label).
		l.pop ();
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
					undef += directives[i].function
						(l, output, new_label);
					return undef;
				}
			}
		}
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
					break;
				if (!valid)
				{
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
		if (output)
			write_out (*s);
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
		addr += s->targets.size ();
		return undef;
	}
	error ("Syntax error");
	return undef;
}
