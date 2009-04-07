#include "asm.hh"
#include <shevek/debug.hh>

Expr Expr::read (Glib::ustring const &input, bool allow_params, Glib::ustring::size_type &pos)
{
	Expr ret;
	shevek::istring l (input.substr (pos));
	bool expect_number = true;
	std::stack <Oper *> opers;
	Oper open ('(', "(", -3, NULL, NULL), close (')', ")", -2, NULL, NULL);
	Oper tri_start ('?', "?", -1, NULL, NULL);
	int correction = 0;
	while (true)
	{
		l (" ");
		if (expect_number)
		{
			dbg (l.rest ());
			Glib::ustring word;
			// Is this a label existance check?
			if (l ("?%r/[A-Za-z_.][A-Za-z_.0-9]*/", word))
			{
				valid_int i;
				i.valid = false;
				i.value = 0;
				ret.list.push_back (ExprElem (ExprElem::ISLABEL, i, NULL, word));
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
			if (l ("%r/[A-Za-z_.][A-Za-z_.0-9]*/", word))
			{
				// Param
				if (allow_params)
				{
					std::list <Param>::iterator i;
					for (i = params.begin (); i != params.end (); ++i)
					{
						if (!i->is_active)
						{
							continue;
						}
						if (word != i->name)
						{
							continue;
						}
						valid_int vi;
						vi.value = 0;
						vi.valid = false;
						ret.list.push_back (ExprElem (ExprElem::PARAM, vi, NULL, Glib::ustring (), i));
						expect_number = false;
						break;
					}
					if (i != params.end ())
						continue;
				}
				// Label (may be defined later)
				valid_int i;
				i.value = 0;
				i.valid = false;
				ret.list.push_back (ExprElem (ExprElem::LABEL, i, NULL, word));
				expect_number = false;
				continue;
			}
			// The special number "$"
			if (l ("$"))
			{
				valid_int i;
				i.value = addr;
				i.valid = true;
				ret.list.push_back (ExprElem (ExprElem::NUM, i));
				expect_number = false;
				continue;
			}
			// Number
			int n;
			if (!l ("%i", n))
			{
				dbg ("Not a number");
				pos = Glib::ustring::npos;
				return Expr ();
			}
			valid_int vi;
			vi.value = n;
			vi.valid = true;
			ret.list.push_back (ExprElem (ExprElem::NUM, vi));
			expect_number = false;
		}
		else
		{
			dbg (l.rest ());
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
			if (l ("?"))
			{
				ret.handle_oper (opers, &tri_start);
				expect_number = true;
				continue;
			}
			if (l (escape (operators3[0].name)))
			{
				ret.handle_oper (opers, &operators3[0]);
				opers.pop ();	// The :
				if (opers.empty () || opers.top () != &tri_start)
				{
					dbg (": without ?");
					pos = Glib::ustring::npos;
					return Expr ();
				}
				opers.pop ();	// The ?
				opers.push (&operators3[0]);
				expect_number = true;
				continue;
			}
			if (l (")"))
			{
				ret.handle_oper (opers, &close);
				opers.pop ();	// The closing parentheses
				if (opers.empty () || opers.top () != &open)
				{
					dbg ("ending expression on closing parentheses");
					// End of expression.
					correction = 1;
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
			pos = Glib::ustring::npos;
			return Expr ();
		}
		if (opers.top () == &tri_start)
		{
			dbg ("unfinished ?: operator");
			pos = Glib::ustring::npos;
			return Expr ();
		}
		dbg ("pushing pending operator " << opers.top ()->name);
		valid_int i;
		i.valid = false;
		i.value = 0;
		ret.list.push_back (ExprElem (ExprElem::OPER, i, opers.top ()));
		opers.pop ();
	}
	pos = input.size () - l.rest ().size () - correction;
	return ret;
}
