#include "lexer.h"
#include "emitter.h"
#include <vector>
#include <algorithm>
#include <utility>

using namespace std;

struct functionParams {
	string name;
	vector<pair<string, TOKEN_TYPE>> params;
};

class FunctionMap {
	public:
		FunctionMap() {}
		
		int exists(string name) { // helper function to figure out if an entry exists
			for (auto function : functions) {
				if (function.first == name) return 1;
			}
			return 0;
		}

		TOKEN_TYPE getReturnType(string name) { // helper function to get the return type of a function
			for (auto function : functions) {
				if (function.first == name) return function.second;
			}
			return TOKEN_TYPE::INVALID; // should never get here if you check for existence first
		}

		int paramExists(string name, string param, TOKEN_TYPE type) { // check to see a param is already defined -- maybe not useful
			for (int i = 0; i < paramMap.size(); i++) {
				if (paramMap[i].name == name) {
					for (int j = 0; j < paramMap[i].params.size(); j++) {
						if (paramMap[i].params[j].first == param && paramMap[i].params[j].second == type) return 1;
					}
				}
			}
			return 0;
		}

		/*	TOP	  BOT
		 *	
		 *	[] [] lr fp|sp	2 params, 
		 *	0  1
		 *	3  2
		 *	24 16 8  0
		 *
		 *	[] [] [] [-] lr fp|sp
		 *	0  1  2
		 *	5  4  3
		 *	40 32 24 16  8  0
		 */

		int getParamIdx(vector<pair<string, TOKEN_TYPE>> params, string param) { // helper function to get the index of a parameter in the function's parameter list
			int i = 0;
			for (auto p : params) {
				if (p.first == param) return i;
				i++;
			}
			return -1; // should never get here if you check for existence first
		}
		
		string getParamOffset(vector<pair<string, TOKEN_TYPE>> params, string param) {
			int idx = getParamIdx(params, param);

			int posFromBack;
				
			if (params.size() % 2 != 0) { 
				posFromBack = params.size() - idx + 2;
			} else { // for an odd number of params, the offset will be 16 aligned, so there's 8 bytes in padding
				posFromBack = params.size() - idx + 1;
			}

			posFromBack *= 8; // 8 bytes per element

			return "#" + to_string(posFromBack);
		}

		string getLabel(string name) { 	// helper function to get the label for each function
			int idx = getParamIdx(functions, name); // works the same way

			return "FUNC" + idx;
		}

		vector<pair<string, TOKEN_TYPE>> getParams(string name) { // return the params listed under a function label
			for (int i = 0; i < paramMap.size(); i++) {
				if (paramMap[i].name == name) return paramMap[i].params;
			}
			return {};
		}

		void push_name(string name, TOKEN_TYPE type) { // should be called before push_back 
			functions.push_back({name, type});
		}

		void push_back(string name, vector<pair<string, TOKEN_TYPE>> params = {}) {
			functionParams entry;
			entry.name = name;
			entry.params = params;

			paramMap.push_back(entry);
		}

		vector<functionParams> paramMap;
		vector<pair<string, TOKEN_TYPE>> functions;
};

class SymbolMap {
	public:
		SymbolMap() {
			fill(registers.begin(), registers.end(), "");

			// X0		: returns from functions
			// X1-X7	: used for parameters
			// X8		: NO. something already
			// X9 - X15	: used in functions
			// X16-X18	: NO. also something already
			// X19 - X28 	: variable storage
			// X29		: FP
			// X30		: SP
			// X31		: 0
		}

		TOKEN_TYPE getType(string name) {
			for (auto symbol : symbols) {
				if (symbol.first == name) return symbol.second;
			}
			return TOKEN_TYPE::INVALID;
		}


		string getLabel(string name) {
			TOKEN_TYPE type = getType(name);
	
			int index = find(symbols.begin(), symbols.end(), pair{name, type}) - symbols.begin();

			return "V" + to_string(index);
		}

		string getLabel(int index) {
			return "V" + to_string(index);
		}
		
		int exists(string name) {
			TOKEN_TYPE type = getType(name);
			if (type == TOKEN_TYPE::INVALID) return 0;

			if (find(symbols.begin(), symbols.end(), pair{name, type}) == symbols.end()) return 0; // doesn't exist
			else return 1; // exists
		}

		void pushBack(string name, TOKEN_TYPE type) {
			symbols.push_back({name, type});
		}

		int size() {
			return symbols.size();
		}
/*
		int getRegister(string identifier) {
			int idx = find(registers.begin(), registers.end(), identifier)
			if (idx == registers.end()) {
				return -1; // -1 means the variable isnt in a register, and you have to go to memory
			}

			return idx - registers.begin(); // returns the number for the register
		}
	
		int pushVariable(string identifier) {
			
		}
*/
		vector<pair<string, TOKEN_TYPE>> symbols; // symbols contains a pair of the name of every symbol used, and it's type
		vector<string> registers; // 
};

