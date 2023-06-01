#ifndef DATATYPES_H
#define DATATYPES_H

#include <string>
#include <vector>
#include <sstream>
#include <map>
#include <algorithm>

struct fileData {
    std::string name;
    std::string content;
};

struct compilationError {
    std::string type;
    std::string message;
    int line;
};

struct tokenMatrix {
    int lines;
    std::vector<std::vector<std::string>> matrix;
};

struct equIdentifier {
    std::string name;
    int value;
};

#endif