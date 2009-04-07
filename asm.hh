#ifndef ASM_HH
#define ASM_HH

#include <string>
#include <stack>
#include <map>
#include <list>
#include <vector>
#include <shevek/iostring.hh>
#include <shevek/error.hh>

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
	// This is a list because it must be output fifo in error messages.
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
	inline Glib::ustring print ();
private:
	void handle_oper (std::stack <Oper *> &stack, Oper *oper);
};

struct Oper
{
	char code;
	Glib::ustring name;
	int priority;
	void (*run) (std::stack <Expr::valid_int> &stack);
	void (*print) (std::stack <Glib::ustring> &stack);
	Oper (char c, Glib::ustring const &n, int p, void (*r)(std::stack <Expr::valid_int> &), void (*pr)(std::stack <Glib::ustring> &))
		: code (c), name (n), priority (p), run (r), print (pr) {}
};

struct Param
{
	Glib::ustring name;
	bool is_enum;
	std::map <Glib::ustring, Expr::valid_int> enum_values;
	std::list <Expr> constraints;

	bool is_active;
	Expr::valid_int value;

	static void reset ();
	static std::list <Param>::iterator find (Glib::ustring const &name);
};

extern std::list <Param> params;

struct ExprElem
{
	enum Type { NUM, OPER, PARAM, LABEL, ISLABEL } type;
	Expr::valid_int value;
	Oper *oper;
	Glib::ustring label;
	std::list <Param>::iterator param;
	ExprElem (Type t, Expr::valid_int v, Oper *o = NULL, Glib::ustring l = Glib::ustring (), std::list <Param>::iterator p = params.end ())
		: type (t), value (v), oper (o), label (l), param (p) {}
	void print (std::stack <Glib::ustring> &stack)
	{
		switch (type)
		{
		case NUM:
			if (!value.valid)
				shevek_error ("unexpected invalid number");
			stack.push (shevek::ostring ("%d", value.value));
			break;
		case OPER:
			oper->print (stack);
			break;
		case PARAM:
			if (param->value.valid)
				stack.push (shevek::ostring ("[%s(%d)]", param->name, param->value.value));
			else
				stack.push (shevek::ostring ("[%s]", param->name));
			break;
		case LABEL:
			stack.push (label);
			break;
		case ISLABEL:
			stack.push ('?' + label);
			break;
		default:
			shevek_error ("invalid case reached");
		}
	}
};

Glib::ustring Expr::print ()
{
	std::stack <Glib::ustring> ret;
	for (std::list <ExprElem>::iterator i = list.begin (); i != list.end (); ++i)
		i->print (ret);
	return ret.top ();
}

struct Label
{
	Glib::ustring name;
	input_line *definition;
	Expr::valid_int value;
};

struct Source
{
	std::list <std::pair <Glib::ustring, std::list <Param>::iterator> > parts;
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

struct File
{
	struct Block
	{
		struct Part
		{
			enum type_t { IF, ELSE, ENDIF, DEFINE, BYTE, CODE, COMMENT };
			type_t type;
			bool have_expr;
			Expr expr;
			std::string name;	// or code.
		};
		bool absolute;
		unsigned address;
		std::list <Part> parts;
		Block () : absolute (false), address (0) {}
		void write_binary ();
		void write_object (std::string &script, std::string &code);
	};
	std::list <Block> blocks;
	void write_binary () { for (std::list <Block>::iterator i = blocks.begin (); i != blocks.end (); ++i) i->write_binary (); }
	void write_object (std::string &script, std::string &code) { for (std::list <Block>::iterator i = blocks.begin (); i != blocks.end (); ++i) i->write_object (script, code); }
};
bool read_file (std::string const &filename);

struct Space
{
	unsigned start, size;
};

extern std::list <Label> labels;
extern std::list <Source> sources;
extern std::list <DefsMacro> defs_macros;
extern std::list <std::string> include_path;
extern std::list <File> files;
extern std::list <Space> spaces;

extern std::ostream *outfile, *listfile;
extern bool usehex;
extern Hex hexfile;
extern unsigned addr;
extern unsigned errors;
extern std::stack <Input> input_stack;
extern std::list <std::pair <unsigned, Glib::ustring> > *current_stack;
extern unsigned undefined_locals;
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
void write_expr (Expr &e);
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
