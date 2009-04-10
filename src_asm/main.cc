#include "asm.hh"
#include <shevek/args.hh>
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
	shevek::args args (argc, argv, opts, 0, -1, "Generic assembler", "2008");
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
		if (listfilename == "-")
			listfile = &std::cout;
		else
			listfile = new std::ofstream (listfilename.c_str ());
		if (!*listfile)
		{
			shevek_warning_errno ("unable to open list file " + Glib::filename_to_utf8 (listfilename));
			if (listfile != &std::cout)
				delete listfile;
			listfile = NULL;
		}
		*listfile << std::hex;
	}

	// TODO: make this configurable.
	spaces.push_back (Space ());
	spaces.back ().start = 0;
	spaces.back ().size = 0x10000;

	if (!defs.empty ())
	{
		input_stack.push (Input ());
		input_stack.top ().name = defs;
		input_stack.top ().basedir = make_base (defs);
		input_stack.top ().type = Input::FILE;
		input_stack.top ().file = new std::ifstream (defs.c_str ());
		input_stack.top ().must_delete = true;
		if (!*input_stack.top ().file)
		{
			delete input_stack.top ().file;
			shevek_error_errno (shevek::rostring ("unable to open definitions file %s", std::string (defs)));
		}
		read_definitions ();
		if (errors)
		{
			if (!outfilename.empty ())
			{
				delete outfile;
				unlink (outfilename.c_str ());
			}
			if (listfile && listfile != &std::cout)
			{
				unlink (listfilename.c_str ());
				delete listfile;
			}
			return 1;
		}
	}
	std::vector <std::string> in_files (args.size ());
	if (args.size () == 0)
		in_files.push_back ("-");
	else for (unsigned i = 0; i < args.size (); ++i)
		in_files[i] = args[i];
	for (unsigned f = 0; f < in_files.size (); ++f)
	{
		if (read_file (in_files[f]))
			continue;
		if (errors)
		{
			if (!outfilename.empty ())
			{
				delete outfile;
				unlink (outfilename.c_str ());
			}
			if (listfile && listfile != &std::cout)
			{
				unlink (listfilename.c_str ());
				delete listfile;
			}
			return 1;
		}
		if (defs.empty ())
		{
			error ("Without a definitions file, only object files are allowed at input (not " + in_files[f] + ")");
			continue;
		}
		files.push_back (File ());
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
		first_pass = true;
		report_labels = false;
		while (getline (line))
		{
			input.push_back (input_line (line));
			parse (input.back ());
		}
		if (errors)
		{
			if (!outfilename.empty ())
			{
				delete outfile;
				unlink (outfilename.c_str ());
			}
			if (listfile && listfile != &std::cout)
			{
				unlink (listfilename.c_str ());
				delete listfile;
			}
			return 1;
		}
		first_pass = false;
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
				return 1;
		}
		if (undefined_locals != 0)
		{
			report_labels = true;
			absolute_addr = false;
			addr = 0;
			for (unsigned t = 0; t < input.size (); ++t)
			{
				parse (input[t]);
			}
			if (!outfilename.empty ())
			{
				delete outfile;
				unlink (outfilename.c_str ());
			}
			if (listfile && listfile != &std::cout)
			{
				unlink (listfilename.c_str ());
				delete listfile;
			}
			return 1;
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
		// Clean up.
		files.back ().clean ();
		// Don't continue if there are errors.
		if (errors)
		{
			if (!outfilename.empty ())
			{
				delete outfile;
				unlink (outfilename.c_str ());
			}
			if (listfile && listfile != &std::cout)
			{
				unlink (listfilename.c_str ());
				delete listfile;
			}
			return 1;
		}
		if (listfile)
			*listfile << "# End of asm source file: " << in_files[f] << '\n';
	}
	// All files are done; do final cleanup.
	for (std::list <Label>::iterator l = labels.begin (); l != labels.end (); ++l)
	{
		for (std::list <ExprElem>::iterator e = l->value.list.begin (); e != l->value.list.end (); ++e)
		{
			if (e->type != ExprElem::ISLABEL)
				continue;
			e->type = ExprElem::NUM;
			e->value.valid = true;
			e->value.value = find_label (e->label) != labels.end ();
			e->label.clear ();
		}
		l->value.simplify ();
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
		errors = 0;
		for (std::list <File>::iterator i = files.begin (); i != files.end (); ++i)
			i->write_binary ();
		if (errors)
		{
			if (!outfilename.empty ())
			{
				delete outfile;
				unlink (outfilename.c_str ());
			}
			if (listfile && listfile != &std::cout)
			{
				unlink (listfilename.c_str ());
				delete listfile;
			}
			return 1;
		}
	}
	if (!useobject && usehex)
		hexfile.write_s19 (*outfile);
	if (listfile)
	{
		*listfile << "\n# List of labels:\n";
		for (std::list <Label>::iterator l = labels.begin (); l != labels.end (); ++l)
			*listfile << l->name << ":\tequ 0x" << l->value.compute (Expr::valid_int (">>")).value << '\n';
	}
	if (!outfilename.empty ())
		delete outfile;
	if (listfile && listfile != &std::cout)
		delete listfile;
	return 0;
}
