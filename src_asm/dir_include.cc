#include "asm.hh"
#include <fstream>

unsigned dir_include (shevek::istring &args, bool write, Label *current_label)
{
	(void)current_label;
	(void)write;
	std::ifstream *file = new std::ifstream (args.rest ().c_str ());
	if (!*file)
	{
		delete file;
		error (shevek::ostring ("unable to open %s", args.rest ()));
		return 0;
	}
	input_stack.push (Input ());
	input_stack.top ().name = args.rest ();
	input_stack.top ().ln = 0;
	input_stack.top ().type = Input::FILE;
	input_stack.top ().must_delete = true;
	input_stack.top ().file = file;
	return 0;
}