class Parser {
	public:
		Parser(Lexer& inputLexer, Emitter& inputEmitter);
		void abort(string message);
		int checkToken(TOKEN_TYPE kind);
		int checkPeek(TOKEN_TYPE kind);
		void nextToken();
		void match(TOKEN_TYPE kind);
		// Sytanx function declarations
		void program();
		void statement(TOKEN_TYPE caller = TOKEN_TYPE::INVALID, vector<pair<string, TOKEN_TYPE>> parameters = {});
		void nl();
		TOKEN_TYPE expression(TOKEN_TYPE caller = TOKEN_TYPE::INVALID, vector<pair<string, TOKEN_TYPE>> parameters = {});
		TOKEN_TYPE term(TOKEN_TYPE caller = TOKEN_TYPE::INVALID, vector<pair<string, TOKEN_TYPE>> parameters = {});
		TOKEN_TYPE unary(TOKEN_TYPE caller = TOKEN_TYPE::INVALID, vector<pair<string, TOKEN_TYPE>> parameters = {});
		TOKEN_TYPE primary(TOKEN_TYPE caller = TOKEN_TYPE::INVALID, vector<pair<string, TOKEN_TYPE>> parameters = {});
		void condition(string exitLabel, TOKEN_TYPE caller = TOKEN_TYPE::INVALID, vector<pair<string, TOKEN_TYPE>> parameters = {});
		TOKEN_TYPE functionCall(TOKEN_TYPE caller = TOKEN_TYPE::INVALID, vector<pair<string, TOKEN_TYPE>> parameters = {});

		Lexer& lexer;
		Emitter& emitter;
		Token curToken;
		Token peekToken;

		SymbolMap symbolMap;
		FunctionMap functionMap;

		vector <string> labels;
		vector <string> gotos;
		vector <string> stringLiterals;
		vector <string> functions;
		

		int ifCount;
		int whileCount;

//		vector<int> registerFile(32, 0);
};

Parser::Parser(Lexer& inputLexer, Emitter& inputEmitter) : lexer(inputLexer), emitter(inputEmitter) {
	ifCount = 0;
	whileCount = 0;

	nextToken();
	nextToken();
}

// Returns 1 when the current token matches the expected type
int Parser::checkToken(TOKEN_TYPE kind) {
	if (kind == curToken.type) return 1;
	else return 0;
}

// Returns 1 when the peeked token matches the expected type
int Parser::checkPeek(TOKEN_TYPE kind) {
	if (kind == peekToken.type) return 1;
	else return 0;
}

// Gets the next token for peeked, moves the peeked value to the cur value
void Parser::nextToken() {
	curToken = peekToken;
	peekToken = lexer.getToken();
}

// Exit message
void Parser::abort(string message) {
	cerr << "Error (PARSER):\n";
	cerr << message << endl;
	exit(1);
}

// Combines nextToken and checkToken, and aborts on failure
void Parser::match(TOKEN_TYPE kind) {
	if (checkToken(kind) != 1) {
		abort("Expected " + tokenTypeToString(kind) + ", got " + tokenTypeToString(curToken.type));
	}
	nextToken();
}


TOKEN_TYPE Parser::functionCall(TOKEN_TYPE caller, vector<pair<string, TOKEN_TYPE>> parameters) {
	if (!functionMap.exists(curToken.text)) {
		abort("Function " + curToken.text + " does not exist");
	}

	string branchIdentifier = curToken.text;
	string bLabel = functionMap.getLabel(curToken.text);
	TOKEN_TYPE returnType = functionMap.getReturnType(branchIdentifier);
	int paramCount = 0;

	cout << " (" + branchIdentifier + ")";
	match(TOKEN_TYPE::IDENTIFIER);

	if (checkToken(TOKEN_TYPE::WITH)) {
		cout << "\nFUNCTIONCALL-PARAMETERS\n";
		nextToken();
		paramCount = 1;

		expression(caller, parameters);
		emitter.emitLine("str x11, [sp, #-8]!", caller);

		while (checkToken(TOKEN_TYPE::COMMA)) {
			match(TOKEN_TYPE::COMMA);
			paramCount++;

			expression(caller, parameters);
			emitter.emitLine("str x11, [sp, #-8]!", caller);
		}

		if (paramCount % 2 != 0) {
			emitter.emitLine("sub sp, sp, #8", caller);
		}
	}

	int expectedParams = functionMap.getParams(branchIdentifier).size();
	if (paramCount != expectedParams) {
		abort("Function (" + branchIdentifier + ") expects " + to_string(expectedParams) + " parameters, only recieved " + to_string(paramCount));
	}

	emitter.emitLine("bl " + bLabel, caller);
	return returnType;
}

// --------------- SYNTAX FUNCTIONS

// Program is made of statements. Do each one until you reach the end
void Parser::program() {
	cout << "PROGRAM\n";

	emitter.headerLine(".global _start");
	emitter.headerLine(".text");
	emitter.headerLine("\n_start:");

	while (checkToken(TOKEN_TYPE::NEWLINE) == 1) nextToken();

	while (checkToken(TOKEN_TYPE::END) != 1) {
		statement();
	}

	for (auto g : gotos) {
		if (find(labels.begin(), labels.end(), g) == labels.end()) {
			abort("Attemping to GOTO undeclared label, " + g);
		}
	}

	for (int i = 0; i < symbolMap.size(); i++) {
		emitter.dataLine(symbolMap.getLabel(i) + ": .quad 0");
	}

	for (int i = 0; i < stringLiterals.size(); i++) {
		string label = "S" + to_string(i);
		emitter.dataLine(label + ": .asciz \"" + stringLiterals[i] + "\"");
		emitter.dataLine(label + "_len = . - " + label);
	}

	emitter.emitLine("bl exit");
}

