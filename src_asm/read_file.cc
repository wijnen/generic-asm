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
	unsigned num_if = 0;
	unsigned current = 0;
	bool bytes (true);
	blocks.push_back (Block ());
	while (!script.rest ().empty ())
	{
		std::string name, expr;
		if (script ("1\n"))
		{
			bytes = true;
			continue;
		}
		if (script ("2\n"))
		{
			bytes = false;
			continue;
		}
		if (script ("?%r/[^\n]*/\n", expr))
		{
			std::string::size_type p = 0;
			Expr e = Expr::read (expr, false, p);
			if (p != expr.size ())
				error ("error in if expression " + expr + " in " + filename);
			++num_if;
			blocks.back ().parts.push_back (Block::Part ());
			blocks.back ().parts.back ().type = Block::Part::IF;
			blocks.back ().parts.back ().have_expr = true;
			blocks.back ().parts.back ().expr = e;
			if (listfile)
				*listfile << "if " << e.print () << '\n';
			continue;
		}
		if (script (":\n"))
		{
			if (num_if == 0)
				error ("else without if in " + filename);
			blocks.back ().parts.push_back (Block::Part ());
			blocks.back ().parts.back ().type = Block::Part::ELSE;
			blocks.back ().parts.back ().have_expr = false;
			if (listfile)
				*listfile << "else\n";
			continue;
		}
		if (script (";\n"))
		{
			if (num_if == 0)
				error ("endif without if in " + filename);
			--num_if;
			blocks.back ().parts.push_back (Block::Part ());
			blocks.back ().parts.back ().type = Block::Part::ENDIF;
			blocks.back ().parts.back ().have_expr = false;
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
			blocks.back ().parts.push_back (Block::Part ());
			blocks.back ().parts.back ().type = Block::Part::DEFINE;
			blocks.back ().parts.back ().have_expr = true;
			blocks.back ().parts.back ().expr = e;
			blocks.back ().parts.back ().name = name;
			blocks.back ().parts.back ().label = --labels.end ();
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
			blocks.back ().parts.push_back (Block::Part ());
			blocks.back ().parts.back ().type = Block::Part::BYTE;
			blocks.back ().parts.back ().have_expr = true;
			blocks.back ().parts.back ().expr = e;
			if (listfile)
				*listfile << "defb " << e.print () << '\n';
			continue;
		}
		if (script ("<%r/[^\n]*/\n", expr))
		{
			blocks.push_back (Block ());
			if (expr.empty ())
			{
				blocks.back ().absolute = false;
				if (listfile)
					*listfile << "org relative\n";
			}
			else
			{
				blocks.back ().absolute = true;
				std::string::size_type p = 0;
				Expr::valid_int i = read_expr (expr, false, p);
				if (!i.valid || p != expr.size ())
					error ("invalid org expression in " + filename);
				blocks.back ().address = i.value;
				if (listfile)
					*listfile << "org 0x" << i.value << '\n';
			}
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
			blocks.back ().parts.push_back (Block::Part ());
			blocks.back ().parts.back ().type = Block::Part::CODE;
			blocks.back ().parts.back ().have_expr = false;
			for (int k = 0; k < i.value; ++k)
			{
				if (bytes)
					blocks.back ().parts.back ().code.push_back (code[current + k]);
				else
					blocks.back ().parts.back ().code.push_back (code[current + 2 * k] | (code[current + 2 * k + 1] << 8));
			}
			current += (bytes ? 1 : 2) * i.value;
			if (listfile)
				*listfile << "using 0x" << i.value << " bytes of code\n";
			continue;
		}
		if (script ("#%r/[^\n]*/\n", name))
		{
			blocks.back ().parts.push_back (Block::Part ());
			blocks.back ().parts.back ().type = Block::Part::COMMENT;
			blocks.back ().parts.back ().have_expr = false;
			blocks.back ().parts.back ().name = name;
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
