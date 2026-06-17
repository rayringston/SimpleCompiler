#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <regex>

#include "lexer.h"
#include "parser.h"

using namespace std;

int main(int argc, char* argv[]) { // compiler <fileName> -o <outputName>
	cout << "<----- Simple Compiler ----->" << endl;
	if (argc < 2) {
		cerr << "Error: you need to input a file to compile\n";
		cerr << "./compiler <filename>" << endl;
		return 1;
	}

	if (string(argv[1]) == "-h" || string(argv[1]) == "--help") {
	
		cout << "Usage: ./compiler <input-file> <output-file.s>" << endl << endl;
		
		cout << "Options:" << endl;
		cout << "\t--help [-h]: Prints this message." << endl;
		cout << "\t--verbose [-v]: Displays compilation status, including lexing and parsing." << endl << endl;

		cout << "Made by Ray Ringston." << endl;

		return 0;
	}

	string filename = argv[1];
	ifstream sourceFile(filename);

	if (!sourceFile.is_open()) {
		cerr << "Error unable to open file: " << filename << endl;
		return 1;
	}

	regex pattern(R"(.*\.(s)$)");

	string outFilePath = "out.s";
	cmatch cm;

	// saves the stream buffer used for the output, needed incase verbose output is enabled
	streambuf* streamBuffer = cout.rdbuf();

	if (argc >= 3) {
		if (regex_match(argv[2], cm, pattern)) {
			outFilePath = argv[2];
		} else {
			cout << argv[2] << " is not a valid file name. Outputting to /out.s" << endl;
		}
		
		int verbose = 0;
		for (int i = 3; i < argc; i++) {
			if (string(argv[i]) == "--verbose" || string(argv[i]) == "-v") { 
				verbose = 1;
			}
		}

		if (verbose == 0) {
			cout.rdbuf(nullptr); // turns off the output buffer
		}
	}


	ostringstream ss;
	ss << sourceFile.rdbuf();
	string source = ss.str();

	Lexer lexer(source);
	Emitter emitter(outFilePath);

	Parser parser(lexer, emitter);

	parser.program();
	emitter.writeFile();

	cout.rdbuf(streamBuffer);

	cout << "Compilation successful." << endl;

	sourceFile.close();
	return 0;
}

// --------------- Syntax Rules -----------------
/*

- to define variables: 	INT [name] = [value]
			FLOAT [name] = [value].[fraction]
			TEXT [name] = "[value]"

TBC
*/
