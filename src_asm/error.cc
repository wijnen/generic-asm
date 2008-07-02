#include "asm.hh"
#include <iostream>
#include <shevek/error.hh>

void error (std::string const &message)
{
	if (!input_stack.empty ())
	{
		input_line l ("");
		current_stack = &l.stack;
	}
	if (current_stack->empty ())
		shevek_error ("bug in assembler: empty stack");
	std::cerr << current_stack->front ().second << ':'
		<< current_stack->front ().first << ": " << message << '\n';
	for (std::list <std::pair <unsigned, std::string> >::iterator
			i = ++current_stack->begin ();
			i != current_stack->end (); ++i)
		std::cerr << i->second << ':' << i->first
			<< ": included from here\n";
	std::cerr.flush ();
	++errors;
}
