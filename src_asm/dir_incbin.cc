#include "asm.hh"
#include <fstream>

unsigned dir_incbin (shevek::istring &args, bool write, bool first,
		Label *current_label)
{
	(void)current_label;
	std::string filename = Glib::filename_from_utf8 (args.rest ());
	std::ifstream file (filename.c_str ());
	if (!file)
	{
		error (shevek::ostring ("unable to open %s", args.rest ()));
		return 0;
	}
	char c;
	while (file >> c)
	{
		if (write)
			write_byte (c, 0);
		++addr;
	}
	if (write && listfile)
		*listfile << "...\t\t";
	return 0;
}
