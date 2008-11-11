#include "asm.hh"

bool getline (Glib::ustring &ret)
{
	while (!input_stack.empty ())
	{
		switch (input_stack.top ().type)
		{
		case Input::FILE:
		{
			std::string line;
			if (!std::getline (*input_stack.top ().file, line))
			{
				if (input_stack.top ().must_delete)
					delete input_stack.top ().file;
				input_stack.pop ();
				continue;
			}
			ret = line;
			break;
		}
		case Input::MACRO:
			if (input_stack.top ().ln >= input_stack.top ()
					.macro->code.size ())
			{
				input_stack.pop ();
				continue;
			}
			ret = input_stack.top ().macro->code
				[input_stack.top ().ln];
			ret = subst_args (ret, input_stack.top ().macro->args);
			break;
		default:
			error ("Bug: invalid case reached");
			return false;
		}
		++input_stack.top ().ln;
		break;
	}
	return !input_stack.empty ();
}