void Parser::statement(TOKEN_TYPE caller, vector<pair<string, TOKEN_TYPE>> parameters) {
	if (checkToken(TOKEN_TYPE::PRINT)) { // PRINT ( STRING | expression )
		cout << (caller == TOKEN_TYPE::FUNC ? "FUNC-STATEMENT-PRINT\n" : "STATEMENT-PRINT\n");

		nextToken();
		match(TOKEN_TYPE::LPARENTH);

		if (checkToken(TOKEN_TYPE::STRING)) {
			if (find(stringLiterals.begin(), stringLiterals.end(), curToken.text) == stringLiterals.end()) stringLiterals.push_back(curToken.text);

			int index = find(stringLiterals.begin(), stringLiterals.end(), curToken.text) - stringLiterals.begin();

			emitter.emitLine("adr x0, S" + to_string(index), caller);
			emitter.emitLine("ldr x1, =S" + to_string(index) + "_len", caller);
			emitter.emitLine("bl print_str", caller);

			nextToken();
		} else {
			TOKEN_TYPE type = expression(caller, parameters);
			emitter.emitLine("mov x0, x11", caller);

			if (type == TOKEN_TYPE::FLOAT) {
				emitter.emitLine("bl print_int", caller);
			} else if (type == TOKEN_TYPE::TEXT) {
				emitter.emitLine("bl print_cstr", caller);
			} else if (type == TOKEN_TYPE::INT) {
				emitter.emitLine("bl print_int", caller);
			}
		}

		match(TOKEN_TYPE::RPARENTH);
	} else if (checkToken(TOKEN_TYPE::PUTCHAR)) { // PUTCHAR ( expression )
		cout << (caller == TOKEN_TYPE::FUNC ? "FUNC-STATEMENT-PRINT_CHAR\n" : "STATEMENT-PRINT_CHAR\n");

		nextToken();
		match(TOKEN_TYPE::LPARENTH);

		TOKEN_TYPE type = expression(caller, parameters);
		emitter.emitLine("mov x0, x11", caller);

		if (type != TOKEN_TYPE::INT) {
			abort("PUTCHAR function expects type (INT), got (" + tokenTypeToString(type) + ").");
		}

		emitter.emitLine("bl print_char", caller);

		match(TOKEN_TYPE::RPARENTH);
	} else if (checkToken(TOKEN_TYPE::IF)) { // IF condition THEN statement ENDIF
		int elseIfCount = 0;

		cout << "STATEMENT-IF\n";
		nextToken();
		condition("XIF" + to_string(ifCount), caller, parameters);

		match(TOKEN_TYPE::THEN);
		nl();
		while (!checkToken(TOKEN_TYPE::ENDIF) && !checkToken(TOKEN_TYPE::ELSEIF) && !checkToken(TOKEN_TYPE::ELSE)) {
			statement(caller, parameters);
		}

		emitter.emitLine("b XELSE" + to_string(ifCount), caller);
		emitter.emitLine("XIF" + to_string(ifCount) + ":", caller);

		while (checkToken(TOKEN_TYPE::ENDIF) == 0) {
			if (checkToken(TOKEN_TYPE::ELSEIF)) {
				cout << "ELSEIF-BRANCH\n";
				nextToken();
				condition("XELIF" + to_string(ifCount) + to_string(elseIfCount), caller, parameters);

				match(TOKEN_TYPE::THEN);
				nl();

				while (!checkToken(TOKEN_TYPE::ELSEIF) && !checkToken(TOKEN_TYPE::ELSE) && !checkToken(TOKEN_TYPE::ENDIF)) {
					statement(caller, parameters);
				}

				emitter.emitLine("b XELSE" + to_string(ifCount), caller);
				emitter.emitLine("XELIF" + to_string(ifCount) + to_string(elseIfCount) + ":", caller);

				elseIfCount++;
			} else if (checkToken(TOKEN_TYPE::ELSE)) {
				cout << "ELSE-BRANCH\n";
				nextToken();
				nl();

				while (checkToken(TOKEN_TYPE::ENDIF) == 0) {
					statement(caller, parameters);
				}

				break;
			}
		}
		emitter.emitLine("XELSE" + to_string(ifCount) +  ":", caller);

		match(TOKEN_TYPE::ENDIF);
		ifCount++;
	} else if (checkToken(TOKEN_TYPE::WHILE)) { // WHILE condition DO statement ENDWHILE
		cout << (caller == TOKEN_TYPE::FUNC ? "FUNC-STATEMENT-WHILE\n" : "STATEMENT-WHILE\n");
		nextToken();
		emitter.emitLine("SWHILE" + to_string(whileCount) + ":", caller);

		condition("XWHILE" + to_string(whileCount), caller, parameters);

		match(TOKEN_TYPE::DO);
		nl();

		while (checkToken(TOKEN_TYPE::ENDWHILE) == 0) {
			statement(caller, parameters);
		}
		match(TOKEN_TYPE::ENDWHILE);

		emitter.emitLine("B SWHILE" + to_string(whileCount), caller);
		emitter.emitLine("XWHILE" + to_string(whileCount) + ":", caller);
		whileCount++;
	} else if (checkToken(TOKEN_TYPE::FUNC)) { // FUNC([INT | FLOAT | TEXT | none]) identifier IS nl {statement} ENDFUNC nl
		if (caller == TOKEN_TYPE::FUNC) {
			abort("Cannot define a function inside of a function");
		}

		cout << "STATEMENT-FUNCTION\n";
		nextToken();

		TOKEN_TYPE returnType = TOKEN_TYPE::INVALID;

		if (checkToken(TOKEN_TYPE::LPARENTH)) {
			nextToken();
			

			if (checkToken(TOKEN_TYPE::INT) || checkToken(TOKEN_TYPE::FLOAT) || checkToken(TOKEN_TYPE::TEXT)) {
				returnType = curToken.type;
				nextToken();
			} else {
				abort("Expected return type for function, got " + tokenTypeToString(curToken.type));
			}

			match(TOKEN_TYPE::RPARENTH);
		}

		if (functionMap.exists(curToken.text)) {
			abort("Function (" + curToken.text + ") already exists");
		}

		functionMap.push_name(curToken.text, returnType);

		string funcIdentifier = curToken.text;
		string bLabel = functionMap.getLabel(curToken.text);
		emitter.emitLine(bLabel + ":", TOKEN_TYPE::FUNC);
		emitter.emitLine("stp fp, lr, [sp, #-16]!", TOKEN_TYPE::FUNC);

		match(TOKEN_TYPE::IDENTIFIER);

		vector<pair<string, TOKEN_TYPE>> params;

		if (checkToken(TOKEN_TYPE::USING)) { // FUNC identifier USING [INT | FLOAT | TEXT] identifier {"," [INT | FLOAT | TEXT] identifier} IS ...
			cout << "\tPARAMETERS\n";
			nextToken();

			if (curToken.type != TOKEN_TYPE::INT && curToken.type != TOKEN_TYPE::FLOAT && curToken.type != TOKEN_TYPE::TEXT) {
				abort("Expected parameter type, got " + curToken.text);
			}

			TOKEN_TYPE paramType = curToken.type;
			nextToken();

			params.push_back({curToken.text, paramType});
			match(TOKEN_TYPE::IDENTIFIER);

			while (checkToken(TOKEN_TYPE::IS) == 0) {
				match(TOKEN_TYPE::COMMA);

				if (curToken.type != TOKEN_TYPE::INT && curToken.type != TOKEN_TYPE::FLOAT && curToken.type != TOKEN_TYPE::TEXT) {
					abort("Expected parameter type, got " + curToken.text);
				}

				paramType = curToken.type;
				nextToken();

				if (find(params.begin(), params.end(), pair{curToken.text, paramType}) != params.end()) {
					abort("Function parameter (" + curToken.text + ") already exists");
				}

				if (symbolMap.exists(curToken.text)) {
					abort("Symbol (" + curToken.text + ") exists outside of the function");
				}

				params.push_back({curToken.text, paramType});

				match(TOKEN_TYPE::IDENTIFIER);
			}

			functionMap.push_back(funcIdentifier, params);
		}

		match(TOKEN_TYPE::IS);
		nl();

		while (!checkToken(TOKEN_TYPE::ENDFUNC)) {
			statement(TOKEN_TYPE::FUNC, params);
		}

		match(TOKEN_TYPE::ENDFUNC);

		emitter.emitLine("ldp fp, lr, [sp], #16", TOKEN_TYPE::FUNC);
		emitter.emitLine("add sp, sp, #" + to_string((params.size() + params.size() % 2) * 8), TOKEN_TYPE::FUNC);
		emitter.emitLine("br lr", TOKEN_TYPE::FUNC);
	} else if (checkToken(TOKEN_TYPE::LABEL)) { // LABEL identifier
		if (caller == TOKEN_TYPE::FUNC) {
			abort("Cannot put a label inside a function");
		}

		cout << "STATEMENT-LABEL\n";
		nextToken();

		if (find(labels.begin(), labels.end(), curToken.text) != labels.end()) {
			abort("Label (" + curToken.text + ") already exists");
		}
		labels.push_back(curToken.text);

		emitter.emitLine("L" + curToken.text + ":", caller);
		match(TOKEN_TYPE::IDENTIFIER);
	} else if (checkToken(TOKEN_TYPE::GOTO)) { // GOTO identifier
		cout << (caller == TOKEN_TYPE::FUNC ? "FUNC-STATEMENT-GOTO\n" : "STATEMENT-GOTO\n");
		nextToken();

		gotos.push_back(curToken.text);

		emitter.emitLine("b L" + curToken.text, caller);
		match(TOKEN_TYPE::IDENTIFIER);
	} else if (checkToken(TOKEN_TYPE::INT) || checkToken(TOKEN_TYPE::FLOAT)) { // type identifier = expression
		TOKEN_TYPE declarationType = curToken.type;
		cout << (caller == TOKEN_TYPE::FUNC ? "FUNC-STATEMENT-" : "STATEMENT-") << tokenTypeToString(declarationType) << "\n";
		nextToken();

		if (symbolMap.exists(curToken.text)) {
			abort("Symbol (" + curToken.text + ") is already declared.");
		}

		symbolMap.pushBack(curToken.text, declarationType);
		string identLabel = symbolMap.getLabel(curToken.text);

		match(TOKEN_TYPE::IDENTIFIER);
		match(TOKEN_TYPE::EQ);

		if (expression(caller, parameters) != declarationType) {
			abort("Cannot assign value of type (" + tokenTypeToString(expression(caller, parameters)) + ") to variable of type (" + tokenTypeToString(declarationType) + ").");
		}

		emitter.emitLine("adr x13, " + identLabel, caller);
		emitter.emitLine("str x11, [x13]", caller);
	} else if (checkToken(TOKEN_TYPE::TEXT)) { // TEXT identifier = STRING | expression
		cout << (caller == TOKEN_TYPE::FUNC ? "FUNC-STATEMENT-TEXT\n" : "STATEMENT-TEXT\n");
		nextToken();

		if (symbolMap.exists(curToken.text)) { // make sure the symbol doesn't already exist
			abort("Symbol (" + curToken.text + ") is already declared.");
		}

		symbolMap.pushBack(curToken.text, TOKEN_TYPE::TEXT); // add the new symbol, type of TEXT
		string identLabel = symbolMap.getLabel(curToken.text);

		match(TOKEN_TYPE::IDENTIFIER);
		match(TOKEN_TYPE::EQ);

		if (checkToken(TOKEN_TYPE::STRING)) { // = STRING
			if (find(stringLiterals.begin(), stringLiterals.end(), curToken.text) == stringLiterals.end()) {
				stringLiterals.push_back(curToken.text);
			}

			int index = find(stringLiterals.begin(), stringLiterals.end(), curToken.text) - stringLiterals.begin();

			emitter.emitLine("adr x13, " + identLabel, caller);
			emitter.emitLine("adr x11, S" + to_string(index), caller);
			emitter.emitLine("str x11, [x13]", caller);

			nextToken();
		} else {
			if (expression(caller, parameters) != TOKEN_TYPE::TEXT) {
				abort("Cannot assign value of type (" + tokenTypeToString(expression(caller, parameters)) + ") to variable of type (TEXT).");
			}

			emitter.emitLine("adr x13, " + identLabel, caller);
			emitter.emitLine("str x11, [x13]", caller);
		}
	/*} else if (checkToken(TOKEN_TYPE::INPUT)) { // INPUT identifier
		cout << (caller == TOKEN_TYPE::FUNC ? "FUNC-STATEMENT-INPUT\n" : "STATEMENT-INPUT\n");
		nextToken();

		if (!symbolMap.exists(curToken.text)) {
			abort("Symbol (" + curToken.text + ") does not exist.");
		}

		if (symbolMap.getType(curToken.text) != TOKEN_TYPE::TEXT) {
			abort("Cannot assign value of type (" + tokenTypeToString(expression(caller, parameters)) + ") to variable of type (TEXT).");
		}

	*/
	} else if (checkToken(TOKEN_TYPE::IDENTIFIER)) { // identifier = expression
		cout << (caller == TOKEN_TYPE::FUNC ? "FUNC-STATEMENT-ASSIGN\n" : "STATEMENT-ASSIGN\n");

		if (!symbolMap.exists(curToken.text)) {
			abort("Symbol (" + curToken.text + ") does not exist.");
		}

		string identName = curToken.text;
		TOKEN_TYPE identType = symbolMap.getType(identName);
		string identLabel = symbolMap.getLabel(identName);
		nextToken();

		match(TOKEN_TYPE::EQ);

		TOKEN_TYPE expressionType = expression(caller, parameters);
		if (expressionType != identType) {
			abort("Cannot assign value of type (" + tokenTypeToString(expressionType) + ") to variable of type (" + tokenTypeToString(identType) + ").");
		}

		emitter.emitLine("adr x13, " + identLabel, caller);
		emitter.emitLine("str x11, [x13]", caller);
	} else if (checkToken(TOKEN_TYPE::DO)) { // DO identifier [WITH expression {"," expression}]
		cout << (caller == TOKEN_TYPE::FUNC ? "FUNC-STATEMENT-FUNCTIONCALL" : "STATEMENT-FUNCTIONCALL");
		nextToken();
		functionCall(caller, parameters);
	} else if (checkToken(TOKEN_TYPE::RET)) {
		if (caller != TOKEN_TYPE::FUNC) {
			abort("Cannot return outside of a function");
		}

		cout << "RETURN\n";
		nextToken();

		expression(caller, parameters);

		emitter.emitLine("mov x9, x11", caller);
	} else {
		abort("Invalid state at " + string(curToken.text) + " (" + tokenTypeToString(curToken.type) + ").");
	}

	nl();
}

