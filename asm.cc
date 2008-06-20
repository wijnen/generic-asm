#include <iostream>
#include <fstream>
#include <vector>
#include <shevek/args.hh>
#include <shevek/error.hh>
#include "asm.hh"

unsigned addr;
unsigned errors;

std::stack <Input> input_stack;

std::map <std::string, Param> params;
std::list <Label> labels;
std::list <Source> sources;
std::list <DefsMacro> defs_macros;

void error (std::string const &message)
{
	std::cerr << input_stack.top ().name << ':' << input_stack.top ().ln
		<< ": " << message << std::endl;
	++errors;
}

std::string escape (std::string const &in)
{
	std::string out;
	for (unsigned i = 0; i < in.size (); ++i)
	{
		if (in[i] == '%')
			out += "%%";
		else
			out += in[i];
	}
	return out;
}

Label *find_label (std::string name)
{
	for (std::list <Label>::iterator
		i = labels.begin (); i != labels.end (); ++i)
	{
		if (i->name == name)
			return &*i;
	}
	return NULL;
}

int Expr::compute (bool *valid)
{
	if (valid)
		*valid = true;
	std::stack <int> stack;
	for (std::list <ExprElem>::const_iterator
			i = list.begin (); i != list.end (); ++i)
	{
		switch (i->type)
		{
			int a, b, c;
		case ExprElem::NUM:
			stack.push (i->value);
			break;
		case ExprElem::OPER:
			i->oper->run (stack);
			break;
		case ExprElem::PARAM:
			if (!i->param->second.is_active)
			{
				error (shevek::ostring
						("inactive param %s used",
						 i->param->first));
				if (valid)
					*valid = false;
				else
					error ("invalid expression");
				stack.push (0);
			}
			else
				stack.push (i->param->second.value);
			break;
		case ExprElem::LABEL:
			Label *l;
			l = find_label (i->label);
			if (!l || !l->valid)
			{
				if (valid)
					*valid = false;
				else
					error ("invalid expression");
				stack.push (0);
			}
			else
				stack.push (l->value);
			break;
		}
	}
	if (stack.size () != 1)
	{
		error ("bug in assembler: not 1 value returned by expression");
		if (valid)
			*valid = false;
		else
			error ("invalid expression");
		return 0;
	}
	return stack.top ();
}

template <typename T, unsigned n> unsigned num_elem (T (&arr)[n]) { return n; }

void Expr::handle_oper (std::stack <Oper *> &stack, Oper *oper)
{
	while (!stack.empty () && stack.top ()->priority >= oper->priority)
	{
		list.push_back (ExprElem (ExprElem::OPER, 0, stack.top ()));
		stack.pop ();
	}
	stack.push (oper);
}

