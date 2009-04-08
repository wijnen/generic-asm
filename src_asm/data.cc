#include "asm.hh"

std::ostream *outfile, *listfile;
bool usehex;
Hex hexfile;
unsigned addr;
bool absolute_addr;
unsigned errors;
unsigned undefined_locals;
bool writing;
bool first_pass;
bool report_labels;

std::stack <Input> input_stack;
std::list <std::pair <unsigned, std::string> > *current_stack;

std::list <Param> params;
std::list <Label> labels;
std::list <Source> sources;
std::list <DefsMacro> defs_macros;
std::list <std::string> include_path;
std::list <File> files;
std::list <Space> spaces;
