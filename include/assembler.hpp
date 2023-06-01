#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include "assemblerMaps.hpp"
#include "datatypes.hpp"
#include "utils.hpp"

void Assembler(fileData *input_file, fileData *output_file);
void TranslateAssemblyToObject(tokenMatrix *input_matrix);

#endif
