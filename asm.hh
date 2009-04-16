#ifndef ASM_HH
#define ASM_HH

#include <string>
#include <stack>
#include <map>
#include <list>
#include <vector>
#include <iostream>
#include <shevek/iostring.hh>
#include <shevek/error.hh>
#include <shevek/debug.hh>

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
	std::list <std::pair <unsigned, std::string> > stack;
	std::string data;
	input_line (std::string d);
};

struct Oper;
struct Param;
struct Expr
{
	struct valid_int
	{
		bool valid;
		std::list <std::string> invalid;	// invalid labels which are used in this value.
		int value;
		valid_int (std::string const &label) : valid (false), value (0) { invalid.push_back (label); }
		valid_int (int v) : valid (true), value (v) {}
	};

	enum Type { NUM, PARAM, LABEL, ISLABEL, OPER } type;
	valid_int value;
	Oper *oper;
	std::list <Expr> children; // For params, the last is the value, the rest are constraints.
	std::string label;
	Param *param;
	Expr (Type t = NUM, valid_int v = valid_int ("{}"), Oper *o = NULL, std::list <Expr> const &c = std::list <Expr> (), std::string const &l = std::string (), Param *p = NULL)
		: type (t), value (v), oper (o), children (c), label (l), param (p) {}

	valid_int compute (valid_int self) const;
	static Expr read (std::string const &input, bool allow_params, std::string::size_type &pos);
	std::string print () const;
	void simplify (bool set_addr = false);
	void check (bool file);
	void clean_islabel ();
};

struct Oper
{
	unsigned num;	// number of children.
	char code;
	std::string name;
	int priority;
	Expr::valid_int (*run) (std::list <Expr::valid_int> &children);
	std::string (*print) (std::list <Expr> const &children);
	Oper (unsigned N, char c, std::string const &n, int p, Expr::valid_int (*r)(std::list <Expr::valid_int> &), std::string (*pr)(std::list <Expr> const &))
		: num (N), code (c), name (n), priority (p), run (r), print (pr) {}
};

struct Param
{
	std::string name;
	bool is_enum;
	std::map <std::string, Expr::valid_int> enum_values;
	std::list <Expr> constraints;

	bool is_active;
	Expr value;
	int mask, dismask;

	static void reset ();
	static std::list <Param>::iterator find (std::string const &name);
};

extern std::list <Param> params;

struct Label
{
	std::string name;
	input_line *definition;
	Expr value;
};

struct Source
{
	std::list <std::pair <std::string, std::list <Param>::iterator> > parts;
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
	void (*function) (shevek::ristring &args, bool first, std::list <Label>::iterator current_label);
	Directive (std::string const &n, void (*f)(shevek::ristring &, bool, std::list <Label>::iterator))
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

struct Block
{
	struct Part
	{
		enum type_t { IF, ELSE, ENDIF, DEFINE, BYTE, CODE, COMMENT };
		type_t type;
		bool have_expr;
		std::list <Label>::iterator label;
		Expr::valid_int value;
		Expr expr;
		std::string name;	// or code.
		Part () : value ("&") {}
	};
	bool absolute;
	unsigned address;
	std::list <Part> parts;
	Block () : absolute (false), address (0) {}
	void lock ();
	void write_binary ();
	void write_object (std::string &script, std::string &code);
	void clean (bool file);
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
extern std::list <Block> blocks;
extern std::list <Space> spaces;

extern std::ostream *outfile, *listfile;
extern bool usehex;
extern Hex hexfile;
extern unsigned addr;
extern bool absolute_addr;
extern unsigned errors;
extern std::stack <Input> input_stack;
extern std::list <std::pair <unsigned, std::string> > *current_stack;
extern unsigned undefined_locals;
extern bool writing;
extern unsigned stage;
extern bool report_labels;

extern Oper operators1[3];
extern Oper operators2[19];
extern Oper operators3[1];
extern Oper *plus_oper;

template <typename T, unsigned n> unsigned num_elem (T (&arr)[n]) { (void)arr; return n; }

void error (std::string const &message);
std::string escape (std::string const &in);
std::list <Label>::iterator find_label (std::string name);
Expr::valid_int read_expr (std::string const &expr, bool allow_params, std::string::size_type &pos);
Expr::valid_int read_expr (std::string const &expr, std::string const &comment);
std::string subst_args (std::string const &orig, std::vector <std::pair <std::string, std::string> > const &args);
bool getline (std::string &ret);
void read_definitions ();
std::string read_filename (shevek::ristring &args);
void write_expr (Expr &e);
void write_out (Source const &s);
void write_byte (Expr::valid_int byte, int addr_offset);
void parse (input_line &input);
std::string make_base (std::string const &file);
int main (int argc, char **argv);
void assemble (std::vector <std::string> &in_files);
void disassemble (std::istream &in);

void dir_org (shevek::ristring &args, bool first, std::list <Label>::iterator current_label);
void dir_defb (shevek::ristring &args, bool first, std::list <Label>::iterator current_label);
void dir_comment (shevek::ristring &args, bool first, std::list <Label>::iterator current_label);
void dir_equ (shevek::ristring &args, bool first, std::list <Label>::iterator current_label);
void dir_include (shevek::ristring &args, bool first, std::list <Label>::iterator current_label);
void dir_incbin (shevek::ristring &args, bool first, std::list <Label>::iterator current_label);
void dir_seek (shevek::ristring &args, bool first, std::list <Label>::iterator current_label);
void dir_macro (shevek::ristring &args, bool first, std::list <Label>::iterator current_label);
void dir_endmacro (shevek::ristring &args, bool first, std::list <Label>::iterator current_label);
void dir_if (shevek::ristring &args, bool first, std::list <Label>::iterator current_label);
void dir_else (shevek::ristring &args, bool first, std::list <Label>::iterator current_label);
void dir_endif (shevek::ristring &args, bool first, std::list <Label>::iterator current_label);
void dir_error (shevek::ristring &args, bool first, std::list <Label>::iterator current_label);

extern Oper operators1[3];
extern Oper operators2[19];
extern Oper operators3[1];
extern Directive directives[13];

#endif
