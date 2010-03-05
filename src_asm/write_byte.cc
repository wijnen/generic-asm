#include "asm.hh"
#include <iomanip>

void write_byte (Expr::valid_int byte, int addr_offset)
{
	startfunc;
	dbg (shevek::ostring ("Writing byte 0x%02x at 0x%04x+0x%x", byte.value, addr, addr_offset));
	if (!byte.valid)
	{
		for (std::list <std::string>::iterator i = byte.invalid.begin (); i != byte.invalid.end (); ++i)
			std::cerr << "use of undefined label " << *i << std::endl;
		++errors;
		return;
	}
	if (use_bytes)
	{
		if ((byte.value < -0x80) || (byte.value >= 0x100))
		{
			std::cerr << shevek::rostring ("byte %02x out of range", byte.value);
			++errors;
			return;
		}
		if (usehex)
			hexfile.set (addr + addr_offset, byte.value);
		else
			*outfile << (char)byte.value;
	}
	else
	{
		if ((byte.value < -0x8000) || (byte.value >= 0x10000))
		{
			std::cerr << shevek::rostring ("word %04x out of range", byte.value);
			++errors;
			return;
		}
		if (usehex)
			hexfile.set (addr + addr_offset, byte.value);
		else
			*outfile << ((char)(byte.value >> 8)) << (char)byte.value;
	}
}
