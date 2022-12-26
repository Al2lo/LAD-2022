#pragma once
#include "stdafx.h"

namespace Gener
{
	void codeGeneration(std::ofstream* stream, LexAnalize::LEX& table);						//передача парам-ов в Generator
	void Head(std::ofstream* stream, LexAnalize::LEX table);				//блок перечесления библиотек , модель .....
	void getConstant(std::ofstream* stream, LexAnalize::LEX table);	//блок констант
	void getData(std::ofstream* stream, LexAnalize::LEX table);				//блок данных
	void getFuncCode(std::ofstream* stream, LexAnalize::LEX table, int i);
	void getExitCode(std:: ofstream* stream, LexAnalize::LEX table, int i, char* funcName);
	void getExitCode(std::ofstream* stream, LexAnalize::LEX table, int i, char* funcName);
	void getConditionsCode(std::ofstream* stream, LexAnalize::LEX table, int i, char* cycleCode);
	void genCallFuncCode(std::ofstream* stream, LexAnalize::LEX& table, int i);
}