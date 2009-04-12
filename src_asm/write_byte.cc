#include "asm.hh"
#include <iomanip>

void write_byte (Expr::valid_int byte, int addr_offset)
{
	startfunc;
	if (!byte.valid)
	{
		for (std::list <std::string>::iterator i = byte.invalid.begin (); i != byte.invalid.end (); ++i)
			std::cerr << "use of undefined label " << *i << std::endl;
		++errors;
		return;
	}
	if ((byte.value < -0x80) || (byte.value >= 0x100))
	{
		std::cerr << shevek::rostring ("byte %x out of range", byte.value);
		++errors;
		return;
	}
	if (usehex)
		hexfile.set (addr + addr_offset, byte.value);
	else
		*outfile << (char)byte.value;
	if (listfile)
		*listfile << ' ' << std::setfill ('0') << std::setw (2) << (byte.value & 0xff);
}
