#include "asm.hh"

std::string escape (std::string const &in)
{
	std::string out;
	for (unsigned i = 0; i < in.size (); ++i)
	{
		if (in[i] == '%')
			out += "%%";
		else
			out += in[i];
	}
	return out;
}
