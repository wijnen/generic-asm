#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <stack>
#include <list>
#include <map>
#include <shevek/iostring.hh>
#include <shevek/args.hh>
#include <shevek/error.hh>

unsigned ln;
unsigned addr;
unsigned errors;
std::string source;

void error (std::string const &message)
{
	std::cerr << source << ':' << ln << ": " << message << std::endl;
	++errors;
}

struct Expr;

struct Param
{
	std::string name;
	bool is_enum;
	std::map <std::string, unsigned> enum_values;
	std::list <Expr> constraints;

	bool is_active;
	int value;

	static void reset ();
	static Param *find (std::string const &name);
};

std::list <Param> params;

struct ExprElem
{
	enum { NUM, OPER, PARAM, LABEL } type;
	int value;
	std::string label;
	Param *param;
};

struct Expr
{
	std::list <ExprElem> list;
	int compute (bool &valid);
};

struct Label
{
	std::string name;
	int value;
	bool valid;
};

std::list <Label> labels;

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

int Expr::compute (bool &valid)
{
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
			switch (i->value)
			{
			case '!':
				a = stack.top ();
				stack.pop ();
				stack.push (!a);
				break;
			case '_':
				a = stack.top ();
				stack.pop ();
				stack.push (-a);
				break;
			case '~':
				a = stack.top ();
				stack.pop ();
				stack.push (~a);
				break;
			case '*':
				a = stack.top ();
				stack.pop ();
				b = stack.top ();
				stack.pop ();
				stack.push (b * a);
				break;
			case '/':
				a = stack.top ();
				stack.pop ();
				b = stack.top ();
				stack.pop ();
				stack.push (b / a);
				break;
			case '%':
				a = stack.top ();
				stack.pop ();
				b = stack.top ();
				stack.pop ();
				stack.push (b % a);
				break;
			case '+':
				a = stack.top ();
				stack.pop ();
				b = stack.top ();
				stack.pop ();
				stack.push (b + a);
				break;
			case '-':
				a = stack.top ();
				stack.pop ();
				b = stack.top ();
				stack.pop ();
				stack.push (b - a);
				break;
			case '{':
				a = stack.top ();
				stack.pop ();
				b = stack.top ();
				stack.pop ();
				stack.push (b << a);
				break;
			case '}':
				a = stack.top ();
				stack.pop ();
				b = stack.top ();
				stack.pop ();
				stack.push (b >> a);
				break;
			case '<':
				a = stack.top ();
				stack.pop ();
				b = stack.top ();
				stack.pop ();
				stack.push (b < a);
				break;
			case '>':
				a = stack.top ();
				stack.pop ();
				b = stack.top ();
				stack.pop ();
				stack.push (b > a);
				break;
			case '[':
				a = stack.top ();
				stack.pop ();
				b = stack.top ();
				stack.pop ();
				stack.push (b <= a);
				break;
			case ']':
				a = stack.top ();
				stack.pop ();
				b = stack.top ();
				stack.pop ();
				stack.push (b >= a);
				break;
			case '=':
				a = stack.top ();
				stack.pop ();
				b = stack.top ();
				stack.pop ();
				stack.push (b == a);
				break;
			case '1':
				a = stack.top ();
				stack.pop ();
				b = stack.top ();
				stack.pop ();
				stack.push (b != a);
				break;
			case '&':
				a = stack.top ();
				stack.pop ();
				b = stack.top ();
				stack.pop ();
				stack.push (b & a);
				break;
			case '^':
				a = stack.top ();
				stack.pop ();
				b = stack.top ();
				stack.pop ();
				stack.push (b ^ a);
				break;
			case '|':
				a = stack.top ();
				stack.pop ();
				b = stack.top ();
				stack.pop ();
				stack.push (b | a);
				break;
			case '7':
				a = stack.top ();
				stack.pop ();
				b = stack.top ();
				stack.pop ();
				stack.push (b && a);
				break;
			case '6':
				a = stack.top ();
				stack.pop ();
				b = stack.top ();
				stack.pop ();
				stack.push (!!b ^ !!a);
				break;
			case '\\':
				a = stack.top ();
				stack.pop ();
				b = stack.top ();
				stack.pop ();
				stack.push (b || a);
				break;
			case '?':
				a = stack.top ();
				stack.pop ();
				b = stack.top ();
				stack.pop ();
				c = stack.top ();
				stack.pop ();
				stack.push (c ? b : a);
				break;
			default:
				error (shevek::ostring ("bug in assembler: "
							"unknown operator `%c'"
							"encountered",
							i->value));
			}
			break;
		case ExprElem::PARAM:
			if (!i->param->is_active)
			{
				valid = false;
				stack.push (0);
			}
			else
				stack.push (i->param->value);
			break;
		case ExprElem::LABEL:
			Label *l;
			l = find_label (i->label);
			if (!l || !l->valid)
			{
				valid = false;
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
		valid = false;
		return 0;
	}
	return stack.top ();
}

int read_expr (std::string const &expr);
int read_expr (std::string const &expr, std::string::size_type &pos);

void Param::reset ()
{
	for (std::list <Param>::iterator
			i = params.begin (); i != params.end (); ++i)
		i->is_active = false;
}

Param *Param::find (std::string const &name)
{
	for (std::list <Param>::iterator
			i = params.begin (); i != params.end (); ++i)
		if (i->name == name)
			return &*i;
	return NULL;
}

struct Source
{
	std::list <std::pair <std::string, Param *> > parts;
	std::string post;
	std::list <std::string> targets;
};

