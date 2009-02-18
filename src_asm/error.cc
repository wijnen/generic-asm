#include "asm.hh"
#include <iostream>
#include <shevek/error.hh>
#include <shevek/debug.hh>

void error (Glib::ustring const &message)
{
	if (!current_stack)
	{
		std::cerr << "From definitions file: " << message << '\n';
	}
	else
	{
		if (current_stack->empty ())
			shevek_error ("bug in assembler: empty stack");
		std::cerr << current_stack->back ().second << ':' << current_stack->back ().first << ": " << message << '\n';
		for (std::list <std::pair <unsigned, Glib::ustring> >::reverse_iterator i = ++current_stack->rbegin (); i != current_stack->rend (); ++i)
			std::cerr << i->second << ':' << i->first << ": included from here\n";
	}
	std::cerr.flush ();
	++errors;
}
