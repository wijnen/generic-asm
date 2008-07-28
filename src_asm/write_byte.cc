#include "asm.hh"
#include <iomanip>

void write_byte (int byte, int addr_offset)
{
	if ((byte < -0x80) || (byte >= 0x100))
		error (shevek::ostring ("byte %x out of range", byte));
	if (usehex)
		hexfile.set (addr + addr_offset, byte);
	else
		*outfile << (char)byte;
	if (listfile)
		*listfile << ' ' << std::setfill ('0') << std::setw (2)
			<< (byte & 0xff);
}
