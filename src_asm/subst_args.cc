#include "asm.hh"

Glib::ustring subst_args (Glib::ustring const &orig,
		std::vector <std::pair <Glib::ustring, Glib::ustring> > const &args)
{
	Glib::ustring ret;
	Glib::ustring::size_type pos = 0;
	for (unsigned i = 0; i < args.size (); ++i)
	{
		while (true)
		{
			Glib::ustring::size_type
				p = orig.find (args[i].first, pos);
			if (p == Glib::ustring::npos)
				break;
			ret += orig.substr (pos, p - pos);
			ret += args[i].second;
			pos = p + args[i].first.size ();
		}
	}
	return ret + orig.substr (pos);
}
