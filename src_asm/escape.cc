#include "asm.hh"

Glib::ustring escape (Glib::ustring const &in)
{
	Glib::ustring out;
	for (unsigned i = 0; i < in.size (); ++i)
	{
		if (in[i] == '%')
			out += "%%";
		else
			out += in[i];
	}
	return out;
}
