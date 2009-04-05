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
	std::string basedir;
	unsigned ln;
	enum { FILE, MACRO } type;
	std::list <DefsMacro>::iterator macro;
	bool must_delete;
	std::istream *file;
};

struct input_line
{
	// This is a list because it must be fifo.
	std::list <std::pair <unsigned, Glib::ustring> > stack;
	Glib::ustring data;
	input_line (Glib::ustring d);
};

struct Oper;
struct ExprElem;
struct Expr
{
	struct valid_int { bool valid; int value; };
	std::list <ExprElem> list;
	valid_int compute ();
	static Expr read (Glib::ustring const &input, bool allow_params, Glib::ustring::size_type &pos);
private:
	void handle_oper (std::stack <Oper *> &stack, Oper *oper);
};

struct Oper
{
	char code;
	Glib::ustring name;
	int priority;
	void (*run) (std::stack <Expr::valid_int> &stack);
	Oper (char c, Glib::ustring const &n, int p, void (*r)(std::stack <Expr::valid_int> &))
		: code (c), name (n), priority (p), run (r) {}
};

struct Param
{
	bool is_enum;
	std::map <Glib::ustring, Expr::valid_int> enum_values;
	std::list <Expr> constraints;

	bool is_active;
	Expr::valid_int value;

	static void reset ();
	static std::map <Glib::ustring, Param>::reverse_iterator find
		(Glib::ustring const &name);
};

extern std::map <Glib::ustring, Param> params;

struct ExprElem
{
	enum Type { NUM, OPER, PARAM, LABEL, ISLABEL } type;
	Expr::valid_int value;
	Oper *oper;
	Glib::ustring label;
	std::map <Glib::ustring, Param>::reverse_iterator param;
	ExprElem (Type t, Expr::valid_int v, Oper *o = NULL, Glib::ustring l = Glib::ustring (), std::map <Glib::ustring, Param>::reverse_iterator p = params.rend ())
		: type (t), value (v), oper (o), label (l), param (p) {}
};

struct Label
{
	Glib::ustring name;
	input_line *definition;
	Expr::valid_int value;
};

struct Source
{
	std::list <std::pair <Glib::ustring, std::map <Glib::ustring, Param>
				::reverse_iterator> > parts;
	Glib::ustring post;
	std::list <Glib::ustring> targets;
};

struct DefsMacro
{
	Glib::ustring name;
	std::vector <std::pair <Glib::ustring, Glib::ustring> > args;
	std::vector <Glib::ustring> code;
};

struct Directive
{
	Glib::ustring name;
	std::list <Glib::ustring> nick;
	void (*function) (shevek::istring &args, bool first, Label *current_label);
	Directive (Glib::ustring const &n, void (*f)(shevek::istring &, bool, Label *))
		: name (n), function (f) {}
};

class Hex
{
public:
	void open (std::istream &file);
	void write_hex (std::ostream &file);
	void write_s19 (std::ostream &file);
	int get (unsigned address) const;
	void set (unsigned address, int value);
	unsigned size () const { return data.size (); }
private:
	std::vector <int> data;
};

extern std::list <Label> labels;
extern std::list <Source> sources;
extern std::list <DefsMacro> defs_macros;
extern std::list <std::string> include_path;

extern std::ostream *outfile, *listfile;
extern bool usehex;
extern Hex hexfile;
extern unsigned addr;
extern unsigned errors;
extern std::stack <Input> input_stack;
extern std::list <std::pair <unsigned, Glib::ustring> > *current_stack;
extern unsigned undefined_labels;
extern bool writing;

extern Oper operators1[3];
extern Oper operators2[19];
extern Oper operators3[1];

template <typename T, unsigned n> unsigned num_elem (T (&arr)[n]) { (void)arr; return n; }

void error (Glib::ustring const &message);
Glib::ustring escape (Glib::ustring const &in);
Label *find_label (Glib::ustring name);
Expr::valid_int read_expr (Glib::ustring const &expr, bool allow_params, Glib::ustring::size_type &pos);
Expr::valid_int read_expr (Glib::ustring const &expr, Glib::ustring const &comment);
Glib::ustring subst_args (Glib::ustring const &orig, std::vector <std::pair <Glib::ustring, Glib::ustring> > const &args);
bool getline (Glib::ustring &ret);
void read_definitions ();
std::string read_filename (shevek::istring &args);
void write_out (Source const &s);
void write_byte (Expr::valid_int byte, int addr_offset);
void parse (input_line &input, bool first_pass, bool report);
int main (int argc, char **argv);

void dir_org (shevek::istring &args, bool first, Label *current_label);
void dir_defb (shevek::istring &args, bool first, Label *current_label);
void dir_comment (shevek::istring &args, bool first, Label *current_label);
void dir_equ (shevek::istring &args, bool first, Label *current_label);
void dir_include (shevek::istring &args, bool first, Label *current_label);
void dir_incbin (shevek::istring &args, bool first, Label *current_label);
void dir_seek (shevek::istring &args, bool first, Label *current_label);
void dir_macro (shevek::istring &args, bool first, Label *current_label);
void dir_endmacro (shevek::istring &args, bool first, Label *current_label);
void dir_if (shevek::istring &args, bool first, Label *current_label);
void dir_else (shevek::istring &args, bool first, Label *current_label);
void dir_endif (shevek::istring &args, bool first, Label *current_label);
void dir_error (shevek::istring &args, bool first, Label *current_label);

extern Oper operators1[3];
extern Oper operators2[19];
extern Oper operators3[1];
extern Directive directives[13];

#endif
