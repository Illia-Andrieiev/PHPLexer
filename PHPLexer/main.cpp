#include <iostream>
#include <fstream>
#include <sstream>
#include "lexer.hpp" 

int main(int argc, char* argv[]) {
    // Check if the user provided a file name as a command-line argument
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <php_file>\n";
        return 1; // Exit with error code
    }

    // Get the file name from the command-line argument
    const std::string filename = argv[1];

    // Try to open the specified file
    std::ifstream in(filename);
    if (!in) {
        std::cerr << "Cannot open input file: " << filename << "\n";
        return 1; // Exit with error if file cannot be opened
    }

    // Read the entire contents of the file into a string
    std::stringstream buffer;
    buffer << in.rdbuf();

    // Create a lexer with the file content
    Lexer lexer(buffer.str());

    // Run the tokenizer to process the input
    lexer.tokenize();

    // Print the resulting tokens to the console
    lexer.getTokens().print();

    return 0; // Successful execution
}
