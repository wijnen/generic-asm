#include "asm.hh"

void write_expr (Expr &e)
{
	if (!writing)
		return;
	if (e.type == Expr::NUM)
	{
		Expr::valid_int v = e.value;
		if (!v.valid)
			error ("invalid value in expression");
		if (v.value < -0x80 || v.value > 0xff)
			error ("output value out of range");
		if (blocks.back ().parts.empty () || blocks.back ().parts.back ().type != Block::Part::CODE)
		{
			blocks.back ().parts.push_back (Block::Part ());
			blocks.back ().parts.back ().type = Block::Part::CODE;
			blocks.back ().parts.back ().have_expr = false;
		}
		blocks.back ().parts.back ().name += (char)v.value;
		if (listfile)
			*listfile << ' ' << std::setfill ('0') << std::setw (2) << (v.value & 0xff);
	}
	else
	{
		blocks.back ().parts.push_back (Block::Part ());
		blocks.back ().parts.back ().type = Block::Part::BYTE;
		blocks.back ().parts.back ().have_expr = true;
		blocks.back ().parts.back ().expr = e;
		if (listfile)
			*listfile << " xx";
	}
}