void Parser::nl() {
	cout << "NEWLINE\n";

	match(TOKEN_TYPE::NEWLINE);

	while (checkToken(TOKEN_TYPE::NEWLINE)) {
		nextToken();
	}
}

// expression ::= term {("+" | "/") term}
TOKEN_TYPE Parser::expression(TOKEN_TYPE caller, vector<pair<string, TOKEN_TYPE>> parameters) {
	cout << "EXPRESSION\n";

	TOKEN_TYPE lTermType = term(caller, parameters);

	emitter.emitLine("mov x11, x10", caller);

	while (checkToken(TOKEN_TYPE::PLUS) || checkToken(TOKEN_TYPE::MINUS)) {	
		TOKEN_TYPE lastType = curToken.type;

		nextToken();
		TOKEN_TYPE currTermType = term(caller, parameters);

		if (lTermType != currTermType) {
			abort("Cannot apply operator (" + tokenTypeToString(lastType) + ") to different types (" + tokenTypeToString(lTermType) + " and " + tokenTypeToString(currTermType) + ").");
		}

		if (lastType == TOKEN_TYPE::PLUS) { // +
			if (lTermType == TOKEN_TYPE::TEXT) { // addition for TEXT type terms. uses the str_concat runtime
				cout << "RUNTIME-CALL [str_concat]\n";
				emitter.emitLine("mov x0, x11");
				emitter.emitLine("mov x1, x10");
				emitter.emitLine("bl str_concat");
				emitter.emitLine("mov x11, x0");
			} else if (lTermType == TOKEN_TYPE::INT || lTermType == TOKEN_TYPE::FLOAT) { // additition for INT or FLOAT type terms
				emitter.emitLine("add x11, x11, x10", caller);
			}
		} else if (lastType == TOKEN_TYPE::MINUS) { // -
			if (lTermType == TOKEN_TYPE::TEXT) {
				abort("Cannot apply operator (" + tokenTypeToString(curToken.type) + ") to non-numeric type (" + tokenTypeToString(lTermType) + ").");
			}

			emitter.emitLine("sub x11, x11, x10", caller);
		}
	}

	return lTermType;
}

