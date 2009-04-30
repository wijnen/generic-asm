#include "asm.hh"

void read_definitions ()
{
	int priority = 0;
	bool is_enum = false, is_num = false, is_source = false;
	bool recording = false;
	std::map <std::string, Expr::valid_int>::iterator current_enum;
	int current_value = 0;
	std::list <Param>::iterator current_param;
	input_stack.top ().ln = 0;
	std::string line;
	while (getline (line))
	{
		shevek::ristring l (line);
		l (" ");
		if (l.rest ().empty () || l ("#"))
			continue;
		if (recording)
		{
			if (l ("endmacro: "))
			{
				recording = false;
				if (!l ("%") && !l ("#"))
					error ("junk after endmacro");
				continue;
			}
			defs_macros.back ().code.push_back (line);
			continue;
		}
		std::string d, n;
		if (l ("memory: %s %s %", d, n))
		{
			Expr::valid_int st = read_expr (d, "#");
			Expr::valid_int sz = read_expr (n, "#");
			if (!st.valid || !sz.valid || st.value < 0 || sz.value <= 0)
			{
				error ("invalid memory region");
				continue;
			}
			unsigned start = st.value;
			unsigned size = sz.value;
			if (size == 0)
				continue;
			for (std::list <Space>::iterator i = spaces.begin (); i != spaces.end (); ++i)
			{
				if ((i->start >= start && i->start < start + size) || (start >= i->start && start < i->start + i->size))
					error ("overlapping memory regions");
			}
			spaces.push_back (Space ());
			spaces.back ().start = start;
			spaces.back ().size = size;
		}
		else if (l ("enum: %s %", d))
		{
			is_num = false;
			is_enum = false;
			is_source = false;
			if (Param::find (d) != params.end ())
			{
				error (shevek::rostring ("duplicate definition of param %s", d));
				continue;
			}
			is_enum = true;
			params.push_back (Param ());
			current_param = --params.end ();
			current_param->name = d;
			current_param->is_enum = true;
			current_param->is_active = false;
			current_param->mask = 0;
			current_enum = current_param->enum_values.end ();
			current_value = 0;
		}
		else if (l ("name: %l", d))
		{
			is_num = false;
			is_source = false;
			if (!is_enum)
			{
				error ("name without enum");
				continue;
			}
			if (Param::find (d) != params.end ())
			{
				error (shevek::rostring ("duplicate name in param: %s", d));
				continue;
			}
			current_enum = current_param->enum_values.insert (std::make_pair (d, Expr::valid_int (current_value++))).first;
		}
		else if (l ("value: %l", d))
		{
			is_num = false;
			is_source = false;
			if (!is_enum || current_enum == current_param->enum_values.end ())
			{
				error ("value without enum");
				continue;
			}
			current_enum->second = read_expr (d, "#");
			if (!current_enum->second.valid)
			{
				error ("invalid value for enum");
				continue;
			}
			current_param->mask |= current_enum->second.value;
			current_enum = current_param->enum_values.end ();
		}
		else if (l ("num: %l", d))
		{
			is_num = false;
			is_enum = false;
			is_source = false;
			if (Param::find (d) != params.end ())
			{
				error (shevek::rostring ("duplicate definition of param %s", d));
				continue;
			}
			is_num = true;
			params.push_back (Param ());
			current_param = --params.end ();
			current_param->name = d;
			current_param->is_enum = false;
			current_param->is_active = false;
			current_param->mask = ~0;
		}
		else if (l ("mask: %l", d))
		{
			is_enum = false;
			is_source = false;
			if (!is_num)
			{
				error ("mask without num");
				continue;
			}
			Expr::valid_int i = read_expr (d, "#");
			if (!i.valid)
			{
				error ("invalid mask");
				continue;
			}
			current_param->mask &= i.value;
		}
		else if (l ("constraint:"))
		{
			is_enum = false;
			is_source = false;
			if (!is_num)
			{
				error ("constraint without num");
				continue;
			}
			std::string::size_type pos = 0;
			Param::reset ();
			current_param->is_active = true;
			int oldaddr = addr;
			addr = 0;
			Expr e = Expr::read (l.rest (), true, pos);
			addr = oldaddr;
			if (pos == std::string::npos)
			{
				error ("invalid constraint expression: " + l.rest ());
				continue;
			}
			l.skip (pos);
			if (!l (" %") && !l (" #"))
			{
				error (shevek::rostring ("junk after constraint: %s", l.rest ()));
				continue;
			}
			current_param->constraints.push_back (e);
		}
		else if (l ("prefix: %r/[a-zA-Z_@.][a-zA-Z_@.0-9]*/", d))
		{
			is_source = false;
			if (!is_num && !is_enum)
			{
				error ("prefix without num or enum");
				continue;
			}
			current_param->prefix = d;
			priority += 2;
			current_param->priority = priority;
		}
		else if (l ("rprefix: %r/[a-zA-Z_@.][a-zA-Z_@.0-9]*/", d))
		{
			is_source = false;
			if (!is_num && !is_enum)
			{
				error ("prefix without num or enum");
				continue;
			}
			current_param->rprefix = d;
		}
		else if (l ("source: %l", d))
		{
			is_num = false;
			is_enum = false;
			is_source = true;
			sources.push_back (Source ());
			std::string::size_type pos = 0;
			while (true)
			{
				// find next param
				std::string::size_type first = d.size ();
				std::list <Param>::iterator firstp;
				for (std::list <Param>::iterator i = params.begin (); i != params.end (); ++i)
				{
					std::string::size_type p = d.find (i->name, pos);
					if (p < first)
					{
						first = p;
						firstp = i;
					}
				}
				if (first >= d.size ())
					break;
				sources.back ().parts.push_back
					(std::make_pair (d.substr
						 (pos, first - pos), firstp));
				pos = first + firstp->name.size ();
			}
			sources.back ().post = d.substr (pos);
		}
		else if (l ("target: %l", d))
		{
			is_num = false;
			is_enum = false;
			if (!is_source)
			{
				error ("target without source");
				continue;
			}
			sources.back ().targets.push_back (d);
		}
		else if (l ("macro: %s", d))
		{
			is_num = false;
			is_enum = false;
			is_source = false;
			defs_macros.push_back (DefsMacro ());
			defs_macros.back ().name = d;
			while (l (" %s", d))
				defs_macros.back ().args.push_back
					(std::make_pair (d, std::string ()));
			recording = true;
		}
		else if (l ("directive: %s %s", d, n))
		{
			is_num = false;
			is_enum = false;
			is_source = false;
			unsigned i;
			for (i = 0; i < num_elem (directives); ++i)
			{
				if (directives[i].name == d)
				{
					directives[i].nick.push_back (n);
					break;
				}
			}
			if (i == num_elem (directives))
				error ("invalid directive");
		}
		else
		{
			std::list <DefsMacro>::iterator i;
			for (i = defs_macros.begin (); i != defs_macros.end ();
					++i)
			{
				if (l (escape (i->name)))
				{
					unsigned k;
					for (k = 0; k < i->args.size (); ++k)
					{
						if (!l (" %s",
							i->args[k].second))
						{
							error ("not enough "
								"macro "
								"arguments");
							break;
						}
					}
					if (k < i->args.size ())
						break;
					std::string base
						= input_stack.top ().basedir;
					input_stack.push (Input ());
					input_stack.top ().name = i->name;
					input_stack.top ().basedir = base;
					input_stack.top ().ln = 0;
					input_stack.top ().type = Input::MACRO;
					input_stack.top ().macro = i;
					break;
				}
			}
			if (i != defs_macros.end ())
				continue;
			error (shevek::rostring ("syntax error trying to parse `%s'", line));
		}
	}
	if (spaces.empty ())
		error ("no memory region defined");
}
