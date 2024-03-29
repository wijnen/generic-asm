#include "asm.hh"

std::ostream *outfile, *listfile;
bool usehex;
bool use_words;
Hex hexfile;
unsigned addr;
bool absolute_addr;
unsigned errors;
unsigned undefined_locals;
bool writing;
unsigned stage;
bool report_labels;

std::stack <Input> input_stack;
std::list <std::pair <unsigned, std::string> > *current_stack;

std::list <Param> params;
std::list <Label> labels;
std::list <Source> sources;
std::list <DefsMacro> defs_macros;
std::list <std::string> include_path;
std::list <Block> blocks;
std::list <Space> spaces;