Expr Expr::read (std::string const &input, bool allow_params,
		std::string::size_type &pos)
{
	Expr ret;
	shevek::istring l (input.substr (pos));
	bool expect_number = true;
	std::stack <Oper *> opers;
	Oper open ('(', "(", -1, NULL), close (')', ")", -1, NULL);
	Oper tri_start ('?', "?", 0, NULL);
	int correction = 0;
	while (true)
	{
		l (" ");
		if (expect_number)
		{
			// Check prefix operators first
			unsigned i;
			for (i = 0; i < num_elem (operators1); ++i)
			{
				if (l (escape (operators1[i].name)))
				{
					opers.push (&operators1[i]);
					break;
				}
			}
			if (i < num_elem (operators1))
				continue;
			// Parentheses
			if (l ("("))
			{
				opers.push (&open);
				continue;
			}
			std::string word;
			if (l ("%r/[A-Za-z_.][A-Za-z_.0-9]*/", word))
			{
				dbg ("checking word " << word);
				// Param
				if (allow_params)
				{
					std::map <std::string, Param>::reverse_iterator i;
					for (i = params.rbegin ();
							i != params.rend ();
							++i)
					{
						dbg (i->second.is_active << ' ' << i->second.is_enum << ' ' << i->first);
						if (!i->second.is_active)
							continue;
						if (word != i->first)
							continue;
						ret.list.push_back (ExprElem (ExprElem::PARAM, 0, NULL, std::string (), i));
						expect_number = false;
						break;
					}
					if (i != params.rend ())
						continue;
				}
				// Label
				std::list <Label>::const_iterator i;
				for (i = labels.begin (); i != labels.end ();
						++i)
				{
					if (word != i->name)
						continue;
					ret.list.push_back (ExprElem
							(ExprElem::LABEL,
							 0, NULL, i->name));
					expect_number = false;
					dbg ("found label");
					break;
				}
				if (i != labels.end ())
					continue;
				// Label which will be defined later
				ret.list.push_back (ExprElem (ExprElem::LABEL,
							0, NULL, word));
				expect_number = false;
				continue;
			}
			// The special number "$"
			if (l ("$"))
			{
				ret.list.push_back
					(ExprElem (ExprElem::NUM, addr));
				expect_number = false;
				continue;
			}
			// Number
			int n;
			if (!l ("%i", n))
			{
				dbg ("not done");
				pos = std::string::npos;
				return Expr ();
			}
			ret.list.push_back (ExprElem (ExprElem::NUM, n));
			expect_number = false;
		}
		else
		{
			// Operator or closing parentheses.
			unsigned i;
			for (i = 0; i < num_elem (operators2); ++i)
			{
				if (l (escape (operators2[i].name)))
				{
					ret.handle_oper (opers, &operators2[i]);
					expect_number = true;
					break;
				}
			}
			if (i != num_elem (operators2))
				continue;
			if (l (escape (operators3[0].name)))
			{
				ret.handle_oper (opers, &operators3[0]);
				if (ret.list.empty ()
						|| ret.list.back ().type
						!= ExprElem::OPER
						|| ret.list.back ().oper->code
						!= '?')
				{
					dbg ("not done");
					pos = std::string::npos;
					return Expr ();
				}
				ret.list.pop_back ();
				expect_number = true;
				continue;
			}
			if (l (")"))
			{
				ret.handle_oper (opers, &close);
				opers.pop ();
				if (ret.list.empty ()
						|| ret.list.back ().type
						!= ExprElem::OPER
						|| ret.list.back ().oper->code
						!= '(')
				{
					// End of expression.
					correction = 1;
					break;
				}
				ret.list.pop_back ();
				continue;
			}
			// Unknown character: end of expression.
			break;
		}
	}
	while (!opers.empty ())
	{
		if (opers.top () == &open)
		{
			dbg ("not done");
			pos = std::string::npos;
			return Expr ();
		}
		if (opers.top () == &tri_start)
		{
			dbg ("not done");
			pos = std::string::npos;
			return Expr ();
		}
		ret.list.push_back (ExprElem (ExprElem::OPER, 0, opers.top ()));
		opers.pop ();
	}
	dbg ("done");
	pos = input.size () - l.rest ().size () - correction;
	return ret;
}

int read_expr (std::string const &expr, bool allow_params, std::string::size_type &pos, bool *valid)
{
	Expr e = Expr::read (expr, allow_params, pos);
	if (pos != std::string::npos)
	{
#if 0
		std::cerr << "Expression successfully read:";
		for (std::list <ExprElem>::iterator i = e.list.begin ();
				i != e.list.end (); ++i)
		{
			std::cerr << " ";
			switch (i->type)
			{
			case ExprElem::NUM:
				std::cerr << i->value;
				break;
			case ExprElem::OPER:
				std::cerr << i->oper->name;
				break;
			case ExprElem::PARAM:
				std::cerr << i->param->first << '=' << i->param->second.value;
				break;
			case ExprElem::LABEL:
				std::cerr << i->label;
				break;
			default:
				std::cerr << "huh!";
				break;
			}
		}
		std::cerr << std::endl;
#endif

		int ret = e.compute (valid);
		return ret;
	}
	return 0;
}

int read_expr (std::string const &expr)
{
	std::string::size_type pos = 0;
	read_expr (expr, true, pos, NULL);
	if (pos == std::string::npos)
		error (shevek::ostring ("invalid expression: %s", expr));
}

void Param::reset ()
{
	for (std::map <std::string, Param>::reverse_iterator
			i = params.rbegin (); i != params.rend (); ++i)
		i->second.is_active = false;
}

std::map <std::string, Param>::reverse_iterator
	Param::find (std::string const &name)
{
	for (std::map <std::string, Param>::reverse_iterator
			i = params.rbegin (); i != params.rend (); ++i)
		if (i->first == name)
			return i;
	return params.rend ();
}

std::string subst_args (std::string const &orig,
		std::vector <std::pair <std::string, std::string> > const &args)
{
	std::string o = orig;
	for (unsigned i = 0; i < args.size (); ++i)
	{
		std::string ret;
		std::string::size_type pos = 0;
		while (true)
		{
			std::string::size_type
				p = orig.find (args[i].first, pos);
			if (p == std::string::npos)
				break;
			ret += o.substr (pos, p - pos);
			ret += args[i].second;
			pos = p + args[i].first.size ();
		}
		o = ret + o.substr (pos);
	}
	return o;
}

