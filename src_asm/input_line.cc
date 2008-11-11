#include "asm.hh"

input_line::input_line (Glib::ustring d)
	: data (d)
{
	std::stack <Input> tmp = input_stack;
	while (!tmp.empty ())
	{
		stack.push_front (std::make_pair (tmp.top ().ln,
					tmp.top ().name));
		tmp.pop ();
	}
}
