#include "asm.hh"

std::string subst_args (std::string const &orig,
		std::vector <std::pair <std::string, std::string> > const &args)
{
	std::string ret;
	std::string::size_type pos = 0;
	for (unsigned i = 0; i < args.size (); ++i)
	{
		while (true)
		{
			std::string::size_type p = orig.find (args[i].first, pos);
			if (p == std::string::npos)
				break;
			ret += orig.substr (pos, p - pos);
			ret += args[i].second;
			pos = p + args[i].first.size ();
		}
	}
	return ret + orig.substr (pos);
}
