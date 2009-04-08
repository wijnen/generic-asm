#include "asm.hh"

std::string read_filename (shevek::ristring &args)
{
	char quote;
	if (!args (" %c", quote))
	{
		error ("include without filename");
		return std::string ();
	}
	std::string rest = args.rest ();
	std::string::size_type pos = rest.find (quote);
	if (pos == std::string::npos)
	{
		error ("no matching quote for filename");
		return std::string ();
	}
	if (pos == 0)
	{
		error ("empty name");
		return std::string ();
	}
	std::string name = args.rest ().substr (0, pos);
	args.skip (pos + 1);
	return name;
}
