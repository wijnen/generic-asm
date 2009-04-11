#include "asm.hh"

void write_expr (Expr &e)
{
	if (!writing)
		return;
	if (files.back ().blocks.empty ())
		files.back ().blocks.push_back (File::Block ());
	if (e.type == Expr::NUM)
	{
		Expr::valid_int v = e.value;
		if (!v.valid)
			error ("invalid value in expression");
		if (v.value < -0x80 || v.value > 0xff)
			error ("output value out of range");
		if (files.back ().blocks.back ().parts.empty () || files.back ().blocks.back ().parts.back ().type != File::Block::Part::CODE)
		{
			files.back ().blocks.back ().parts.push_back (File::Block::Part ());
			files.back ().blocks.back ().parts.back ().type = File::Block::Part::CODE;
			files.back ().blocks.back ().parts.back ().have_expr = false;
		}
		files.back ().blocks.back ().parts.back ().name += (char)v.value;
	}
	else
	{
		files.back ().blocks.back ().parts.push_back (File::Block::Part ());
		files.back ().blocks.back ().parts.back ().type = File::Block::Part::BYTE;
		files.back ().blocks.back ().parts.back ().have_expr = true;
		files.back ().blocks.back ().parts.back ().expr = e;
	}
}
