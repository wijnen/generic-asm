#include "asm.hh"

void dir_endif (shevek::ristring &args, bool first, std::list <Label>::iterator current_label)
{
	(void)first;
	(void)args;
	(void)current_label;
	blocks.back ().parts.push_back (Block::Part ());
	blocks.back ().parts.back ().type = Block::Part::ENDIF;
	if (writing && listfile)
		*listfile << "\t\t\t";
}
