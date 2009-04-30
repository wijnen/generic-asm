#include "asm.hh"

void error (std::string const &message)
{
	if (!current_stack)
	{
		std::cerr << "From definitions file: " << message << '\n';
	}
	else
	{
		if (current_stack->empty ())
		{
			std::cerr << "After compiling: " << message << '\n';
		}
		std::cerr << current_stack->back ().second << ':' << current_stack->back ().first << ": " << message << '\n';
		for (std::list <std::pair <unsigned, std::string> >::iterator i = ++current_stack->begin (); i != current_stack->end (); ++i)
			std::cerr << i->second << ':' << i->first << ": included from here\n";
	}
	std::cerr.flush ();
	++errors;
}
