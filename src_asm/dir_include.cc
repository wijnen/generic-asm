#include "asm.hh"
#include <fstream>

void dir_include (shevek::istring &args, bool write, bool first, Label *current_label)
{
	(void)current_label;
	if (write && listfile)
		*listfile << "\t\t\t";
	if (!first)
		return;
	std::string name = read_filename (args);
	if (name.empty ())
		return;
	std::string fullname = input_stack.top ().basedir + '/' + name;
	std::ifstream *file = new std::ifstream (fullname.c_str ());
	if (*file)
	{
		std::string base = input_stack.top ().basedir;
		input_stack.push (Input ());
		input_stack.top ().basedir = base;
	}
	else
	{
		delete file;
		file = NULL;
		for (std::list <std::string>::const_iterator
				i = include_path.begin ();
				i != include_path.end (); ++i)
		{
			fullname = *i + '/' + name;
			file = new std::ifstream (fullname.c_str ());
			if (*file)
			{
				input_stack.push (Input ());
				input_stack.top ().basedir = *i;
				break;
			}
			delete file;
			file = NULL;
		}
		if (!file)
		{
			error (shevek::ostring ("unable to open %s",
						Glib::ustring (name)));
			return;
		}
	}
	input_stack.top ().name = fullname;
	input_stack.top ().ln = 0;
	input_stack.top ().type = Input::FILE;
	input_stack.top ().must_delete = true;
	input_stack.top ().file = file;
}
