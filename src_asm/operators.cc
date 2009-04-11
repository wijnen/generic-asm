#include "asm.hh"

template <typename _T> static _T do_pop (std::stack <_T> &stack)
{
	_T ret = stack.top ();
	stack.pop ();
	return ret;
}

#define run1(name, oper) \
	static Expr::valid_int run_##name (std::list <Expr::valid_int> &children) \
	{ \
		Expr::valid_int a = children.front (); \
		Expr::valid_int r ("!" #name); \
		r.valid = a.valid; \
		r.invalid = a.invalid; \
		r.value = oper a.value; \
		return r; \
	} \
	static std::string print_##name (std::list <Expr> &children) \
	{ \
		return #oper + children.front ().print (); \
	}

#define run2(name, oper) \
	static Expr::valid_int run_##name (std::list <Expr::valid_int> &children) \
	{ \
		Expr::valid_int a = children.front (); \
		Expr::valid_int b = children.back (); \
		Expr::valid_int r ("!" #name); \
		r.valid = b.valid && a.valid; \
		r.value = b.value oper a.value; \
		r.invalid = a.invalid; \
		for (std::list <std::string>::iterator i = b.invalid.begin (); i != b.invalid.end (); ++i) \
			r.invalid.push_back (*i); \
		return r; \
	} \
	static std::string print_##name (std::list <Expr> &children) \
	{ \
		return '(' + children.front ().print () + #oper + children.back ().print () + ')'; \
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

static Expr::valid_int run_xor (std::list <Expr::valid_int> &children)
{
	Expr::valid_int a = children.front ();
	Expr::valid_int b = children.back ();
	Expr::valid_int r ("!xor");
	r.valid = a.valid && b.valid;
	r.invalid = a.invalid;
	for (std::list <std::string>::iterator i = b.invalid.begin (); i != b.invalid.end (); ++i)
		r.invalid.push_back (*i);
	r.value = !b.value ^ !a.value;
	return r;
}
static std::string print_xor (std::list <Expr> &children)
{
	return '(' + children.front ().print () + "^^" + children.back ().print () + ')';
}

static Expr::valid_int run_tri (std::list <Expr::valid_int> &children)
{
	Expr::valid_int a = children.front ();
	Expr::valid_int b = *++children.begin ();
	Expr::valid_int c = children.back ();
	Expr::valid_int r ("!tri");
	r.valid = a.valid && (a.value ? b.valid : c.valid);
	if (!a.valid)
		r.invalid = a.invalid;
	else if (a.value)
		r.invalid = b.invalid;
	else
		r.invalid = c.invalid;
	r.value = a.value ? b.value : c.value;
	return r;
}
static std::string print_tri (std::list <Expr> &children)
{
	return '(' + children.front ().print () + "?" + (++children.begin ())->print () + ":" + children.back ().print () + ')';
}

Oper operators1[3] = {
	Oper (1, '!', "!", 12, &run_not, &print_not),
	Oper (1, '~', "~", 12, &run_bitnot, &print_bitnot),
	Oper (1, '_', "-", 12, &run_minus1, &print_minus1)
};
Oper operators2[19] = {
	Oper (2, '+', "+", 10, &run_plus, &print_plus),
	Oper (2, '*', "*", 11, &run_mul, &print_mul),
	Oper (2, '/', "/", 11, &run_div, &print_div),
	Oper (2, '%', "%", 11, &run_mod, &print_mod),
	Oper (2, '-', "-", 10, &run_minus, &print_minus),
	Oper (2, '{', "<<", 9, &run_lshift, &print_lshift),
	Oper (2, '}', ">>", 9, &run_rshift, &print_rshift),
	Oper (2, '[', "<=", 8, &run_lte, &print_lte),
	Oper (2, ']', ">=", 8, &run_gte, &print_gte),
	Oper (2, '<', "<", 8, &run_lt, &print_lt),
	Oper (2, '>', ">", 8, &run_gt, &print_gt),
	Oper (2, '=', "==", 7, &run_eq, &print_eq),
	Oper (2, '1', "!=", 7, &run_neq, &print_neq),
	Oper (2, '7', "&&", 3, &run_and, &print_and),
	Oper (2, '6', "^^", 2, &run_xor, &print_xor),
	Oper (2, '\\', "||", 1, &run_or, &print_or),
	Oper (2, '&', "&", 6, &run_bitand, &print_bitand),
	Oper (2, '|', "|", 5, &run_bitor, &print_bitor),
	Oper (2, '^', "^", 4, &run_bitxor, &print_bitxor)
};
Oper *plus_oper = &operators2[0];

Oper operators3[1] = {
	Oper (3, ':', ":", 0, &run_tri, &print_tri)
};