bool getline (std::string &ret)
{
	while (true)
	{
		switch (input_stack.top ().type)
		{
		case Input::FILE:
			if (!std::getline (*input_stack.top ().file, ret))
				return false;
			break;
		case Input::MACRO:
			if (input_stack.top ().ln >= input_stack.top ()
					.macro->code.size ())
				return false;
			ret = input_stack.top ().macro->code
				[input_stack.top ().ln];
			ret = subst_args (ret, input_stack.top ().macro->args);
			break;
		default:
			error ("Bug: invalid case reached");
			return false;
		}
		++input_stack.top ().ln;
		if (!ret.empty ())
			break;
	}
	return true;
}

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

void write_out (Source const &s)
{
	for (std::list <std::string>::const_iterator
			i = s.targets.begin (); i != s.targets.end (); ++i)
	{
		int byte = read_expr (*i);
		if ((byte < -0x80) || (byte >= 0x100))
			error (shevek::ostring ("byte %x out of range", byte));
		std::cout << (char)byte;
	}
}

unsigned parse (std::string line, bool output, bool first_pass)
{
	unsigned undef = 0;
	bool make_label = false;
	std::string label;
	Label *new_label;
	int old_label_value = 0;
	bool old_label_valid = false;
	shevek::istring l (line);
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
		// Set all params to unused.
		Param::reset ();
		std::list <std::pair <std::string, std::map <std::string, Param>::reverse_iterator> >::iterator p;
		for (p = s->parts.begin (); p != s->parts.end (); ++p)
		{
			p->second->second.is_active = true;
			if (!l (escape (p->first)))
				break;
			dbg ("checked " << p->first);
			if (p->second->second.is_enum)
			{
				dbg ("enum");
				std::map <std::string, unsigned>::iterator v;
				for (v = p->second->second.enum_values.begin (); v !=
						p->second->second.enum_values.end ();
						++v)
				{
					if (!l (escape (v->first)))
						continue;
					dbg ("enumcheck " << v->first);
					p->second->second.value = v->second;
					break;
				}
				if (v == p->second->second.enum_values.end ())
					break;
			}
			else
			{
				dbg ("num");
				std::string::size_type pos = 0;
				bool valid;
				p->second->second.value = read_expr (l.rest (), false, pos, &valid);
				if (pos == std::string::npos)
					break;
				if (!valid)
					++undef;
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
		dbg ("done writing " << s->targets.size () << " bytes");
		return undef;
	}
	error ("Syntax error");
	return undef;
}

int main (int argc, char **argv)
{
	std::string defs;
	shevek::args::option opts[] = {
		shevek::args::option (0, "defs", "code definitions",
				false, defs),
	};
	shevek::args args (argc, argv, opts, 0, 1, "Generic assembler", "2008");
	if (defs.empty ())
		shevek_error ("you must specify a definitions file");
	std::ifstream file (defs.c_str ());
	if (!file)
		shevek_error_errno ("unable to open file " << defs);
	std::string line;
	input_stack.push (Input ());
	input_stack.top ().name = defs;
	input_stack.top ().type = Input::FILE;
	input_stack.top ().file = &file;
	read_definitions ();
	if (errors)
		return 1;
	std::vector <std::pair <unsigned, std::string> > input;
	std::istream *infile;
	bool must_delete;
	if (args.size () == 0 || args[0] == "-")
	{
		infile = &std::cin;
		must_delete = false;
		input_stack.top ().name = "Standard input";
	}
	else
	{
		infile = new std::ifstream (args[0].c_str ());
		if (!infile)
			shevek_error_errno ("unable to open input file "
					<< infile);
		must_delete = true;
		input_stack.top ().name = args[0];
	}
	std::string pre;
	input_stack.top ().ln = 0;
	while (std::getline (*infile, line))
	{
		++input_stack.top ().ln;
		if (!line.empty () && line[line.size () - 1] == '\\')
		{
			pre += line.substr (0, line.size () - 1);
			continue;
		}
		input.push_back (std::make_pair (input_stack.top ().ln,
					pre + line));
		pre.clear ();
	}
	if (!pre.empty ())
		input.push_back (std::make_pair (input_stack.top ().ln, pre));
	if (must_delete)
		delete infile;
	// Determine labels
	bool first = true;
	int undefined_labels = -1;
	while (undefined_labels != 0)
	{
		int last_undefined_labels = undefined_labels;
		addr = 0;
		undefined_labels = 0;
		for (unsigned t = 0; t < input.size (); ++t)
		{
			input_stack.top ().ln = input[t].first;
			undefined_labels
				+= parse (input[t].second, false, first);
		}
		if (undefined_labels == last_undefined_labels)
		{
			error ("Label dependency loop");
			return 1;
		}
		if (errors)
			return 1;
		first = false;
	}
	// Write output
	addr = 0;
	for (unsigned t = 0; t < input.size (); ++t)
	{
		input_stack.top ().ln = input[t].first;
		parse (input[t].second, true, false);
	}
	if (errors)
		return 1;
	return 0;
}
