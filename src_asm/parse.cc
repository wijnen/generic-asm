#include "asm.hh"
#include <shevek/debug.hh>

void parse (input_line &input, bool first_pass, bool report)
{
	current_stack = &input.stack;
	bool make_label = false;
	Glib::ustring label;
	Label *new_label = NULL;
	Expr::valid_int old_label;
	old_label.value = 0;
	old_label.valid = false;
	shevek::istring l (input.data);
	if (l (" %r/[a-zA-Z_.][a-zA-Z_.0-9]*/:", label))
	{
		dbg ("found label " << label);
		make_label = true;
		new_label = find_label (label);
		if (first_pass && new_label)
		{
			error (shevek::ostring ("Duplicate definition of label %s", label));
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
			new_label->value.value = addr;
			new_label->value.valid = true;
			dbg ("Label " << label << " defined at " << addr);
		}
		else
		{
			old_label = new_label->value;
		}
	}
	l (" ");
	if (l.rest ().empty ())
	{
		if (writing && listfile)
			*listfile << std::setw (4) << std::setfill ('0')
				<< addr << " \t\t\t" << std::dec
				<< std::setw (6) << std::setfill (' ')
				<< input.stack.back ().first << std::hex
				<< "  " << input.data << '\n';
		return;
	}
	l.push ();
	// Check if it's a directive
	unsigned i;
	for (i = 0; i < num_elem (directives); ++i)
	{
		std::list <Glib::ustring>::iterator k;
		for (k = directives[i].nick.begin ();
				k != directives[i].nick.end (); ++k)
		{
			if (l (escape (*k)))
			{
				if (writing && listfile)
				{
					*listfile << std::setw (4)
						<< std::setfill ('0')
						<< addr << ' ';
				}
				unsigned lineno = input.stack.back ().first;
				Glib::ustring line = input.data;
				directives[i].function (l, first_pass, new_label);
				if (writing && listfile)
				{
					*listfile << std::dec << std::setw (6) << std::setfill (' ') << lineno << std::hex << "  " << line << '\n';
				}
				return;
			}
		}
	}
	for (std::list <Source>::iterator s = sources.begin (); s != sources.end (); ++s)
	{
		// Restore start of line (after label).
		l.pop ();
		l.push ();
		// Set all params to unused.
		Param::reset ();
		std::list <std::pair <Glib::ustring, std::map <Glib::ustring, Param>::reverse_iterator> >::iterator p;
		for (p = s->parts.begin (); p != s->parts.end (); ++p)
		{
			p->second->second.is_active = true;
			if (!l (escape (p->first)))
				break;
			if (p->second->second.is_enum)
			{
				std::map <Glib::ustring, Expr::valid_int>::iterator v;
				for (v = p->second->second.enum_values.begin (); v != p->second->second.enum_values.end (); ++v)
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
				Glib::ustring::size_type pos = 0;
				p->second->second.value = read_expr (l.rest (), false, pos);
				if (pos == Glib::ustring::npos)
				{
					dbg ("failed to read expression");
					break;
				}
				if (!p->second->second.value.valid)
				{
					dbg ("invalid expression found");
					if (report)
						error ("undefined or recursively defined label");
				}
				else
				{
					dbg ("computing " << p->second->second.constraints.size () << " constraints");
					for (std::list <Expr>::iterator i = p->second->second.constraints.begin (); i != p->second->second.constraints.end (); ++i)
					{
						dbg ("computing constraint");
						Expr::valid_int vi = i->compute ();
						if (!vi.valid)
							error ("Constraint is invalid");
						if (!vi.value)
							error (shevek::ostring ("Given value %d fails constraint", p->second->second.value.value));
					}
				}
				l.skip (pos);
			}
		}
		if (p != s->parts.end () || !l (escape (s->post)) || (!l (" %") && !l (" ;")))
			continue;
		if (make_label && old_label.valid)
		{
			if (!new_label->value.valid)
				error (shevek::ostring ("Value of label %s changed from 0x%x to invalid", new_label->name, old_label.value));
			else if (new_label->value.value != old_label.value)
				error (shevek::ostring ("Value of label %s changed from 0x%x to 0x%x", new_label->name, old_label.value, new_label->value.value));
		}
		if (writing && listfile)
			*listfile << std::setw (4) << std::setfill ('0') << addr << ' ';
		write_out (*s);
		if (writing && listfile)
		{
			int size = 5 + 3 * s->targets.size ();
			int numtabs = (31 - size) / 8;
			*listfile << Glib::ustring (numtabs, '\t') << shevek::ostring ("%6d  %s\n", input.stack.back ().first, input.data);
		}
		addr += s->targets.size ();
		return;
	}
	error (shevek::ostring ("Syntax error: %s", l.rest ()));
}
