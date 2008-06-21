#include "asm.hh"

std::string subst_args (std::string const &orig,
		std::vector <std::pair <std::string, std::string> > const &args)
{
	std::string o = orig;
	for (unsigned i = 0; i < args.size (); ++i)
	{
		std::string ret;
		std::string::size_type pos = 0;
		while (true)
		{
			std::string::size_type
				p = orig.find (args[i].first, pos);
			if (p == std::string::npos)
				break;
			ret += o.substr (pos, p - pos);
			ret += args[i].second;
			pos = p + args[i].first.size ();
		}
		o = ret + o.substr (pos);
	}
	return o;
}
