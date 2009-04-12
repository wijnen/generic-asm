#include "asm.hh"
#include <fstream>

void dir_incbin (shevek::ristring &args, bool first, std::list <Label>::iterator current_label)
{
	(void)first;
	(void)current_label;
	std::string filename = Glib::filename_from_utf8 (args.rest ());
	std::ifstream file (filename.c_str ());
	if (!file)
	{
		error (shevek::rostring ("unable to open %s", args.rest ()));
		return;
	}
	std::ostringstream s;
	s << file.rdbuf ();
	std::string str = s.str ();
	if (writing)
	{
		if (blocks.empty ())
			blocks.push_back (Block ());
		if (blocks.back ().parts.empty () || blocks.back ().parts.back ().type != Block::Part::CODE)
		{
			blocks.back ().parts.push_back (Block::Part ());
			blocks.back ().parts.back ().type = Block::Part::CODE;
			blocks.back ().parts.back ().have_expr = false;
		}
		blocks.back ().parts.back ().name += str;
	}
	addr += str.size ();
	if (writing && listfile)
		*listfile << "...\t\t";
}
