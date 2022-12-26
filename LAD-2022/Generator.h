#pragma once
#include "stdafx.h"

namespace Gener
{
	void codeGeneration(std::ofstream* stream, LexAnalize::LEX& table);						//�������� �����-�� � Generator
	void Head(std::ofstream* stream, LexAnalize::LEX table);				//���� ������������ ��������� , ������ .....
	void getConstant(std::ofstream* stream, LexAnalize::LEX table);	//���� ��������
	void getData(std::ofstream* stream, LexAnalize::LEX table);				//���� ������
	void getFuncCode(std::ofstream* stream, LexAnalize::LEX table, int i);
	void getExitCode(std:: ofstream* stream, LexAnalize::LEX table, int i, char* funcName);
	void getExitCode(std::ofstream* stream, LexAnalize::LEX table, int i, char* funcName);
	void getConditionsCode(std::ofstream* stream, LexAnalize::LEX table, int i, char* cycleCode);
	void genCallFuncCode(std::ofstream* stream, LexAnalize::LEX& table, int i);
}