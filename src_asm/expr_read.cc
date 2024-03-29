#include "asm.hh"

static void oper_push (std::stack <Expr> &result, std::stack <Oper *> &opers)
{
	Expr o (Expr::OPER, Expr::valid_int (std::string (1, '*') + std::string (1, opers.top ()->code)), opers.top ());
	for (unsigned c = 0; c < opers.top ()->num; ++c)
	{
		o.children.push_front (result.top ());
		result.pop ();
	}
	result.push (o);
	opers.pop ();
}

static void handle_oper (std::stack <Expr> &result, std::stack <Oper *> &opers, Oper *o)
{
	while (!opers.empty () && opers.top ()->priority >= o->priority)
		oper_push (result, opers);
	opers.push (o);
}

Expr Expr::read (std::string const &input, bool allow_params, std::string::size_type &pos)
{
	shevek::ristring l (input.substr (pos));
	bool expect_number = true;
	std::stack <Oper *> opers;
	std::stack <Expr> result;
	Oper open (0, '(', "(", -3, NULL, NULL), close (0, ')', ")", -2, NULL, NULL);
	Oper tri_start (0, '?', "?", -1, NULL, NULL);
	dbg ("reading expression " + l.rest ());
	while (true)
	{
		l (" ");
		if (expect_number)
		{
			std::string word;
			// Is this a label existance check?
			if (l ("?%r/[A-Za-z_.][A-Za-z_.0-9]*/", word))
			{
				result.push (Expr (ISLABEL, valid_int ("?"), NULL, std::list <Expr> (), word));
				expect_number = false;
				continue;
			}
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
			// Bit-constructor
			if (l ("{"))
			{
				dbg ("bit construct");
				// Protect the list from operators before it.
				opers.push (&open);
				std::list <Expr> list;
				bool need_comma = false;
				if (l (" }"))
				{
					result.push (Expr (Expr::NUM, 0));
				}
				else
				{
					while (true)
					{
						dbg (l.rest ());
						if (need_comma)
						{
							if (!l (" , "))
								break;
						}
						need_comma = true;
						std::string::size_type p = input.size () - l.rest ().size ();
						std::string::size_type n = p;
						list.push_back (Expr::read (input, allow_params, n));
						l.skip (n - p);
						dbg (n << ',' << p);
					}
					if (!l ("}"))
					{
						error ("bit-constructor isn't closed");
						pos = std::string::npos;
						return Expr ();
					}
					result.push (Expr (Expr::NUM, 1));
					handle_oper (result, opers, lshift_oper);
					result.push (list.front ());
					list.pop_front ();
					for (std::list <Expr>::iterator i = list.begin (); i != list.end (); ++i)
					{
						handle_oper (result, opers, or_oper);
						result.push (Expr (Expr::NUM, 1));
						handle_oper (result, opers, lshift_oper);
						result.push (*i);
					}
				}
				handle_oper (result, opers, &close);
				opers.pop ();
				if (opers.empty () || opers.top () != &open)
				{
					error ("bit construction ends before closing brace");
					pos = std::string::npos;
					return Expr ();
				}
				opers.pop ();
				expect_number = false;
				dbg ("done");
				continue;
			}
			// Param
			if (l ("#"))
			{
				result.push (Expr (PARAM, valid_int ("#")));
				expect_number = false;
				continue;
			}
			if (l ("["))
			{
				std::string r = l.rest ();
				std::string::size_type p = 0;
				std::list <Expr> lst;
				int last = 0, beforelast = 0;
				bool valid = false;
				while (true)
				{
					Expr e = Expr::read (r, false, p);
					if (p == std::string::npos || p >= r.size ())
					{
						pos = std::string::npos;
						return Expr ();
					}
					beforelast = last;
					if (e.type == NUM && e.value.valid)
					{
						last = e.value.value;
						valid = true;
					}
					else
					{
						last = 0;
						valid = false;
					}
					lst.push_back (e);
					if (r[p] != ';')
						break;
					++p;
				}
				l.skip (p);
				if (!l ("]"))
				{
					dbg ("missing ']' after param");
					pos = std::string::npos;
					return Expr ();
				}
				if (lst.size () < 2 || beforelast == 0 || !valid)
				{
					error ("invalid param in expression");
				}
				lst.pop_back ();
				lst.pop_back ();
				result.push (Expr (PARAM, valid_int ("#!#")));
				result.top ().children.push_back (Expr (NUM, valid_int (last)));
				result.top ().children.push_back (Expr (NUM, valid_int (beforelast)));
				result.top ().constraints = lst;
				expect_number = false;
				continue;
			}
			if (l ("%r/[A-Za-z_.@][A-Za-z_.@0-9]*/", word))
			{
				// Param
				if (allow_params)
				{
					std::list <Param>::iterator i;
					for (i = params.begin (); i != params.end (); ++i)
					{
						if (!i->is_active)
							continue;
						if (word != i->name)
							continue;
						if (stage == 0)
						{
							result.push (Expr (PARAM, valid_int ("#>")));
							result.top ().param = &*i;
						}
						else
						{
							result.push (Expr (PARAM, valid_int ("##")));
							result.top ().constraints = i->constraints;
							result.top ().param = &*i;
							result.top ().children.push_back (i->value);
							result.top ().children.push_back (Expr (Expr::NUM, Expr::valid_int (i->mask)));
						}
						expect_number = false;
						break;
					}
					if (i != params.end ())
						continue;
				}
				// Label (may be defined later)
				result.push (Expr (LABEL, valid_int ('!' + word), NULL, std::list <Expr> (), word));
				expect_number = false;
				continue;
			}
			// The special number "$"
			if (l ("$"))
			{
				if (absolute_addr)
					result.push (Expr (NUM, valid_int (addr)));
				else
				{
					std::list <Expr> c;
					c.push_back (Expr (NUM, valid_int (addr)));
					c.push_back (Expr (LABEL, valid_int ("!$"), NULL, std::list <Expr> (), "$$"));
					result.push (Expr (OPER, valid_int ("$+"), plus_oper, c));
				}
				expect_number = false;
				continue;
			}
			// Number
			int n;
			if (!l ("%i", n))
			{
				dbg ("Not a number");
				pos = std::string::npos;
				return Expr ();
			}
			result.push (Expr (NUM, valid_int (n)));
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
					handle_oper (result, opers, &operators2[i]);
					expect_number = true;
					break;
				}
			}
			if (i != num_elem (operators2))
				continue;
			if (l ("?"))
			{
				handle_oper (result, opers, &tri_start);
				expect_number = true;
				continue;
			}
			if (l (escape (operators3[0].name)))
			{
				handle_oper (result, opers, &operators3[0]);
				opers.pop ();	// The :
				if (opers.empty () || opers.top () != &tri_start)
				{
					dbg (": without ?");
					pos = std::string::npos;
					return Expr ();
				}
				opers.pop ();	// The ?
				opers.push (&operators3[0]);
				expect_number = true;
				continue;
			}
			if (l (")"))
			{
				handle_oper (result, opers, &close);
				opers.pop ();	// The closing parentheses
				if (opers.empty () || opers.top () != &open)
				{
					dbg ("ending expression on closing parentheses");
					// End of expression.
					l.skip (-1);
					break;
				}
				opers.pop ();	// The opening parentheses
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
			dbg ("too many open parentheses");
			pos = std::string::npos;
			return Expr ();
		}
		if (opers.top () == &tri_start)
		{
			dbg ("unfinished ?: operator");
			pos = std::string::npos;
			return Expr ();
		}
		oper_push (result, opers);
	}
	pos = input.size () - l.rest ().size ();
	if (result.size () != 1)
		shevek_error ("not one value returned by expression");
	result.top ().simplify ();
	return result.top ();
}
