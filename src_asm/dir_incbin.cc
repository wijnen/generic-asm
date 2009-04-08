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
		if (files.back ().blocks.empty ())
			files.back ().blocks.push_back (File::Block ());
		if (files.back ().blocks.back ().parts.empty () || files.back ().blocks.back ().parts.back ().type != File::Block::Part::CODE)
		{
			files.back ().blocks.back ().parts.push_back (File::Block::Part ());
			files.back ().blocks.back ().parts.back ().type = File::Block::Part::CODE;
			files.back ().blocks.back ().parts.back ().have_expr = false;
		}
		files.back ().blocks.back ().parts.back ().name += str;
	}
	addr += str.size ();
	if (writing && listfile)
		*listfile << "...\t\t";
}
