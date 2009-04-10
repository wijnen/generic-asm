#include "asm.hh"

template <typename _T> static _T do_pop (std::stack <_T> &stack)
{
	_T ret = stack.top ();
	stack.pop ();
	return ret;
}

#define run1(name, oper) \
	static void run_##name (std::stack <Expr::valid_int> &stack) \
	{ \
		Expr::valid_int a = do_pop (stack); \
		Expr::valid_int r ("!" #name); \
		r.valid = a.valid; \
		r.invalid = a.invalid; \
		r.value = oper a.value; \
		stack.push (r); \
	} \
	static void print_##name (std::stack <std::string> &stack) \
	{ \
		std::string a = do_pop (stack); \
		stack.push (#oper + a); \
	}

#define run2(name, oper) \
	static void run_##name (std::stack <Expr::valid_int> &stack) \
	{ \
		Expr::valid_int a = do_pop (stack); \
		Expr::valid_int b = do_pop (stack); \
		Expr::valid_int r ("!" #name); \
		r.valid = b.valid && a.valid; \
		r.value = b.value oper a.value; \
		r.invalid = a.invalid; \
		for (std::list <std::string>::iterator i = b.invalid.begin (); i != b.invalid.end (); ++i) \
			r.invalid.push_back (*i); \
		stack.push (r); \
	} \
	static void print_##name (std::stack <std::string> &stack) \
	{ \
		std::string a = do_pop (stack); \
		std::string b = do_pop (stack); \
		stack.push ('(' + b + #oper + a + ')'); \
	}

run1 (not, !)
run1 (bitnot, ~)
run1 (minus1, -)

run2 (mul, *)
run2 (div, /)
run2 (mod, %)
run2 (plus, +)
run2 (minus, -)
run2 (lshift, <<)
run2 (rshift, >>)
run2 (lte, <=)
run2 (gte, >=)
run2 (lt, <)
run2 (gt, >)
run2 (eq, ==)
run2 (neq, !=)
run2 (and, &&)
run2 (or, ||)
run2 (bitand, &)
run2 (bitor, |)
run2 (bitxor, ^)

static void run_xor (std::stack <Expr::valid_int> &stack)
{
	Expr::valid_int a = do_pop (stack);
	Expr::valid_int b = do_pop (stack);
	Expr::valid_int r ("!xor");
	r.valid = b.valid && a.valid;
	r.invalid = a.invalid;
	for (std::list <std::string>::iterator i = b.invalid.begin (); i != b.invalid.end (); ++i)
		r.invalid.push_back (*i);
	r.value = !b.value ^ !a.value;
	stack.push (r);
}
static void print_xor (std::stack <std::string> &stack)
{
	std::string a = do_pop (stack);
	std::string b = do_pop (stack);
	stack.push ('(' + b + "^^" + a + ')');
}

static void run_tri (std::stack <Expr::valid_int> &stack)
{
	Expr::valid_int a = do_pop (stack);
	Expr::valid_int b = do_pop (stack);
	Expr::valid_int c = do_pop (stack);
	Expr::valid_int r ("!tri");
	r.valid = c.valid && (c.value ? b.valid : a.valid);
	if (!c.valid)
		r.invalid = c.invalid;
	else if (!c.value)
		r.invalid = a.invalid;
	else
		r.invalid = b.invalid;
	r.value = c.value ? b.value : a.value;
	stack.push (r);
}
static void print_tri (std::stack <std::string> &stack)
{
	std::string a = do_pop (stack);
	std::string b = do_pop (stack);
	std::string c = do_pop (stack);
	stack.push ('(' + c + "?" + b + ":" + a + ')');
}

Oper operators1[3] = {
	Oper ('!', "!", 12, &run_not, &print_not),
	Oper ('~', "~", 12, &run_bitnot, &print_bitnot),
	Oper ('_', "-", 12, &run_minus1, &print_minus1)
};
Oper operators2[19] = {
	Oper ('+', "+", 10, &run_plus, &print_plus),
	Oper ('*', "*", 11, &run_mul, &print_mul),
	Oper ('/', "/", 11, &run_div, &print_div),
	Oper ('%', "%", 11, &run_mod, &print_mod),
	Oper ('-', "-", 10, &run_minus, &print_minus),
	Oper ('{', "<<", 9, &run_lshift, &print_lshift),
	Oper ('}', ">>", 9, &run_rshift, &print_rshift),
	Oper ('[', "<=", 8, &run_lte, &print_lte),
	Oper (']', ">=", 8, &run_gte, &print_gte),
	Oper ('<', "<", 8, &run_lt, &print_lt),
	Oper ('>', ">", 8, &run_gt, &print_gt),
	Oper ('=', "==", 7, &run_eq, &print_eq),
	Oper ('1', "!=", 7, &run_neq, &print_neq),
	Oper ('7', "&&", 3, &run_and, &print_and),
	Oper ('6', "^^", 2, &run_xor, &print_xor),
	Oper ('\\', "||", 1, &run_or, &print_or),
	Oper ('&', "&", 6, &run_bitand, &print_bitand),
	Oper ('|', "|", 5, &run_bitor, &print_bitor),
	Oper ('^', "^", 4, &run_bitxor, &print_bitxor)
};
Oper *plus_oper = &operators2[0];

Oper operators3[1] = {
	Oper (':', ":", 0, &run_tri, &print_tri)
};
