#include "asm.hh"

std::ostream *outfile, *listfile;
bool usehex;
Hex hexfile;
unsigned addr;
unsigned errors;

std::stack <Input> input_stack;
std::list <std::pair <unsigned, Glib::ustring> > *current_stack;

std::map <Glib::ustring, Param> params;
std::list <Label> labels;
std::list <Source> sources;
std::list <DefsMacro> defs_macros;
std::list <std::string> include_path;
