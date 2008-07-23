#include "asm.hh"

std::ostream *outfile, *listfile;
bool usehex;
Hex hexfile;
unsigned addr;
unsigned errors;

std::stack <Input> input_stack;
std::list <std::pair <unsigned, std::string> > *current_stack;

std::map <std::string, Param> params;
std::list <Label> labels;
std::list <Source> sources;
std::list <DefsMacro> defs_macros;
std::list <std::string> include_path;
