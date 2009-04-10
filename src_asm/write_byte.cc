#include "asm.hh"
#include <iomanip>

void write_byte (Expr::valid_int byte, int addr_offset)
{
	startfunc;
	if (!byte.valid)
		shevek_error ("trying to write invalid byte");
	if (!writing)
		return;
	if ((byte.value < -0x80) || (byte.value >= 0x100))
		error (shevek::rostring ("byte %x out of range", byte.value));
	if (usehex)
		hexfile.set (addr + addr_offset, byte.value);
	else
		*outfile << (char)byte.value;
	if (listfile)
		*listfile << ' ' << std::setfill ('0') << std::setw (2) << (byte.value & 0xff);
}
