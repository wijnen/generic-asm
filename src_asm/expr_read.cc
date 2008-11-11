#include "asm.hh"
#include <shevek/debug.hh>

Expr Expr::read (Glib::ustring const &input, bool allow_params,
		Glib::ustring::size_type &pos)
{
	Expr ret;
	shevek::istring l (input.substr (pos));
	bool expect_number = true;
	std::stack <Oper *> opers;
	Oper open ('(', "(", -3, NULL), close (')', ")", -2, NULL);
	Oper tri_start ('?', "?", -1, NULL);
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
			Glib::ustring word;
			if (l ("%r/[A-Za-z_.][A-Za-z_.0-9]*/", word))
			{
				// Param
				if (allow_params)
				{
					std::map <Glib::ustring, Param>::reverse_iterator i;
					for (i = params.rbegin ();
							i != params.rend ();
							++i)
					{
						if (!i->second.is_active)
							continue;
						if (word != i->first)
							continue;
						ret.list.push_back (ExprElem (ExprElem::PARAM, 0, NULL, Glib::ustring (), i));
						expect_number = false;
						break;
					}
					if (i != params.rend ())
						continue;
				}
				// Label (may be defined later)
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
				pos = Glib::ustring::npos;
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
				opers.pop ();	// The :
				if (opers.empty ()
						|| opers.top () != &tri_start)
				{
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
			pos = Glib::ustring::npos;
			return Expr ();
		}
		dbg ("pushing pending operator " << opers.top ()->name);
		ret.list.push_back (ExprElem (ExprElem::OPER, 0, opers.top ()));
		opers.pop ();
	}
	pos = input.size () - l.rest ().size () - correction;
	return ret;
}
