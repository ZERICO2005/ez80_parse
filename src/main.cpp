#include <stdio.h>

#include <iostream>
#include <sstream>
#include <fstream>

#include "common_std.h"
#include "ez80_asm.h"
#include "ez80_parse.h"

void load_asm_file(string& output, const char* input_path) {
    std::ifstream file(input_path);
    if (file) {
        // Read the file content into the string
        output.assign((std::istreambuf_iterator<char>(file)),
                            std::istreambuf_iterator<char>());
        file.close(); // Close the file
    } else {
        std::cerr << "Error opening file: " << input_path << std::endl;
    }
}

int main(void) {
	char const * const input_path = "../input/input_asm.h";
	printf("asm parse\n");
	string input_asm;
	load_asm_file(input_asm, input_path);
	if (input_asm.empty() || input_asm.size() < 10) {
		printf("Error: \"%s\" is empty\n", input_path);
		return 0;
	}
	vector<asm_line> asm_text;
	parse_asm(input_asm, asm_text);
	printf("finished\n");
	return 0;
}