// term ::= unary {("*" | "/") unary}
TOKEN_TYPE Parser::term(TOKEN_TYPE caller, vector<pair<string, TOKEN_TYPE>> parameters) {
	cout << "TERM\n";

	TOKEN_TYPE lUnaryType = unary(caller, parameters); // hold each unary in r10. do operations on r9 and put the results in r10
	
	emitter.emitLine("mov x10, x9", caller);


	while (checkToken(TOKEN_TYPE::ASTERISK) || checkToken(TOKEN_TYPE::SLASH) || checkToken(TOKEN_TYPE::MODULO)) {
		if (lUnaryType != TOKEN_TYPE::INT && lUnaryType != TOKEN_TYPE::FLOAT) {
			abort("Cannot apply operator (" + tokenTypeToString(curToken.type) + ") to non-numeric type (" + tokenTypeToString(lUnaryType) + ").");
		}

		TOKEN_TYPE lastType = curToken.type;
		nextToken();
		TOKEN_TYPE currUnaryType = unary(caller, parameters);

		if (lUnaryType != currUnaryType) {
			abort("Cannot apply operator (" + tokenTypeToString(lastType) + ") to different types (" + tokenTypeToString(lUnaryType) + " and " + tokenTypeToString(currUnaryType) + ").");
		}

		if (lastType == TOKEN_TYPE::ASTERISK) { 	// multiply
			emitter.emitLine("mul x10, x10, x9", caller);
		} else if (lastType == TOKEN_TYPE::SLASH) { // divide
			emitter.emitLine("sdiv x10, x10, x9", caller);
		} else {
			emitter.emitLine("udiv x8, x10, x9", caller);
			emitter.emitLine("msub x10, x8, x9, x10", caller);
		}
	}

	return lUnaryType;
}

