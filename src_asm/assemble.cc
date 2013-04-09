#include "asm.hh"
#include <fstream>

std::string make_base (std::string const &file)
{
	std::string::size_type pos = file.find_last_of ("/");
	if (pos == std::string::npos)
		return ".";
	return file.substr (0, pos);
}

void assemble (std::vector <std::string> &in_files)
{
	for (unsigned f = 0; f < in_files.size (); ++f)
	{
		// Try to read file as object file.  If it works, continue.
		if (read_file (in_files[f]))
			continue;
		if (errors)
			return;
		if (sources.empty ())
		{
			error ("Without definitions, only object files are allowed at input (not " + in_files[f] + ")");
			continue;
		}
		blocks.push_back (Block ());
		// Read input in memory.
		std::vector <input_line> input;
		input_stack.push (Input ());
		input_stack.top ().type = Input::FILE;
		input_stack.top ().ln = 0;
		if (in_files[f] == "-")
		{
			input_stack.top ().name = "Standard input";
			input_stack.top ().basedir = ".";
			input_stack.top ().file = &std::cin;
			input_stack.top ().must_delete = false;
		}
		else
		{
			input_stack.top ().name = in_files[f];
			input_stack.top ().basedir = make_base (in_files[f]);
			input_stack.top ().file = new std::ifstream (input_stack.top ().name.c_str ());
			if (!input_stack.top ().file)
				shevek_error_errno (shevek::rostring ("unable to open input file %s", std::string (input_stack.top ().name)));
			input_stack.top ().must_delete = true;
		}
		// Determine labels
		std::string line;
		undefined_locals = 0;
		absolute_addr = false;
		addr = 0;
		writing = false;
		stage = 1;
		report_labels = false;
		while (getline (line))
		{
			input.push_back (input_line (line));
			parse (input.back ());
		}
		if (errors)
			return;
		stage = 2;
		unsigned last_undefined_locals = ~0;
		while (undefined_locals != last_undefined_locals)
		{
			last_undefined_locals = undefined_locals;
			absolute_addr = false;
			addr = 0;
			undefined_locals = 0;
			for (unsigned t = 0; t < input.size (); ++t)
			{
				parse (input[t]);
			}
			dbg ("undefined local labels: " << undefined_locals);
			if (errors)
				return;
		}
		if (undefined_locals != 0)
		{
			report_labels = true;
			absolute_addr = false;
			addr = 0;
			for (unsigned t = 0; t < input.size (); ++t)
				parse (input[t]);
			return;
		}
		// Write output to internal object representation.
		writing = true;
		if (listfile)
			*listfile << "# Asm source file: " << in_files[f] << '\n';
		absolute_addr = false;
		addr = 0;
		for (unsigned t = 0; t < input.size (); ++t)
		{
			parse (input[t]);
		}
		// Don't continue if there are errors.
		if (errors)
			return;
		if (listfile)
			*listfile << "# End of asm source file: " << in_files[f] << '\n';
		// Clean up.
		for (std::list <Block>::iterator i = blocks.begin (); i != blocks.end (); ++i)
			i->clean (true);
	}
	// All files are done; do final cleanup.
	for (std::list <Label>::iterator l = labels.begin (); l != labels.end (); ++l)
	{
		l->value.clean_islabel ();
		l->value.simplify ();
	}
}
