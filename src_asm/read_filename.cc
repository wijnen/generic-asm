#include "asm.hh"

std::string read_filename (shevek::istring &args)
{
	char quote;
	if (!args (" %c", quote))
	{
		error ("include without filename");
		return std::string ();
	}
	Glib::ustring rest = args.rest ();
	Glib::ustring::size_type pos = rest.find (quote);
	if (pos == Glib::ustring::npos)
	{
		error ("no matching quote for filename");
		return std::string ();
	}
	if (pos == 0)
	{
		error ("empty name");
		return std::string ();
	}
	Glib::ustring name = args.rest ().substr (0, pos);
	args.skip (pos + 1);
	return filename_from_utf8 (name);
}