// unary ::= ["+" | "-"] primary
TOKEN_TYPE Parser::unary(TOKEN_TYPE caller, vector<pair<string, TOKEN_TYPE>> parameters) {
	cout << "UNARY\n";

	TOKEN_TYPE lastType = curToken.type;

	// If theres a +/- skip and go to primary
	if (curToken.type == TOKEN_TYPE::PLUS || curToken.type == TOKEN_TYPE::MINUS) {
		nextToken();
	}
	TOKEN_TYPE primaryType = primary(caller, parameters);

	if (primaryType != TOKEN_TYPE::INT && primaryType != TOKEN_TYPE::FLOAT) {
		if (lastType == TOKEN_TYPE::PLUS || lastType == TOKEN_TYPE::MINUS) {
			abort("Cannot apply unary (" + tokenTypeToString(lastType) + ") to non-numeric type (" + curToken.text + ").");
		}
	}

	if (lastType == TOKEN_TYPE::MINUS) {
		emitter.emitLine("mvn x9, x9", caller);
	}

	return primaryType;
}

// primary ::= number | identifier
TOKEN_TYPE Parser::primary(TOKEN_TYPE caller, vector<pair<string, TOKEN_TYPE>> parameters) { // Primary held in r9
	cout << "PRIMARY (" << curToken.text << ")\n";
	TOKEN_TYPE type = TOKEN_TYPE::INVALID;


	if (checkToken(TOKEN_TYPE::NUMBER)) {
		emitter.emitLine("mov x9, #" + curToken.text, caller);
		nextToken();

		type = TOKEN_TYPE::INT;
	} else if (checkToken(TOKEN_TYPE::IDENTIFIER)) {
		if (!functionMap.exists(curToken.text)) { // not a function
			if (caller == TOKEN_TYPE::FUNC) {
				 if (!parameters.empty()) { // parameter

					for (auto p : parameters) {
						if (p.first == curToken.text) {
							type = p.second;
							emitter.emitLine("ldr x9, [sp, " + functionMap.getParamOffset(parameters, curToken.text) + "]", caller);
							break;
						}
					}
					if (type == TOKEN_TYPE::INVALID) { // not a parameter, check if variable in symbol table
						if (!symbolMap.exists(curToken.text)) {
							abort("Symbol (" + curToken.text + ") does not exist.");
						}

						type = symbolMap.getType(curToken.text);
						emitter.emitLine("adr x9, " + symbolMap.getLabel(curToken.text), caller);
						emitter.emitLine("ldr x9, [x9]", caller);	
					}
				}
			}
			
			else if (symbolMap.exists(curToken.text)) { // variable
				type = symbolMap.getType(curToken.text);

				emitter.emitLine("adr x9, " + symbolMap.getLabel(curToken.text), caller);
				emitter.emitLine("ldr x9, [x9]", caller);
			} else {
				abort("Symbol (" + curToken.text + ") does not exist.");
			}

			nextToken();
		} else { // must be a function call
			type = functionCall(caller, parameters);
		}
	} else if (checkToken(TOKEN_TYPE::LEN)) { // built-in LEN function
		nextToken();

		type = TOKEN_TYPE::INT;
		match(TOKEN_TYPE::LPARENTH);

		emitter.emitLine("stp x0, x1, [sp, #-16]!", caller);
		emitter.emitLine("stp x2, x3, [sp, #-16]!", caller);

		if (checkToken(TOKEN_TYPE::STRING)) { // input is a string literal
			if (find(stringLiterals.begin(), stringLiterals.end(), curToken.text) == stringLiterals.end()) {
				stringLiterals.push_back(curToken.text);
			}

			int index = find(stringLiterals.begin(), stringLiterals.end(), curToken.text) - stringLiterals.begin();
			
			cout << "RUNTIME-CALL [str_len]\n";

			emitter.emitLine("adr x0, S" + to_string(index), caller);
			emitter.emitLine("bl str_len", caller);
			emitter.emitLine("mov x9, x0", caller);

			nextToken();
		} else { // input is an expression, of TEXT type
			if (expression(caller, parameters) != TOKEN_TYPE::TEXT) {
				abort("LEN function expects type (TEXT), got (" + tokenTypeToString(expression(caller, parameters)) + ").");
			}

			cout << "RUNTIME-CALL [str_len]\n";

			emitter.emitLine("mov x0, x11", caller);
			emitter.emitLine("bl str_len", caller);
			emitter.emitLine("mov x9, x0", caller);
		}

		match(TOKEN_TYPE::RPARENTH);

		emitter.emitLine("ldp x2, x3, [sp], #16", caller);
		emitter.emitLine("ldp x0, x1, [sp], #16", caller);


	} else if (checkToken(TOKEN_TYPE::CHARAT)) { 	// built-in CHARAT function
		nextToken();								// CHARAT(TEXT, INT) -- returns INT

		type = TOKEN_TYPE::INT;
		match(TOKEN_TYPE::LPARENTH);
		
		emitter.emitLine("stp x0, x1, [sp, #-16]!", caller);
		emitter.emitLine("stp x2, x3, [sp, #-16]!", caller);

		if (checkToken(TOKEN_TYPE::STRING)) { // input is a string literal
			if (find(stringLiterals.begin(), stringLiterals.end(), curToken.text) == stringLiterals.end()) {
				stringLiterals.push_back(curToken.text);
			}

			int index = find(stringLiterals.begin(), stringLiterals.end(), curToken.text) - stringLiterals.begin();

			emitter.emitLine("adr x0, S" + to_string(index), caller);

			nextToken();
		} else { // input is an expression, of TEXT type
			if (expression(caller, parameters) != TOKEN_TYPE::TEXT) {
				abort("CHARAT function expects type (TEXT) for first parameter, got (" + tokenTypeToString(expression(caller, parameters)) + ").");
			}

			emitter.emitLine("mov x0, x11", caller);
		}
		
		match(TOKEN_TYPE::COMMA);

		if (expression(caller, parameters) != TOKEN_TYPE::INT) {
			abort("CHARAT function expects type (INT) for second parameter, got (" + tokenTypeToString(expression(caller, parameters)) + ").");
		}

		cout << "RUNTIME-CALL [str_char_at]\n";

		emitter.emitLine("mov x1, x11", caller);
		emitter.emitLine("bl str_char_at", caller);
		emitter.emitLine("mov x9, x0", caller);
		match(TOKEN_TYPE::RPARENTH);

		emitter.emitLine("ldp x2, x3, [sp], #16", caller);
		emitter.emitLine("ldp x0, x1, [sp], #16", caller);

	} else if (checkToken(TOKEN_TYPE::SUBSTR)) {	// build-in SUBSTR function
		nextToken();								// SUBTR(TEXT, INT, INT)

		type = TOKEN_TYPE::TEXT;
		match(TOKEN_TYPE::LPARENTH);

		emitter.emitLine("stp x0, x1, [sp, #-16]!", caller);
		emitter.emitLine("stp x2, x3, [sp, #-16]!", caller);

		if (checkToken(TOKEN_TYPE::STRING)) { // input is a string literal
			if (find(stringLiterals.begin(), stringLiterals.end(), curToken.text) == stringLiterals.end()) {
				stringLiterals.push_back(curToken.text);
			}

			int index = find(stringLiterals.begin(), stringLiterals.end(), curToken.text) - stringLiterals.begin();

			emitter.emitLine("adr x0, S" + to_string(index), caller);

			nextToken();
		} else { // input is an expression, of TEXT type
			if (expression(caller, parameters) != TOKEN_TYPE::TEXT) {
				abort("CHARAT function expects type (TEXT) for first parameter, got (" + tokenTypeToString(expression(caller, parameters)) + ").");
			}

			emitter.emitLine("mov x0, x11", caller);
		}
		
		match(TOKEN_TYPE::COMMA);

		if (expression(caller, parameters) != TOKEN_TYPE::INT) {
			abort("CHARAT function expects type (INT) for second parameter, got (" + tokenTypeToString(expression(caller, parameters)) + ").");
		}

		emitter.emitLine("mov x1, x11", caller);

		match(TOKEN_TYPE::COMMA);

		if (expression(caller, parameters) != TOKEN_TYPE::INT) {
			abort("CHARAT function expects type (INT) for second parameter, got (" + tokenTypeToString(expression(caller, parameters)) + ").");
		}

		emitter.emitLine("mov x2, x11", caller);

		cout << "RUNTIME-CALL [sub_str]\n";

		emitter.emitLine("bl sub_str", caller);
		emitter.emitLine("mov x9, x0", caller);
		match(TOKEN_TYPE::RPARENTH);

		emitter.emitLine("ldp x2, x3, [sp], #16", caller);
		emitter.emitLine("ldp x0, x1, [sp], #16", caller);

	} else if (checkToken(TOKEN_TYPE::INPUT)) { // built-in INPUT functions
		nextToken();							// INPUT ()

		type = TOKEN_TYPE::TEXT;
		match(TOKEN_TYPE::LPARENTH);

		emitter.emitLine("stp x0, x1, [sp, #-16]!", caller);
		emitter.emitLine("stp x2, x3, [sp, #-16]!", caller);

		cout << "RUNTIME-CALL [usr_input]\n";
		emitter.emitLine("bl usr_input", caller);
		emitter.emitLine("mov x9, x0", caller);

		match(TOKEN_TYPE::RPARENTH);

		emitter.emitLine("ldp x2, x3, [sp], #16", caller);
		emitter.emitLine("ldp x0, x1, [sp], #16", caller);
	} else if (checkToken(TOKEN_TYPE::STRING)) { // string literal in an expression
		if (find(stringLiterals.begin(), stringLiterals.end(), curToken.text) == stringLiterals.end()) {
			stringLiterals.push_back(curToken.text);
		}

		int index = find(stringLiterals.begin(), stringLiterals.end(), curToken.text) - stringLiterals.begin();

		emitter.emitLine("adr x9, S" + to_string(index), caller);

		type = TOKEN_TYPE::TEXT;
		nextToken();
	} else if (checkToken(TOKEN_TYPE::CHARACTER)) { // character literal in an expression
		type = TOKEN_TYPE::INT;

		emitter.emitLine("mov x9, #" + to_string((int)curToken.text[0]), caller);
		nextToken();
	} else {
		abort("Unexpected token (" + curToken.text + ") of type (" + tokenTypeToString(curToken.type) + ") in primary.");
	}

	return type;
}

