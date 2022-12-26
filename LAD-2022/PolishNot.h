#pragma once

#include "stdafx.h"
#include <vector>

typedef std::vector<LT::Entry> ltVec;
typedef std::vector<int> intVec;

namespace PolishNot
{
	bool PolishNotation(LexAnalize::LEX& lex, Log::LOG& log);
	// � ������ ��������� ������� ������ ���� ��������� � �������� ������
	intVec getExprPositions(LexAnalize::LEX& lex);
	// ��������� ������ ��������� � ��������� �������
	int  fillVector(int lextable_pos, LT::LexTable& lextable, ltVec& v);
	// ������ ������� �������� ������� ��������������� � �������� �������
	bool setPolishNotation(IT::IdTable& idtable, Log::LOG& log, int lextable_pos, ltVec& v);
	// ��������� ������ � ����� ������� ������
	void addToTable(LT::LexTable& new_table, IT::IdTable& idtable, ltVec& v);
}
