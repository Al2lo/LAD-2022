#pragma once

#include "stdafx.h"
#include <vector>

typedef std::vector<LT::Entry> ltVec;
typedef std::vector<int> intVec;

namespace PolishNot
{
	bool PolishNotation(LexAnalize::LEX& lex, Log::LOG& log);
	// в вектор сохраняем позиции начала всех выражений в исходном тексте
	intVec getExprPositions(LexAnalize::LEX& lex);
	// заполняем вектор лексемами в инфиксной нотации
	int  fillVector(int lextable_pos, LT::LexTable& lextable, ltVec& v);
	// внутри вектора получаем порядок идентификаторов в польской нотации
	bool setPolishNotation(IT::IdTable& idtable, Log::LOG& log, int lextable_pos, ltVec& v);
	// добавляем вектор в новую таблицу лексем
	void addToTable(LT::LexTable& new_table, IT::IdTable& idtable, ltVec& v);
}