std::list <Source> sources;

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

void read_input (std::istream &file)
{
	bool is_enum = false, is_num = false, is_source = false;
	std::map <std::string, unsigned>::iterator current_enum;
	int current_value;
	ln = 0;
	std::string line;
	while (std::getline (file, line))
	{
		++ln;
		shevek::istring l (line);
		l (" ");
		if (l ("#"))
			continue;
		std::string d;
		if (l ("enum: %s %", d))
		{
			is_num = false;
			is_enum = false;
			is_source = false;
			if (Param::find (d))
			{
				error (shevek::ostring ("duplicate definition "
							"of param %s", d));
				continue;
			}
			is_enum = true;
			params.push_back (Param ());
			params.back ().name = d;
			params.back ().is_enum = true;
			current_enum = params.back ().enum_values.end ();
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
			if (Param::find (escape (d)))
			{
				error (shevek::ostring ("duplicate name in param: %s", d));
				continue;
			}
			current_enum = params.back ().enum_values.insert (std::make_pair (escape (d), current_value++)).first;
		}
		else if (l ("value: %l", d))
		{
			is_num = false;
			is_source = false;
			if (!is_enum || current_enum == params.back ().enum_values.end ())
			{
				error ("value without enum");
				continue;
			}
			current_enum->second = read_expr (d);
			current_enum = params.back ().enum_values.end ();
		}
		else if (l ("num: %l", d))
		{
			is_num = false;
			is_enum = false;
			is_source = false;
			if (Param::find (d))
			{
				error (shevek::ostring ("duplicate definition "
							"of param %s", d));
				continue;
			}
			is_num = true;
			params.push_back (Param ());
			params.back ().name = d;
			params.back ().is_enum = false;
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
			//params.back ().lowest = read_expr (d);
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
				Param *firstp;
				for (std::list <Param>::iterator i = params.begin (); i != params.end (); ++i)
				{
					std::string::size_type p = d.find (i->name, pos);
					if (p < first)
					{
						first = p;
						firstp = &*i;
					}
				}
				if (first >= d.size ())
					break;
				sources.back ().parts.push_back (std::make_pair (escape (d.substr (pos, first - pos)), firstp));
				pos = first + firstp->name.size ();
			}
			sources.back ().post = escape (d.substr (pos));
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
		else
			error (shevek::ostring ("syntax error trying to "
						"parse `%s'", line));
	}
}

void write_out (Source const &s)
{
	for (std::list <std::string>::const_iterator
			i = s.targets.begin (); i != s.targets.end (); ++i)
	{
		unsigned byte = read_expr (*i);
		if (byte < -0x80 || byte >= 0x100)
			error ("byte out of range");
		std::cout << (char)byte;
	}
}

void parse (std::string line, bool output)
{
	shevek::istring l (line);
	for (std::list <Source>::iterator
			s = sources.begin (); s != sources.end (); ++s)
	{
		l.reset ();
		l (" ");
		std::list <std::pair <std::string, Param *> >::iterator p;
		for (p = s->parts.begin (); p != s->parts.end (); ++p)
		{
			if (!l (p->first))
				break;
			if (p->second->is_enum)
			{
				for (std::map <std::string, unsigned>::iterator v = p->second->enum_values.begin (); v != p->second->enum_values.end (); ++v)
				{
					if (!l (escape (v->first)))
						continue;
					p->second->value = v->second;
				}
				break;
			}
			else
			{
				std::string::size_type pos = 0;
				p->second->value = read_expr (l.rest (), pos);
				if (pos == std::string::npos)
					break;
				l.skip (pos);
			}
		}
		if (p != s->parts.end () || !l (s->post) || !l (" %"))
			continue;
		if (output)
			write_out (*s);
		addr += s->targets.size ();
		return;
	}
}

int main (int argc, char **argv)
{
	std::string defs;
	shevek::args::option opts[] = {
		shevek::args::option (0, "defs", "code definitions",
				false, defs),
	};
	shevek::args args (argc, argv, opts, 0, 0, "Generic assembler", "2008");
	bool need_header = false;
	if (defs.empty ())
	{
		defs = argv[0];
		need_header = true;
	}
	std::ifstream file (defs.c_str ());
	if (!file)
		shevek_error_errno ("unable to open file " << defs);
	std::string line;
	if (need_header)
	{
		std::getline (file, line);
		if (!file || line.size () < 2
				|| line[0] != '#' || line[1] != '!')
			shevek_error ("you must specify a definitions file");
	}
	read_input (file);
	if (errors)
		return 1;
	std::vector <std::pair <unsigned, std::string> > input;
	std::string pre;
	ln = 0;
	while (std::getline (std::cin, line))
	{
		++ln;
		if (!line.empty () && line[line.size () - 1] == '\\')
		{
			pre += line.substr (0, line.size () - 1);
			continue;
		}
		input.push_back (std::make_pair (ln, pre + line));
		pre.clear ();
	}
	if (!pre.empty ())
		input.push_back (std::make_pair (ln, pre));
	// Determine labels
	addr = 0;
	for (unsigned t = 0; t < input.size (); ++t)
	{
		ln = input[t].first;
		parse (input[t].second, false);
	}
	if (errors)
		return 1;
	// Write output
	addr = 0;
	for (unsigned t = 0; t < input.size (); ++t)
	{
		ln = input[t].first;
		parse (input[t].second, true);
	}
	if (errors)
		return 1;
	return 0;
}
