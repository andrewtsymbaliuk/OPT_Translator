#pragma once
#include "LexAnaliz.h"
#include "SyntaxAnaliz.h"

class CodeGenerator {
public:
	CodeGenerator(string File);
	string StrAsm;
	string StrError;
	Node* N;
	int Error;
	int Label;
	int LabelCounter;
	string ProgramName;
	string elsejmp;
	int check1;
	int check2;
	void listing(string File);
	void init(string File);
	void start(Node* N);
	void statements_list(Node* N);
	void statement(Node* N);
	void condition_statement(Node* N);
	void condition_expression(Node* N);
	void incomplete_statement(Node* N);
	bool alternative_part(Node* N);
	void comparison_operator(Node* N);
	bool expression(Node* N);
	bool check_expression(Node* N);
};