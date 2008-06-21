#include "asm.hh"
#include <iostream>

void error (std::string const &message)
{
	std::cerr << input_stack.top ().name << ':' << input_stack.top ().ln
		<< ": " << message << std::endl;
	++errors;
}
