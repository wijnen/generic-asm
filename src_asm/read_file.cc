#include "asm.hh"
#include <fstream>

#define error(x) do { std::cerr << (x) << std::endl; ++errors; return false; } while (0)

bool read_file (std::string const &filename)
{
	std::ifstream file (filename.c_str ());
	if (!file)
		return false;
	std::ostringstream s;
	s << file.rdbuf ();
	std::string str = s.str ();
	if (!file || str.size () < 9 || str.substr (0, 4) != "#\xfeof")
		return false;
	if (listfile)
		*listfile << "# Object source file: " << filename << '\n';
	if (str.substr (4, 4) != std::string ("\0\0\0\0", 4))
		error ("invalid object file format version");
	std::string::size_type pos = str.find ('\0', 8);
	if (pos == std::string::npos)
		error ("object file doesn't contain separator");
	shevek::ristring script (str.substr (8, pos - 8));
	std::string code = str.substr (pos + 1);
	files.push_back (File ());
	unsigned num_if = 0;
	unsigned current = 0;
	while (!script.rest ().empty ())
	{
		std::string name, expr;
		if (script ("?%r/[^\n]*/\n", expr))
		{
			std::string::size_type p = 0;
			Expr e = Expr::read (expr, false, p);
			if (p != expr.size ())
				error ("error in if expression " + expr + " in " + filename);
			++num_if;
			if (files.back ().blocks.empty ())
				files.back ().blocks.push_back (File::Block ());
			files.back ().blocks.back ().parts.push_back (File::Block::Part ());
			files.back ().blocks.back ().parts.back ().type = File::Block::Part::IF;
			files.back ().blocks.back ().parts.back ().have_expr = true;
			files.back ().blocks.back ().parts.back ().expr = e;
			if (listfile)
				*listfile << "if " << e.print () << '\n';
			continue;
		}
		if (script (":\n"))
		{
			if (num_if == 0)
				error ("else without if in " + filename);
			if (files.back ().blocks.empty ())
				files.back ().blocks.push_back (File::Block ());
			files.back ().blocks.back ().parts.push_back (File::Block::Part ());
			files.back ().blocks.back ().parts.back ().type = File::Block::Part::ELSE;
			files.back ().blocks.back ().parts.back ().have_expr = false;
			if (listfile)
				*listfile << "else\n";
			continue;
		}
		if (script (";\n"))
		{
			if (num_if == 0)
				error ("endif without if in " + filename);
			--num_if;
			if (files.back ().blocks.empty ())
				files.back ().blocks.push_back (File::Block ());
			files.back ().blocks.back ().parts.push_back (File::Block::Part ());
			files.back ().blocks.back ().parts.back ().type = File::Block::Part::ENDIF;
			files.back ().blocks.back ().parts.back ().have_expr = false;
			if (listfile)
				*listfile << "endif\n";
			continue;
		}
		if (script ("%r/[a-zA-Z_.@][a-zA-Z_.@0-9]*/=%r/[^\n]*/\n", name, expr))
		{
			std::string::size_type p = 0;
			Expr e = Expr::read (expr, false, p);
			if (p != expr.size ())
			{
				error ("invalid define expression " + expr + " for " + name + " in " + filename);
				continue;
			}
			if (find_label (name) != labels.end ())
			{
				error ("duplicate definition of label " + name + " in " + filename);
				continue;
			}
			labels.push_back (Label ());
			labels.back ().name = name;
			labels.back ().definition = NULL;
			labels.back ().value = e;
			if (files.back ().blocks.empty ())
				files.back ().blocks.push_back (File::Block ());
			files.back ().blocks.back ().parts.push_back (File::Block::Part ());
			files.back ().blocks.back ().parts.back ().type = File::Block::Part::DEFINE;
			files.back ().blocks.back ().parts.back ().have_expr = true;
			files.back ().blocks.back ().parts.back ().expr = e;
			files.back ().blocks.back ().parts.back ().name = name;
			files.back ().blocks.back ().parts.back ().label = --labels.end ();
			if (listfile)
				*listfile << name << '=' << e.print () << '\n';
			continue;
		}
		if (script ("+%r/[^\n]*/\n", expr))
		{
			std::string::size_type p = 0;
			Expr e = Expr::read (expr, false, p);
			if (p != expr.size ())
				error ("invalid add expression " + expr + " in " + filename);
			if (files.back ().blocks.empty ())
				files.back ().blocks.push_back (File::Block ());
			files.back ().blocks.back ().parts.push_back (File::Block::Part ());
			files.back ().blocks.back ().parts.back ().type = File::Block::Part::BYTE;
			files.back ().blocks.back ().parts.back ().have_expr = true;
			files.back ().blocks.back ().parts.back ().expr = e;
			if (listfile)
				*listfile << "defb " << e.print () << '\n';
			continue;
		}
		if (script ("<%r/[^\n]*/\n", expr))
		{
			files.back ().blocks.push_back (File::Block ());
			if (expr.empty ())
			{
				files.back ().blocks.back ().absolute = false;
				if (listfile)
					*listfile << "org relative\n";
			}
			else
			{
				files.back ().blocks.back ().absolute = true;
				std::string::size_type p = 0;
				Expr::valid_int i = read_expr (expr, false, p);
				if (!i.valid || p != expr.size ())
					error ("invalid org expression in " + filename);
				files.back ().blocks.back ().address = i.value;
				if (listfile)
					*listfile << "org 0x" << i.value << '\n';
			}
			continue;
		}
		if (script ("-\n"))
		{
			files.push_back (File ());
			if (listfile)
				*listfile << "-- File separator --\n";
			continue;
		}
		if (script (">%r/[^\n]*/\n", expr))
		{
			std::string::size_type p = 0;
			Expr::valid_int i = read_expr (expr, false, p);
			if (!i.valid || p != expr.size ())
				error ("invalid code expression " + expr + " in " + filename);
			if (i.value + current > code.size ())
				error ("trying to use more than available data in " + filename);
			if (files.back ().blocks.empty ())
				files.back ().blocks.push_back (File::Block ());
			files.back ().blocks.back ().parts.push_back (File::Block::Part ());
			files.back ().blocks.back ().parts.back ().type = File::Block::Part::CODE;
			files.back ().blocks.back ().parts.back ().have_expr = false;
			files.back ().blocks.back ().parts.back ().name = code.substr (current, i.value);
			current += i.value;
			if (listfile)
				*listfile << "using 0x" << i.value << " bytes of code\n";
			continue;
		}
		if (script ("#%r/[^\n]*/\n", name))
		{
			if (files.back ().blocks.empty ())
				files.back ().blocks.push_back (File::Block ());
			files.back ().blocks.back ().parts.push_back (File::Block::Part ());
			files.back ().blocks.back ().parts.back ().type = File::Block::Part::COMMENT;
			files.back ().blocks.back ().parts.back ().have_expr = false;
			files.back ().blocks.back ().parts.back ().name = name;
			if (listfile)
				*listfile << "# comment: " << name << '\n';
			continue;
		}
		error ("invalid script in object file " + filename + ": " + script.rest ());
	}
	if (listfile)
		*listfile << "# End of object source file: " << filename << '\n';
	return true;
}
