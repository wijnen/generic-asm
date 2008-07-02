#include "asm.hh"
#include <shevek/args.hh>
#include <shevek/error.hh>
#include <iostream>
#include <fstream>

int main (int argc, char **argv)
{
	std::string defs;
	shevek::args::option opts[] = {
		shevek::args::option
			(0, "defs", "code definitions", false, defs),
	};
	shevek::args args
		(argc, argv, opts, 0, 1, "Generic assembler", "2008");
	if (defs.empty ())
		shevek_error ("you must specify a definitions file");
	input_stack.push (Input ());
	input_stack.top ().name = defs;
	input_stack.top ().type = Input::FILE;
	input_stack.top ().file = new std::ifstream (defs.c_str ());
	input_stack.top ().must_delete = true;
	if (!*input_stack.top ().file)
	{
		delete input_stack.top ().file;
		shevek_error_errno
			("unable to open definitions file " << defs);
	}
	read_definitions ();
	if (errors)
		return 1;
	// Read input in memory.
	std::vector <input_line> input;
	bool must_delete;
	input_stack.push (Input ());
	input_stack.top ().type = Input::FILE;
	input_stack.top ().ln = 0;
	if (args.size () == 0 || args[0] == "-")
	{
		input_stack.top ().name = "Standard input";
		input_stack.top ().file = &std::cin;
		input_stack.top ().must_delete = false;
	}
	else
	{
		input_stack.top ().name = args[0];
		input_stack.top ().file = new std::ifstream
			(input_stack.top ().name.c_str ());
		if (!input_stack.top ().file)
			shevek_error_errno ("unable to open input file "
					<< input_stack.top ().name);
		input_stack.top ().must_delete = true;
	}
	std::string line;
	while (getline (line))
		input.push_back (input_line (line));
	// Determine labels
	bool first = true;
	int undefined_labels = -1;
	while (undefined_labels != 0)
	{
		int last_undefined_labels = undefined_labels;
		addr = 0;
		undefined_labels = 0;
		for (unsigned t = 0; t < input.size (); ++t)
		{
			undefined_labels += parse (input[t], false, first,
					false);
		}
		dbg ("undefined labels: " << undefined_labels);
		if (undefined_labels == last_undefined_labels)
		{
			error ("Label dependency loop");
			for (unsigned t = 0; t < input.size (); ++t)
			{
				parse (input[t], true, true, true);
			}
			return 1;
		}
		if (errors)
			return 1;
		first = false;
	}
	// Write output
	addr = 0;
	for (unsigned t = 0; t < input.size (); ++t)
	{
		parse (input[t], true, false, false);
	}
	if (errors)
		return 1;
	return 0;
}
