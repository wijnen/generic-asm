#include "asm.hh"

int read_expr (Glib::ustring const &expr, bool allow_params,
		Glib::ustring::size_type &pos, bool *valid)
{
	Expr e = Expr::read (expr, allow_params, pos);
	if (pos != Glib::ustring::npos)
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

int read_expr (Glib::ustring const &expr, Glib::ustring const &comment)
{
	Glib::ustring::size_type pos = 0;
	int ret = read_expr (expr, true, pos, NULL);
	if (pos == Glib::ustring::npos)
		error (shevek::ostring ("invalid expression: %s", expr));
	else
	{
		shevek::istring s (expr.substr (pos));
		s (" ");
		if (!s.rest ().empty () && !s (comment))
			error (shevek::ostring ("junk at end of value: %s", s.rest ()));
	}
	return ret;
}
