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
	bool useobject = false;
	include_path.push_back (".");
	shevek::args::option opts[] = {
		shevek::args::option (0, "defs", "code definitions", false, defs),
		shevek::args::option ('o', "output", "output file", false, outfilename),
		shevek::args::option ('L', "list", "list file", false, listfilename),
		shevek::args::option ('b', "binary", "binary output format", usehex, false),
		shevek::args::option ('O', "object", "object output format", useobject, true),
		shevek::args::option ('I', "includedir", "add directory to include path", include_path),
	};
	shevek::args args (argc, argv, opts, 0, 1, "Generic assembler", "2008");
	if (defs.empty ())
		shevek_error ("you must specify a definitions file");
	if (!usehex && useobject)
		shevek_error ("specify only one type of output file");
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

	// TODO: make this configurable.
	spaces.push_back (Space ());
	spaces.back ().start = 0;
	spaces.back ().size = 0x10000;

	input_stack.push (Input ());
	input_stack.top ().name = defs;
	input_stack.top ().basedir = make_base (defs);
	input_stack.top ().type = Input::FILE;
	input_stack.top ().file = new std::ifstream (defs.c_str ());
	input_stack.top ().must_delete = true;
	if (!*input_stack.top ().file)
	{
		delete input_stack.top ().file;
		shevek_error_errno (shevek::ostring ("unable to open definitions file %s", Glib::ustring (defs)));
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
	std::vector <std::string> in_files (args.size ());
	if (args.size () == 0)
		in_files.push_back ("-");
	else for (unsigned i = 0; i < args.size (); ++i)
		in_files[i] = args[i];
	for (unsigned f = 0; f < in_files.size (); ++f)
	{
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
			input_stack.top ().file = new std::ifstream
				(input_stack.top ().name.c_str ());
			if (!input_stack.top ().file)
				shevek_error_errno (shevek::ostring ("unable to open input file %s", Glib::ustring (input_stack.top ().name)));
			input_stack.top ().must_delete = true;
		}
		// Determine labels
		Glib::ustring line;
		undefined_locals = 0;
		writing = false;
		while (getline (line))
		{
			input.push_back (input_line (line));
			parse (input.back (), true, false);
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
		unsigned last_undefined_locals = ~0;
		while (undefined_locals != last_undefined_locals)
		{
			last_undefined_locals = undefined_locals;
			addr = 0;
			undefined_locals = 0;
			for (unsigned t = 0; t < input.size (); ++t)
			{
				parse (input[t], false, false);
			}
			dbg ("undefined local labels: " << undefined_locals);
			if (errors)
				return 1;
		}
		if (undefined_locals != 0)
		{
			for (unsigned t = 0; t < input.size (); ++t)
			{
				parse (input[t], false, true);
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
		// Write output to internal object representation
		writing = true;
		addr = 0;
		for (unsigned t = 0; t < input.size (); ++t)
		{
			parse (input[t], false, false);
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
		// Clean up
		std::list <Label>::iterator l, next;
		for (l = labels.begin (), next = l; l != labels.end (); l = next)
		{
			++next;
			if (l->name[0] == '.' || l->name[0] == '@')
				labels.erase (l);
			else
				l->definition = NULL;
		}
	}
	if (useobject)
	{
		std::string script ("#\xfeof\0\0\0\0", 8), code;
		bool first = true;
		for (std::list <File>::iterator i = files.begin (); i != files.end (); ++i)
		{
			if (first)
				first = false;
			else
				script += "-\n";
			i->write_object (script, code);
		}
		*outfile << script << '\0' << code << std::flush;
	}
	else
	{
		for (std::list <File>::iterator i = files.begin (); i != files.end (); ++i)
			i->write_binary ();
	}
	if (usehex)
		hexfile.write_s19 (*outfile);
	if (!outfilename.empty ())
		delete outfile;
	if (listfile)
		delete listfile;
	return 0;
}
