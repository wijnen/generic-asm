#include "asm.hh"

int read_expr (std::string const &expr, bool allow_params,
		std::string::size_type &pos, bool *valid)
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
