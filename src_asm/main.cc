#include "asm.hh"
#include <shevek/args.hh>
#include <shevek/error.hh>
#include <iostream>
#include <fstream>

int main (int argc, char **argv)
{
	std::string defs;
	shevek::args::option opts[] = {
		shevek::args::option (0, "defs", "code definitions",
				false, defs),
	};
	shevek::args args (argc, argv, opts, 0, 1, "Generic assembler", "2008");
	if (defs.empty ())
		shevek_error ("you must specify a definitions file");
	std::ifstream file (defs.c_str ());
	if (!file)
		shevek_error_errno ("unable to open file " << defs);
	std::string line;
	input_stack.push (Input ());
	input_stack.top ().name = defs;
	input_stack.top ().type = Input::FILE;
	input_stack.top ().file = &file;
	read_definitions ();
	if (errors)
		return 1;
	std::vector <std::pair <unsigned, std::string> > input;
	std::istream *infile;
	bool must_delete;
	if (args.size () == 0 || args[0] == "-")
	{
		infile = &std::cin;
		must_delete = false;
		input_stack.top ().name = "Standard input";
	}
	else
	{
		infile = new std::ifstream (args[0].c_str ());
		if (!infile)
			shevek_error_errno ("unable to open input file "
					<< infile);
		must_delete = true;
		input_stack.top ().name = args[0];
	}
	std::string pre;
	input_stack.top ().ln = 0;
	while (std::getline (*infile, line))
	{
		++input_stack.top ().ln;
		if (!line.empty () && line[line.size () - 1] == '\\')
		{
			pre += line.substr (0, line.size () - 1);
			continue;
		}
		input.push_back (std::make_pair (input_stack.top ().ln,
					pre + line));
		pre.clear ();
	}
	if (!pre.empty ())
		input.push_back (std::make_pair (input_stack.top ().ln, pre));
	if (must_delete)
		delete infile;
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
			input_stack.top ().ln = input[t].first;
			undefined_labels
				+= parse (input[t].second, false, first);
		}
		if (undefined_labels == last_undefined_labels)
		{
			error ("Label dependency loop");
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
		input_stack.top ().ln = input[t].first;
		parse (input[t].second, true, false);
	}
	if (errors)
		return 1;
	return 0;
}
