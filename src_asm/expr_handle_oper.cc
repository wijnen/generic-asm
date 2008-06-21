#include "asm.hh"

void Expr::handle_oper (std::stack <Oper *> &stack, Oper *oper)
{
	while (!stack.empty () && stack.top ()->priority >= oper->priority)
	{
		list.push_back (ExprElem (ExprElem::OPER, 0, stack.top ()));
		stack.pop ();
	}
	stack.push (oper);
}