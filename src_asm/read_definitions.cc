#include "asm.hh"

void read_definitions ()
{
	bool is_enum = false, is_num = false, is_source = false;
	bool recording = false;
	std::map <std::string, unsigned>::iterator current_enum;
	int current_value;
	std::map <std::string, Param>::iterator current_param;
	input_stack.top ().ln = 0;
	std::string line;
	while (getline (line))
	{
		shevek::istring l (line);
		l (" ");
		if (l.rest ().empty () || l ("#"))
			continue;
		if (recording)
		{
			if (l ("endmacro: "))
			{
				if (!recording)
					error ("endmacro without macro");
				recording = false;
				if (!l ("%") && !l ("#"))
					error ("junk after endmacro");
				continue;
			}
			defs_macros.back ().code.push_back (line);
			continue;
		}
		std::string d;
		if (l ("enum: %s %", d))
		{
			is_num = false;
			is_enum = false;
			is_source = false;
			if (Param::find (d) != params.rend ())
			{
				error (shevek::ostring ("duplicate definition "
							"of param %s", d));
				continue;
			}
			is_enum = true;
			current_param = params.insert (std::make_pair (d, Param ())).first;
			current_param->second.is_enum = true;
			current_param->second.is_active = false;
			current_enum = current_param->second.enum_values.end ();
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
			if (Param::find (d) != params.rend ())
			{
				error (shevek::ostring ("duplicate name in param: %s", d));
				continue;
			}
			current_enum = current_param->second.enum_values.insert (std::make_pair (d, current_value++)).first;
		}
		else if (l ("value: %l", d))
		{
			is_num = false;
			is_source = false;
			if (!is_enum || current_enum == current_param->second.enum_values.end ())
			{
				error ("value without enum");
				continue;
			}
			current_enum->second = read_expr (d);
			current_enum = current_param->second.enum_values.end ();
		}
		else if (l ("num: %l", d))
		{
			is_num = false;
			is_enum = false;
			is_source = false;
			if (Param::find (d) != params.rend ())
			{
				error (shevek::ostring ("duplicate definition "
							"of param %s", d));
				continue;
			}
			is_num = true;
			current_param = params.insert (std::make_pair (d, Param ())).first;
			current_param->second.is_enum = false;
			current_param->second.is_active = false;
		}
		else if (l ("constraint: %l", d))
		{
			is_enum = false;
			is_source = false;
			if (!is_num)
			{
				error ("constraint without num");
				continue;
			}
			//current_params->second.constraint = read_expr (d);
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
				std::map <std::string, Param>::reverse_iterator firstp;
				for (std::map <std::string, Param>::reverse_iterator i = params.rbegin (); i != params.rend (); ++i)
				{
					std::string::size_type p = d.find (i->first, pos);
					if (p < first)
					{
						first = p;
						firstp = i;
					}
				}
				if (first >= d.size ())
					break;
				sources.back ().parts.push_back (std::make_pair (d.substr (pos, first - pos), firstp));
				pos = first + firstp->first.size ();
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
						if (!l (" %s", i->args[k].second))
						{
							error ("not enough macro arguments");
							break;
						}
					}
					if (k < i->args.size ())
						break;
					input_stack.push (Input ());
					input_stack.top ().name = i->name;
					input_stack.top ().ln = 0;
					input_stack.top ().type = Input::MACRO;
					input_stack.top ().macro = i;
				}
			}
			if (i != defs_macros.end ())
				continue;
			error (shevek::ostring ("syntax error trying to "
						"parse `%s'", line));
		}
	}
}
