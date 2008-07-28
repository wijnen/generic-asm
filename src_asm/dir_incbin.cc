#include "asm.hh"
#include <fstream>

unsigned dir_incbin (shevek::istring &args, bool write, bool first,
		Label *current_label)
{
	(void)current_label;
	std::ifstream file (args.rest ().c_str ());
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
