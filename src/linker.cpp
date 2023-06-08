#include "../include/linker.hpp"

void Link(std::vector<tokenMatrix> &input_matrixes, fileData *output_file){
    std::vector<objectData> modules(input_matrixes.size());
    std::map<std::string, int> correction_factor;
    std::vector<symbolData> global_definition_table;


    SeparateMatrixes(input_matrixes, modules);

    if(input_matrixes.size() == 1){
        ConvertArrayObjectToFile(modules.begin()->assembled_code, output_file);
    }
    else{
        for(auto current_module : modules){
            // percorrer a definition_table de current_module e colocar todos os símbolos do vetor em globa_defintion_table
            for(auto current_symbol : current_module.definition_table){
                global_definition_table.insert(global_definition_table.end(), {.name = current_symbol.name, .value = current_symbol.value});
            }
        }

        // Ligar
    }
}

void SeparateMatrixes(std::vector<tokenMatrix> &input_matrixes, std::vector<objectData> &modules){
    std::vector<std::string> matrix_line;
    tokenMatrix current_matrix;
    int index = 0;

    for(auto current_matrix : input_matrixes){
        std::vector<symbolData> use_table;
        std::vector<symbolData> definition_table;
        std::vector<std::string> relative_table;
        std::vector<std::string> assembled_code;
        bool useSection = false;
        bool defSection = false;
        bool relSection = false;
        bool codeSection = false;

        for (int i = 0; i < current_matrix.matrix.size(); i++){
            matrix_line = current_matrix.matrix[i];

            if(matrix_line.size() == 1 && matrix_line[0].compare("USO") == 0){
                useSection = true;
            }
            else if(matrix_line.size() == 1 && matrix_line[0].compare("DEF") == 0){
                useSection = false;
                defSection = true;
            }
            else if(matrix_line.size() == 1 && matrix_line[0].compare("RELATIVOS") == 0){
                defSection = false;
                relSection = true;
            }
            else if(matrix_line.size() == 1 && matrix_line[0].compare("CODE") == 0){
                relSection = false;
                codeSection = true;
            }
            else if(useSection){
                use_table.insert(use_table.end(), {.name = matrix_line[0],.value = std::stoi(matrix_line[1])});
            }
            else if(defSection){
                definition_table.insert(definition_table.end(), {.name = matrix_line[0],.value = std::stoi(matrix_line[1])});
            }
            else if(relSection){
                for (int j = 0; j < matrix_line.size(); j++){
                    relative_table.insert(relative_table.end(), matrix_line[j]);                    
                }
            }
            else if(codeSection){
                for (int j = 0; j < matrix_line.size(); j++){
                    assembled_code.insert(assembled_code.end(), matrix_line[j]);                    
                }
            }

        }

        modules.at(index).index = index;
        modules.at(index).use_table = use_table;
        modules.at(index).definition_table = definition_table;
        modules.at(index).relative_table = relative_table;
        modules.at(index).assembled_code = assembled_code;
        
        index++;
    }
}