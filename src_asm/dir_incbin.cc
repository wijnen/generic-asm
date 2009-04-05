#include "asm.hh"
#include <fstream>

void dir_incbin (shevek::istring &args, bool first, Label *current_label)
{
	(void)first;
	(void)current_label;
	std::string filename = Glib::filename_from_utf8 (args.rest ());
	std::ifstream file (filename.c_str ());
	if (!file)
	{
		error (shevek::ostring ("unable to open %s", args.rest ()));
		return;
	}
	char c;
	while (file >> c)
	{
		Expr::valid_int i;
		i.value = c;
		i.valid = false;
		write_byte (i, 0);
		++addr;
	}
	if (writing && listfile)
		*listfile << "...\t\t";
}
