#include "asm.hh"

static Expr::valid_int do_pop (std::stack <Expr::valid_int> &stack)
{
	Expr::valid_int ret = stack.top ();
	stack.pop ();
	return ret;
}

#define run1(name, oper) \
	static void name (std::stack <Expr::valid_int> &stack) \
	{ \
		Expr::valid_int a = do_pop (stack); \
		Expr::valid_int r; \
		r.valid = a.valid; \
		r.value = oper a.value; \
		stack.push (r); \
	}

#define run2(name, oper) \
	static void name (std::stack <Expr::valid_int> &stack) \
	{ \
		Expr::valid_int a = do_pop (stack); \
		Expr::valid_int b = do_pop (stack); \
		Expr::valid_int r; \
		r.valid = a.valid && b.valid; \
		r.value = b.value oper a.value; \
		stack.push (r); \
	}

run1 (run_not, !)
run1 (run_bitnot, ~)
run1 (run_minus1, -)

run2 (run_mul, *)
run2 (run_div, /)
run2 (run_mod, %)
run2 (run_plus, +)
run2 (run_minus, -)
run2 (run_lshift, <<)
run2 (run_rshift, >>)
run2 (run_lte, <=)
run2 (run_gte, >=)
run2 (run_lt, <)
run2 (run_gt, >)
run2 (run_eq, ==)
run2 (run_neq, !=)
run2 (run_and, &&)
run2 (run_or, ||)
run2 (run_bitand, &)
run2 (run_bitor, |)
run2 (run_bitxor, ^)

static void run_xor (std::stack <Expr::valid_int> &stack)
{
	Expr::valid_int a = do_pop (stack);
	Expr::valid_int b = do_pop (stack);
	Expr::valid_int r;
	r.valid = a.valid && b.valid;
	r.value = !b.value ^ !a.value;
	stack.push (r);
}

static void run_tri (std::stack <Expr::valid_int> &stack)
{
	Expr::valid_int a = do_pop (stack);
	Expr::valid_int b = do_pop (stack);
	Expr::valid_int c = do_pop (stack);
	Expr::valid_int r;
	r.valid = a.valid && b.valid;
	r.value = c.value ? b.value : a.value;
	stack.push (r);
}

Oper operators1[3] = {
	Oper ('!', "!", 12, &run_not),
	Oper ('~', "~", 12, &run_bitnot),
	Oper ('_', "-", 12, &run_minus1)
};
Oper operators2[19] = {
	Oper ('*', "*", 11, &run_mul),
	Oper ('/', "/", 11, &run_div),
	Oper ('%', "%", 11, &run_mod),
	Oper ('+', "+", 10, &run_plus),
	Oper ('-', "-", 10, &run_minus),
	Oper ('{', "<<", 9, &run_lshift),
	Oper ('}', ">>", 9, &run_rshift),
	Oper ('[', "<=", 8, &run_lte),
	Oper (']', ">=", 8, &run_gte),
	Oper ('<', "<", 8, &run_lt),
	Oper ('>', ">", 8, &run_gt),
	Oper ('=', "==", 7, &run_eq),
	Oper ('1', "!=", 7, &run_neq),
	Oper ('7', "&&", 3, &run_and),
	Oper ('6', "^^", 2, &run_xor),
	Oper ('\\', "||", 1, &run_or),
	Oper ('&', "&", 6, &run_bitand),
	Oper ('|', "|", 5, &run_bitor),
	Oper ('^', "^", 4, &run_bitxor)
};

Oper operators3[1] = {
	Oper (':', ":", 0, &run_tri)
};
