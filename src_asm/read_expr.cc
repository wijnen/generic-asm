#include "asm.hh"

Expr::valid_int read_expr (std::string const &expr, bool allow_params, std::string::size_type &pos)
{
	Expr e = Expr::read (expr, allow_params, pos);
	if (pos == std::string::npos)
		return Expr::valid_int ("??");
	return e.compute (Expr::valid_int ("<"));
}

Expr::valid_int read_expr (std::string const &expr, std::string const &comment)
{
	std::string::size_type pos = 0;
	Expr::valid_int ret = read_expr (expr, true, pos);
	if (pos == std::string::npos)
		error (shevek::rostring ("incorrect expression: %s", expr));
	else
	{
		shevek::ristring s (expr.substr (pos));
		s (" ");
		if (!s.rest ().empty () && !s (comment))
			error (shevek::rostring ("junk at end of value: %s", s.rest ()));
	}
	return ret;
}
