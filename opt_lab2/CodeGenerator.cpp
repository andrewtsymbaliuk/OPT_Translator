#include "CodeGenerator.h"

CodeGenerator::CodeGenerator(string File)
{
	init(File);
	listing(File);
}

void CodeGenerator::init(string File)
{
	SyntaxAnaliz SynAn(File);
	N = SynAn.Tree;
	ProgramName=N->Down->Down->Next->Down->Down->Name;
	StrAsm = "";
	Error = 0;
	StrError = "";
	Label = 0;
	LabelCounter = 0;
	N = N->Down->Down->Next->Next->Next->Down->Next;
	if(SynAn.ErrorCounter==0)
		start(N);
}

void CodeGenerator::start(Node* N)
{
	StrAsm.append("data SEGMENT\ndata ENDS\n");
	if (N->Func == "<statements-list>") {
		StrAsm.append("code SEGMENT\n\tASSUME cs:code, ds:data\t\n"+ProgramName+":\n");
		statements_list(N->Down);
		StrAsm.append("mov ah, 4ch\nint 21h\ncode ends\nend " + ProgramName);
	}
	else {
		Error++;
		StrError += "Code Generator Eror: line " + to_string(N->Line) + " column " + to_string(N->Column) + "\n";
		return;
	}

}

void CodeGenerator::statements_list(Node* N) {
	if (N->Func == "<empty>") {
		StrAsm.append("\tnop\n");
	}
	else if (N->Func == "<statement>") {
		statement(N->Down);
		if (N->Next->Func == "<statements-list>") {
			Label = LabelCounter;
			statements_list(N->Next->Down);
		}
	}
}

void CodeGenerator::statement(Node* N) {
	Label = LabelCounter;
	if (N->Func == "<condition-statement>") {
		condition_statement(N->Down);

	}
	else if(N->Code==408){
		int whileLabel = Label;
		StrAsm.append("?L"+to_string(Label)+":\n");
		Label++;
		LabelCounter++;
		if (N->Next->Func == "<conditional-expression>") {
			condition_expression(N->Next->Down);
			Label++;
			LabelCounter++;
			N = N->Next->Next->Next;
			statements_list(N->Down);
			Label = whileLabel;
			StrAsm.append("\tjmp ?L" + to_string(Label) + "\n");
			Label++;
			StrAsm.append("?L" + to_string(Label) + ":\tnop\n");
		}
	}
}

void CodeGenerator::condition_statement(Node* N) {
	int ifLabel = Label;
	if (N->Func=="<incomplete-condition-statement>") {
		incomplete_statement(N->Down->Next);
		Label = ifLabel;
		elsejmp+="?L" + to_string(Label) + ":\tnop\n";
		if (N->Next != NULL) {
			if (N->Next->Func == "<alternative-part>") {
				N = N->Next->Down;
				if (alternative_part(N)) {
					Label++;
					LabelCounter;
					StrAsm.append("?L" + to_string(Label) + ":\tnop\n");
				}
			}
		}
	}
}

void CodeGenerator::condition_expression(Node* N)
{
	if (N->Func == "<expression>") {
		StrAsm.append("\tmov ax, ");
		expression(N->Down);
		StrAsm.append("\tmov bx, ");
		expression(N->Next->Next->Down);
		StrAsm.append("\tcmp ax, bx\n");
		comparison_operator(N->Next->Down);
	}
}

void CodeGenerator::incomplete_statement(Node* N) {
	if (N->Func == "<conditional-expression>") {
		condition_expression(N->Down);
		LabelCounter++;
		Label++;
		int ifLabel = Label;
		Label++;
		LabelCounter++;
		N=N->Next->Next;
		if (N->Func == "<statements-list>") {
			statements_list(N->Down);
			Label = ifLabel;
			elsejmp="\tjmp ?L" + to_string(Label) + "\n";
		}
	}
}

bool CodeGenerator::alternative_part(Node* N)
{
	int alternativeLabel = Label;
	if (N->Func == "<empty>") {	
		StrAsm.append("?L" + to_string(Label) + ":\tnop\n");
		elsejmp = "";
		return 0;
	}
	if (N->Next->Func == "<statements-list>") {
		StrAsm.append(elsejmp);
		statements_list(N->Next->Down);
		Label = alternativeLabel;
		return 1;
	}
}

void CodeGenerator::comparison_operator(Node* N)
{
	switch (N->Code)
	{
	case 60:
		StrAsm.append("\tjge ?L" + to_string(Label) + "\n");
		return ;
	case 61:
		StrAsm.append("\tjne ?L" + to_string(Label) + "\n");
		return ;
	case 62:
		StrAsm.append("\tjle ?L" + to_string(Label) + "\n");
		return ;
	case 304:
		StrAsm.append("\tjg ?L" + to_string(Label) + "\n");
		return ;
	case 305:
		StrAsm.append("\tje ?L" + to_string(Label) + "\n");
		return ;
	case 306:
		StrAsm.append("\tjl ?L" + to_string(Label) + "\n");
		return ;
	}
}

bool CodeGenerator::expression(Node* N)
{
	if (N->Func=="<unsigned-integer>") {
		StrAsm.append(N->Down->Name + "\n");
		return true;
	}
	else if (N->Down->Func=="<identifier>") {
		N = N->Down->Down;
		if (N->Name==ProgramName) {
			Error++;
			StrError.append("Code Generator: Error on Line " + to_string(N->Line) + ", Column " + to_string(N->Column) + ": Invalid identifier, don`t use name program");
		}
		else {
			StrAsm.append(N->Name + "\n");
		}
	}
}

void CodeGenerator::listing(string File)
{
	File += "generated.txt";
	ofstream f;
	f.open(File, std::ios::app);
	if (Error != 0) {
		cout << "Code Generator completed with error" << endl << StrError << endl << endl;
		f << "Code Generator completed with error" << endl << StrError << endl;
	}
	else {
		cout << "code generator completed successfuly" << endl << endl;
		f << StrAsm << endl << endl;
	}
}
