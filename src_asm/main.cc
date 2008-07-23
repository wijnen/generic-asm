#include "asm.hh"
#include <shevek/args.hh>
#include <shevek/error.hh>
#include <iostream>
#include <fstream>

static std::string make_base (std::string const &file)
{
	std::string::size_type pos = file.find_last_of ("/");
	if (pos == std::string::npos)
		return ".";
	return file.substr (0, pos);
}

int main (int argc, char **argv)
{
	std::string defs;
	std::string outfilename, listfilename;
	usehex = true;
	include_path.push_back (".");
	shevek::args::option opts[] = {
		shevek::args::option
			(0, "defs", "code definitions", false, defs),
		shevek::args::option ('o', "output",
				"output file", false, outfilename),
		shevek::args::option ('L', "list",
				"list file", false, listfilename),
		shevek::args::option ('h', "hex",
				"intel hex output format", usehex, true),
		shevek::args::option ('b', "binary",
				"binary output format", usehex, false),
		shevek::args::option ('I', "includedir",
				"add directory to include path", include_path),
	};
	shevek::args args
		(argc, argv, opts, 0, 1, "Generic assembler", "2008");
	if (defs.empty ())
		shevek_error ("you must specify a definitions file");
	if (outfilename.empty ())
		outfile = &std::cout;
	else
	{
		outfile = new std::ofstream (outfilename.c_str ());
		if (!*outfile)
			shevek_error_errno ("unable to open output file");
	}
	if (!listfilename.empty ())
	{
		listfile = new std::ofstream (listfilename.c_str ());
		if (!*listfile)
		{
			shevek_warning_errno ("unable to open list file");
			delete listfile;
			listfile = NULL;
		}
		*listfile << std::hex;
	}

	input_stack.push (Input ());
	input_stack.top ().name = defs;
	input_stack.top ().basedir = make_base (defs);
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
	{
		if (!outfilename.empty ())
		{
			delete outfile;
			unlink (outfilename.c_str ());
		}
		if (listfile)
			unlink (listfilename.c_str ());
		return 1;
	}
	// Read input in memory.
	std::vector <input_line> input;
	bool must_delete;
	input_stack.push (Input ());
	input_stack.top ().type = Input::FILE;
	input_stack.top ().ln = 0;
	if (args.size () == 0 || args[0] == "-")
	{
		input_stack.top ().name = "Standard input";
		input_stack.top ().basedir = ".";
		input_stack.top ().file = &std::cin;
		input_stack.top ().must_delete = false;
	}
	else
	{
		input_stack.top ().name = args[0];
		input_stack.top ().basedir = make_base (args[0]);
		input_stack.top ().file = new std::ifstream
			(input_stack.top ().name.c_str ());
		if (!input_stack.top ().file)
			shevek_error_errno ("unable to open input file "
					<< input_stack.top ().name);
		input_stack.top ().must_delete = true;
	}
	// Determine labels
	int undefined_labels = 0;
	std::string line;
	while (getline (line))
	{
		input.push_back (input_line (line));
		undefined_labels += parse (input.back (), false, true, false);
	}
	if (errors)
	{
		if (!outfilename.empty ())
		{
			delete outfile;
			unlink (outfilename.c_str ());
		}
		if (listfile)
			unlink (listfilename.c_str ());
		return 1;
	}
	while (undefined_labels != 0)
	{
		int last_undefined_labels = undefined_labels;
		addr = 0;
		undefined_labels = 0;
		for (unsigned t = 0; t < input.size (); ++t)
		{
			undefined_labels += parse (input[t], false, false,
					false);
		}
		dbg ("undefined labels: " << undefined_labels);
		if (undefined_labels == last_undefined_labels)
		{
			error ("Label dependency loop");
			for (unsigned t = 0; t < input.size (); ++t)
			{
				parse (input[t], false, false, true);
			}
			if (!outfilename.empty ())
			{
				delete outfile;
				unlink (outfilename.c_str ());
			}
			if (listfile)
				unlink (listfilename.c_str ());
			return 1;
		}
		if (errors)
			return 1;
	}
	// Write output
	addr = 0;
	for (unsigned t = 0; t < input.size (); ++t)
	{
		parse (input[t], true, false, false);
	}
	if (errors)
	{
		if (!outfilename.empty ())
		{
			delete outfile;
			unlink (outfilename.c_str ());
		}
		if (listfile)
			unlink (listfilename.c_str ());
		return 1;
	}
	if (usehex)
		hexfile.write (*outfile);
	if (!outfilename.empty ())
		delete outfile;
	if (listfile)
		delete listfile;
	return 0;
}
