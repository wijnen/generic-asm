#include "asm.hh"

void dir_if (shevek::ristring &args, bool first, std::list <Label>::iterator current_label)
{
	(void)first;
	(void)current_label;
	std::string::size_type pos = 0;
	Expr e = Expr::read (args.rest (), false, pos);
	if (pos == std::string::npos)
	{
		error ("incorrect expression in if");
		return;
	}
	args.skip (pos);
	blocks.back ().parts.push_back (Block::Part ());
	blocks.back ().parts.back ().type = Block::Part::IF;
	blocks.back ().parts.back ().expr = e;
	if (writing && listfile)
		*listfile << "\t\t\t";
}
