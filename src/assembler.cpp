#include "../include/assembler.hpp"

void Assembler(fileData *input_file, fileData *output_file){
    
    tokenMatrix *input_matrix = new tokenMatrix{.lines = 0};
    std::vector<std::string> output_object;

    ConvertFileToMatrix(input_file, input_matrix);
    TranslateAssemblyToObject(input_file, input_matrix, output_object);
    ConvertArrayObjectToFile(output_object, output_file);

    delete input_matrix;
}

void TranslateAssemblyToObject(fileData *input_file,tokenMatrix *input_matrix, std::vector<std::string> &output_object){
    std::vector<std::string> matrix_line;
    std::vector<symbolData> symbol_table;
    std::string symbol_clean_name; // usado para guardar o rotulo sem ':'

    int symbol_address = 0;
    int current_address = 0;
    int operand_quantity = 0; // usada para contar os operandos
    int value = 0; // usada para auxiliar na contagem da posição do operando
    int current_line_address = 0;
    int current_line_size = 0;

    int opcode;
    std::vector<int> list_aux; // usada para auxiliar na criação da lista de dependencias

    bool has_section_text = false;
    int line_label = 0;

    bool label_duplicated = false;
    int error_line_duplicated = 0;
    long int const_number = 0;
    bool hasBegin = false;
    bool hasEnd = false;
    std::string section="TEXT";

    bool error_line_without_begin_end = false;
    bool error_lexic = false;

    for(int i = 0; i < input_matrix->matrix.size(); i++){
        matrix_line = input_matrix->matrix[i];
        operand_quantity = 0;
        line_label = 0;
        error_line_without_begin_end = false;
        bool error_lexic = false;
        bool label_duplicated = false;

        for(int j = 0; j < matrix_line.size(); j++){

            if(matrix_line[j] == "EXTERN" || matrix_line[j] == "PUBLIC"){
                if(!hasBegin && !hasEnd){
                    error_line_without_begin_end = true;
                }
            }

            if(matrix_line[j] == "BEGIN" || matrix_line[j] == "END"){
                hasBegin = true;
                hasEnd= true;
            }
                        
            if(isLabel(matrix_line[j])){
                line_label++;
                symbol_clean_name = matrix_line[j];
                symbol_clean_name.erase(remove(symbol_clean_name.begin(), symbol_clean_name.end(), ':'), symbol_clean_name.end());
                
                if(hasLexicError(matrix_line[j])){
                    error_lexic = true;
                }

                if(isSymbolOnSymbolTable(symbol_table, symbol_clean_name) != -1){
                    if(isSymbolDefined(symbol_table, symbol_clean_name)){
                        printf("[Arquivo %s] ERRO SEMÂNTICO: rótulo duplicado (linha %d)\n", input_file->name.c_str(), i + 1);
                    }
                    else{
                        
                        symbol_address = isSymbolOnSymbolTable(symbol_table, symbol_clean_name);
                        symbol_table[symbol_address].is_defined = true;
                        symbol_table[symbol_address].value = current_line_address;
                    
                        for(int l = 0; l < symbol_table[symbol_address].list_of_dependencies.size(); l++){
                            output_object[symbol_table[symbol_address].list_of_dependencies[l]] = std::to_string(symbol_table[symbol_address].value);
                        }
                    }
                }else{
                    
                    insertOnSymbolTable(symbol_table, {.name = symbol_clean_name,.value =  current_line_address, .is_defined= true, .line = (i+1)});
                }                
            }
            else if(isOperator(matrix_line[j])){ 
                operand_quantity++;
                value = current_line_address + operand_quantity;

                if(hasLexicError(matrix_line[j])){
                    error_lexic = true;
                }
                
                if(isSymbolOnSymbolTable(symbol_table, matrix_line[j]) != -1){
                    if(isSymbolDefined(symbol_table, matrix_line[j])){
                        symbol_address = isSymbolOnSymbolTable(symbol_table, symbol_clean_name);
                        output_object.insert(output_object.end(), std::to_string(symbol_table[symbol_address].value));
                    }
                    else {
                        symbol_address = isSymbolOnSymbolTable(symbol_table, matrix_line[j]);
                        insertOnListOfDependecies(symbol_table, symbol_address, value);
                        output_object.insert(output_object.end(), matrix_line[j]);
                    }
                }
                else {
                    list_aux.clear();
                    list_aux.insert(list_aux.end(), value);
                    insertOnSymbolTable(symbol_table, {.name = matrix_line[j],.value =  -1, .is_defined= false,.list_of_dependencies = list_aux, .line = (i+1), .section=section});
                    output_object.insert(output_object.end(), matrix_line[j]);
                }
            }
            else if(isInstructionOrDirective(matrix_line[j])){
                current_line_size += op_size_map.find(matrix_line[j])->second;;

                if((matrix_line[j].compare("CONST") != 0) && (matrix_line[j].compare("SPACE") != 0)){
                    output_object.insert(output_object.end(), op_code_map.find(matrix_line[j])->second);
                }

                if((matrix_line[j].compare("CONST") == 0)){
                    if(matrix_line.size() > j + 1) {
                        if(isLabel(matrix_line[j+1])){
                            error_line_duplicated = true;
                        }else{
                            if(isNumber(matrix_line[j+1])){
                                const_number = toNumber(matrix_line[j+1]);
                                output_object.insert(output_object.end(), std::to_string(const_number));
                            }
                        }
                    }
                }

                if((matrix_line[j].compare("SPACE") == 0)){
                    if(matrix_line.size() > j + 1) {
                        if(isLabel(matrix_line[j+1])){
                            error_line_duplicated = true;
                        }else{
                            for(int m = 0; m < stol(matrix_line[j+1]); m++){
                                output_object.insert(output_object.end(), "0");
                            }
                        }
                    }
                    else{
                        output_object.insert(output_object.end(), "0");
                    }
                }
            }

            else{
                if(matrix_line[j] == "TEXT"){
                    section = "TEXT";
                    has_section_text = true;
                }else if(matrix_line[j] == "DATA"){
                    section = "DATA";
                }
            }
        }

        current_line_address += current_line_size;
        current_line_size = 0;

        if(line_label > 1){
            printf("[Arquivo %s] ERRO SEMÂNTICO: dois rótulos na mesma linha (linha %d)\n",input_file->name.c_str(), i + 1 );
        }

        if(error_line_without_begin_end){
            printf("[Arquivo %s] ERRO SEMÂNTICO: EXTERN ou PUBLIC declarados em um arquivo sem BEGIN e END (linha %d)\n",input_file->name.c_str(),i+1);
        }

        if(error_lexic){
             printf("[Arquivo %s] ERRO LÉXICO: tokens não podem começar com digitos ou underscore (linha %d)\n",input_file->name.c_str(), i + 1);
        }

    }

    if(!has_section_text){
        printf("[Arquivo %s] ERRO SINTÁTICO: SECTION TEXT ausente\n",input_file->name.c_str());
    }

    for(int i = 0; i < symbol_table.size(); i++){
        if(!symbol_table[i].is_defined){            
            if(symbol_table[i].section == "DATA"){
                printf("[Arquivo %s] ERRO SEMÂNTICO: dado indefinido(linha %d)\n ",input_file->name.c_str(), symbol_table[i].line);
            }
            if(symbol_table[i].section == "TEXT"){
                printf("[Arquivo %s] ERRO SEMÂNTICO: rótulo indefinido(linha %d)\n ",input_file->name.c_str(), symbol_table[i].line);
            }            
        }
    }

}
