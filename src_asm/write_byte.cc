#include "asm.hh"
#include <iostream>

void write_byte (int byte)
{
	if ((byte < -0x80) || (byte >= 0x100))
		error (shevek::ostring ("byte %x out of range", byte));
	std::cout << (char)byte;
}