// condition ::= expression (("==" | ">" | ">=" | "<"| "<=") experssion)+
void Parser::condition(string exitLabel, TOKEN_TYPE caller, vector<pair<string, TOKEN_TYPE>> parameters) {
	cout << "CONDITION\n";

	TOKEN_TYPE expressionType = expression(caller, parameters);
	// result in r11
	emitter.emitLine("mov x12, x11", caller);

	TOKEN_TYPE conditional = curToken.type;

	if (checkToken(TOKEN_TYPE::EQEQ) || checkToken(TOKEN_TYPE::NEQ) || checkToken(TOKEN_TYPE::GT) || checkToken(TOKEN_TYPE::GTEQ) || checkToken(TOKEN_TYPE::LT) || checkToken(TOKEN_TYPE::LTEQ)) {
		nextToken();
		if (expression(caller, parameters) != expressionType) {
			abort("Incompatible types in condition (" + tokenTypeToString(expressionType) + ").");
		}
	} else {
		abort("Expected expression, got " + curToken.text);
	}
	

	string branchType = "";

	switch (conditional) {
		case TOKEN_TYPE::EQEQ:
			if (expressionType == TOKEN_TYPE::TEXT) {
				cout << "RUNTIME-CALL [str_cmp]\n";

				emitter.emitLine("mov x0, x12", caller); // set up x0, x1 for str_cmp runtime
				emitter.emitLine("mov x1, x11", caller);
				emitter.emitLine("bl str_cmp", caller);

				branchType = "cbnz x0, ";
			} else {
				 branchType = "bne ";
			}

			break;
		case TOKEN_TYPE::NEQ:
			if (expressionType == TOKEN_TYPE::TEXT) {
				cout << "RUNTIME-CALL [str_cmp]\n";

				emitter.emitLine("mov x0, x12", caller); // set up x0, x1 for str_cmp runtime
				emitter.emitLine("mov x1, x11", caller);
				emitter.emitLine("bl str_cmp", caller);

				branchType = "cbz x0, ";
			} else {
				branchType = "beq ";
			}

			break;
		case TOKEN_TYPE::GT:
			branchType = "ble "; 	
			break;
		case TOKEN_TYPE::GTEQ:
			branchType = "blt ";
			break;
		case TOKEN_TYPE::LT:
			branchType = "bge ";
			break;
		case TOKEN_TYPE::LTEQ:
			branchType = "bgt ";
			break;
	}

	if (expressionType != TOKEN_TYPE::TEXT) emitter.emitLine("cmp x12, x11", caller);
	emitter.emitLine(branchType + exitLabel, caller);

	/* -------------- Currently removed multiple expressions w/i a condition
	while (checkToken(TOKEN_TYPE::EQEQ) || checkToken(TOKEN_TYPE::GT) || checkToken(TOKEN_TYPE::GTEQ) || checkToken(TOKEN_TYPE::LT) || checkToken(TOKEN_TYPE::LTEQ)) {
		nextToken();
		expression();
	}
	*/
}