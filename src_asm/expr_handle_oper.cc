#include "asm.hh"

void Expr::handle_oper (std::stack <Oper *> &stack, Oper *oper)
{
	while (!stack.empty () && stack.top ()->priority >= oper->priority)
	{
		list.push_back (ExprElem (ExprElem::OPER, Expr::valid_int ("+"), stack.top ()));
		stack.pop ();
	}
	stack.push (oper);
}
