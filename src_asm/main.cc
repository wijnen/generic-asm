#include "asm.hh"
#include <shevek/args.hh>
#include <fstream>

int main (int argc, char **argv)
{
	std::string defs;
	std::string outfilename, listfilename;
	usehex = true;
	bool useobject = false;
	bool disasm = false;
	include_path.push_back (".");
	shevek::args::option opts[] = {
		shevek::args::option (0, "defs", "code definitions", false, defs),
		shevek::args::option ('o', "output", "output file", false, outfilename),
		shevek::args::option ('L', "list", "list file", false, listfilename),
		shevek::args::option ('b', "binary", "binary output format", usehex, false),
		shevek::args::option ('O', "object", "object output format", useobject, true),
		shevek::args::option ('I', "includedir", "add directory to include path", include_path),
		shevek::args::option ('d', "disasm", "disassemle binary input file", disasm, true),
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
		stage = 0;
		read_definitions ();
	}

	if (!errors)
	{
		if (!disasm)
		{
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
	
			std::vector <std::string> in_files (args.size ());
			if (args.size () == 0)
				in_files.push_back ("-");
			else for (unsigned i = 0; i < args.size (); ++i)
				in_files[i] = args[i];
	
			assemble (in_files);
			if (!errors)
			{
				if (useobject)
				{
					std::string script ("#\xfeof\0\0\0\0", 8), code;
					for (std::list <Block>::iterator i = blocks.begin (); i != blocks.end (); ++i)
						i->write_object (script, code);
					*outfile << script << '\0' << code << std::flush;
				}
				else
				{
					errors = 0;
					for (std::list <Block>::iterator i = blocks.begin (); i != blocks.end (); ++i)
						i->lock ();
					if (!errors)
					{
						for (std::list <Block>::iterator i = blocks.begin (); i != blocks.end (); ++i)
							i->write_binary ();
						if (!errors && listfile)
						{
							*listfile << "\n# List of labels:\n";
							for (std::list <Label>::iterator l = labels.begin (); l != labels.end (); ++l)
								*listfile << l->name << ":\tequ 0x" << l->value.compute (Expr::valid_int (">>")).value << '\n';
						}
					}
				}
				if (!errors && !useobject && usehex)
					hexfile.write_s19 (*outfile);
			}
		}
		else
		{
			if (defs.empty ())
				std::cerr << "Error: cannot disassemble without a definitions file\n";
			else if (args.size () > 1)
				std::cerr << "Error: cannot disassemble multiple files\n";
			else if (args.size () == 0)
				disassemble (std::cin);
			else
			{
				std::ifstream f (args[0].c_str ());
				if (!f)
					std::cerr << "Error: cannot open input file " << args[0] << '\n';
				else
					disassemble (f);
			}
		}
	}
	if (errors)
	{
		if (!outfilename.empty ())
			unlink (outfilename.c_str ());
	}

	if (!outfilename.empty ())
		delete outfile;
	if (listfile && listfile != &std::cout)
		delete listfile;
	return 0;
}
