#include "asm.hh"

unsigned addr;
unsigned errors;

std::stack <Input> input_stack;

std::map <std::string, Param> params;
std::list <Label> labels;
std::list <Source> sources;
std::list <DefsMacro> defs_macros;
