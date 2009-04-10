#include "asm.hh"

void parse (input_line &input)
{
	current_stack = &input.stack;
	bool make_label = false;
	std::string label;
	std::list <Label>::iterator new_label = labels.end ();
	Expr::valid_int old_label ("<>");
	shevek::ristring l (input.data);
	if (l (" %r/[a-zA-Z_.@][a-zA-Z_.@0-9]*/:", label))
	{
		dbg ("found label " << label);
		make_label = true;
		new_label = find_label (label);
		if (first_pass && new_label != labels.end ())
		{
			error (shevek::rostring ("Duplicate definition of label %s", label));
			if (new_label->definition)
			{
				current_stack = &new_label->definition->stack;
				error ("Previous definition was here");
				current_stack = &input.stack;
			}
		}
		if (new_label == labels.end ())
		{
			labels.push_back (Label ());
			new_label = --labels.end ();
			new_label->name = label;
			new_label->definition = &input;
			if (absolute_addr)
				new_label->value.list.push_back (ExprElem (ExprElem::NUM, Expr::valid_int (addr)));
			else
			{
				new_label->value.list.push_back (ExprElem (ExprElem::NUM, Expr::valid_int (addr)));
				new_label->value.list.push_back (ExprElem (ExprElem::LABEL, Expr::valid_int ("$!"), NULL, "$"));
				new_label->value.list.push_back (ExprElem (ExprElem::OPER, Expr::valid_int ("+$"), plus_oper));
			}
			if (files.back ().blocks.empty ())
				files.back ().blocks.push_back (File::Block ());
			if (files.back ().blocks.back ().parts.empty ())
				files.back ().blocks.back ().parts.push_back (File::Block::Part ());
			files.back ().blocks.back ().parts.back ().type = File::Block::Part::DEFINE;
			files.back ().blocks.back ().parts.back ().have_expr = false;
			files.back ().blocks.back ().parts.back ().label = new_label;
			files.back ().blocks.back ().parts.back ().name = label;
			dbg ("Label " << label << " defined at " << addr);
		}
		else
		{
			old_label = new_label->value.compute (Expr::valid_int ("::"));
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
		std::list <std::string>::iterator k;
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
				std::string line = input.data;
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
		std::list <std::pair <std::string, std::list <Param>::iterator> >::iterator p;
		for (p = s->parts.begin (); p != s->parts.end (); ++p)
		{
			p->second->is_active = true;
			if (!l (escape (p->first)))
				break;
			if (p->second->is_enum)
			{
				std::map <std::string, Expr::valid_int>::iterator v;
				for (v = p->second->enum_values.begin (); v != p->second->enum_values.end (); ++v)
				{
					if (!l (escape (v->first)))
						continue;
					Expr e;
					e.list.push_back (ExprElem (ExprElem::NUM, v->second));
					p->second->value = e;
					break;
				}
				if (v == p->second->enum_values.end ())
					break;
			}
			else
			{
				std::string::size_type pos = 0;
				p->second->value = Expr::read (l.rest (), false, pos);
				if (pos == std::string::npos)
				{
					dbg ("failed to read expression");
					break;
				}
				Expr::valid_int pvi = p->second->value.compute (Expr::valid_int ("=="));
				if (pvi.valid)
				{
					dbg ("computing " << p->second->constraints.size () << " constraints");
					for (std::list <Expr>::iterator i = p->second->constraints.begin (); i != p->second->constraints.end (); ++i)
					{
						dbg ("computing constraint " << i->print ());
						Expr::valid_int vi = i->compute (pvi);
						if (!vi.valid)
						{
							error ("Constraint is invalid, because of:");
							for (std::list <std::string>::iterator i = vi.invalid.begin (); i != vi.invalid.end (); ++i)
								error ("\tThis label: " + *i);
						}
						if (!vi.value)
							error (shevek::rostring ("Given value %d fails constraint", pvi.value));
					}
				}
				else
				{
					for (std::list <std::string>::iterator iv = pvi.invalid.begin (); iv != pvi.invalid.end (); ++iv)
					{
						if ((*iv)[0] == '.')
						{
							++undefined_locals;
							if (report_labels)
								error ("use of undefined local label: " + *iv);
						}
					}
				}
				l.skip (pos);
			}
		}
		if (p != s->parts.end () || !l (escape (s->post)) || (!l (" %") && !l (" ;")))
			continue;
		if (make_label)
		{
			dbg ("invalid?");
			Expr::valid_int vi = new_label->value.compute (Expr::valid_int ("?:"));
			if (old_label.valid)
			{
				if (!vi.valid)
					error (shevek::rostring ("Value of label %s changed from 0x%x to invalid", new_label->name, old_label.value));
				else if (vi.value != old_label.value)
					error (shevek::rostring ("Value of label %s changed from 0x%x to 0x%x", new_label->name, old_label.value, vi.value));
			}
		}
		if (writing && listfile)
			*listfile << std::setw (4) << std::setfill ('0') << addr << ' ';
		write_out (*s);
		if (writing && listfile)
		{
			int size = 5 + 3 * s->targets.size ();
			int numtabs = (31 - size) / 8;
			*listfile << std::string (numtabs, '\t') << shevek::rostring ("%6d  %s\n", input.stack.back ().first, input.data);
		}
		addr += s->targets.size ();
		return;
	}
	error (shevek::rostring ("Syntax error: %s", l.rest ()));
}
