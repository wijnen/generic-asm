#ifndef ASM_HH
#define ASM_HH

#include <string>
#include <stack>
#include <shevek/iostring.hh>
#include <map>
#include <list>
#include <vector>

struct DefsMacro;

struct Input
{
	std::string name;
	unsigned ln;
	enum { FILE, MACRO } type;
	std::list <DefsMacro>::iterator macro;
	bool must_delete;
	std::istream *file;
};

struct input_line
{
	// This is a list because it must be fifo.
	std::list <std::pair <unsigned, std::string> > stack;
	std::string data;
	input_line (std::string d);
};

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
	static std::map <std::string, Param>::reverse_iterator find
		(std::string const &name);
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
			std::string l = std::string (),
			std::map <std::string, Param>::reverse_iterator
			p = params.rend ())
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
	std::list <std::pair <std::string, std::map <std::string, Param>
				::reverse_iterator> > parts;
	std::string post;
	std::list <std::string> targets;
};

struct DefsMacro
{
	std::string name;
	std::vector <std::pair <std::string, std::string> > args;
	std::vector <std::string> code;
};

struct Directive
{
	std::string name;
	std::list <std::string> nick;
	unsigned (*function) (shevek::istring &args, bool write,
			Label *current_label);
	Directive (std::string const &n,
			unsigned (*f)(shevek::istring &, bool, Label *))
		: name (n), function (f) {}
};

extern std::list <Label> labels;
extern std::list <Source> sources;
extern std::list <DefsMacro> defs_macros;

extern unsigned addr;
extern unsigned errors;
extern std::stack <Input> input_stack;
extern std::list <std::pair <unsigned, std::string> > *current_stack;

extern Oper operators1[3];
extern Oper operators2[19];
extern Oper operators3[1];

template <typename T, unsigned n> unsigned num_elem (T (&arr)[n]) { return n; }

void error (std::string const &message);
std::string escape (std::string const &in);
Label *find_label (std::string name);
int read_expr (std::string const &expr, bool allow_params,
		std::string::size_type &pos, bool *valid);
int read_expr (std::string const &expr, std::string const &comment);
std::string subst_args (std::string const &orig, std::vector
		<std::pair <std::string, std::string> > const &args);
bool getline (std::string &ret);
void read_definitions ();
void write_out (Source const &s);
void write_byte (int byte);
unsigned parse (input_line &input, bool output, bool first_pass, bool report);
int main (int argc, char **argv);

unsigned dir_org (shevek::istring &args, bool write, Label *current_label);
unsigned dir_defb (shevek::istring &args, bool write, Label *current_label);
unsigned dir_comment (shevek::istring &args, bool write, Label *current_label);
unsigned dir_equ (shevek::istring &args, bool write, Label *current_label);
unsigned dir_include (shevek::istring &args, bool write, Label *current_label);
unsigned dir_incbin (shevek::istring &args, bool write, Label *current_label);
unsigned dir_seek (shevek::istring &args, bool write, Label *current_label);
unsigned dir_macro (shevek::istring &args, bool write, Label *current_label);
unsigned dir_endmacro (shevek::istring &args, bool write, Label *current_label);
unsigned dir_if (shevek::istring &args, bool write, Label *current_label);
unsigned dir_else (shevek::istring &args, bool write, Label *current_label);
unsigned dir_endif (shevek::istring &args, bool write, Label *current_label);

extern Oper operators1[3];
extern Oper operators2[19];
extern Oper operators3[1];
extern Directive directives[13];

#endif
