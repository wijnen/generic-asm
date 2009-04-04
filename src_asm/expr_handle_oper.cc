#include "asm.hh"

void Expr::handle_oper (std::stack <Oper *> &stack, Oper *oper)
{
	while (!stack.empty () && stack.top ()->priority >= oper->priority)
	{
		Expr::valid_int i;
		i.valid = false;
		i.value = 0;
		list.push_back (ExprElem (ExprElem::OPER, i, stack.top ()));
		stack.pop ();
	}
	stack.push (oper);
}
