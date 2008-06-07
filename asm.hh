#ifndef OPERATORS_HH
#define OPERATORS_HH

#include <string>
#include <stack>
#include <shevek/iostring.hh>
#include <map>
#include <list>

struct Oper
{
	char code;
	std::string name;
	int priority;
	void (*run) (std::stack <int> &stack);
	Oper (char c, std::string const &n, int p,
			void (*r)(std::stack <int> &))
		: code (c), name (n), priority (p), run (r) {}
};

struct Expr;

struct Param
{
	bool is_enum;
	std::map <std::string, unsigned> enum_values;
	std::list <Expr> constraints;

	bool is_active;
	int value;

	static void reset ();
	static std::map <std::string, Param>::reverse_iterator find (std::string const &name);
};

extern std::map <std::string, Param> params;

struct ExprElem
{
	enum Type { NUM, OPER, PARAM, LABEL } type;
	int value;
	Oper *oper;
	std::string label;
	std::map <std::string, Param>::reverse_iterator param;
	ExprElem (Type t, int v, Oper *o = NULL,
	std::string l = std::string (), std::map <std::string, Param>::reverse_iterator p = params.rend ())
		: type (t), value (v), oper (o), label (l), param (p) {}
};

struct Expr
{
	std::list <ExprElem> list;
	int compute (bool *valid);
	static Expr read (std::string const &input, bool allow_params,
			std::string::size_type &pos);
private:
	void handle_oper (std::stack <Oper *> &stack, Oper *oper);
};

struct Label
{
	std::string name;
	int value;
	bool valid;
};

struct Source
{
	std::list <std::pair <std::string, std::map <std::string, Param>::reverse_iterator> > parts;
	std::string post;
	std::list <std::string> targets;
};

void error (std::string const &message);
std::string escape (std::string const &in);
Label *find_label (std::string name);
void read_input (std::istream &file);

extern std::list <Label> labels;
extern std::list <Source> sources;

extern unsigned ln;
extern unsigned addr;
extern unsigned errors;
extern std::string source;

extern Oper operators1[3];
extern Oper operators2[19];
extern Oper operators3[1];

#endif
