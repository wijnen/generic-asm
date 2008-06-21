#include "asm.hh"

bool getline (std::string &ret)
{
	while (true)
	{
		switch (input_stack.top ().type)
		{
		case Input::FILE:
			if (!std::getline (*input_stack.top ().file, ret))
				return false;
			break;
		case Input::MACRO:
			if (input_stack.top ().ln >= input_stack.top ()
					.macro->code.size ())
				return false;
			ret = input_stack.top ().macro->code
				[input_stack.top ().ln];
			ret = subst_args (ret, input_stack.top ().macro->args);
			break;
		default:
			error ("Bug: invalid case reached");
			return false;
		}
		++input_stack.top ().ln;
		if (!ret.empty ())
			break;
	}
	return true;
}
