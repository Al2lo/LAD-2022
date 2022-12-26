#include "stdafx.h"

namespace PolishNot
{
	int getPriority(LT::Entry& e)
	{
		switch (e.lexema)
		{
		case LEX_LEFTHESIS:
		case LEX_RIGHTHESIS:
			return 0;
		case LEX_PLUS:
		case LEX_MINUS:
			return 1;
		case LEX_MULTIPLICATION:
		case LEX_DIRSLASH:
		case LEX_PERSENT:
			return 2;
		case LEX_LEFT:
		case LEX_RIGHT:
			return 3;

		default:
			return -1;
		}
	}
	bool PolishNotation(LexAnalize::LEX& tables, Log::LOG& log)
	{
		unsigned curExprBegin = 0;
		ltVec vec;
		LT::LexTable new_table = LT::Create(tables.lextab.maxsize);
		intVec vPosition = getExprPositions(tables); 

		for (int i = 0; i < tables.lextab.size; i++)
		{
			if (curExprBegin < vPosition.size() && i == vPosition[curExprBegin])
			{
				int lexCount = fillVector(vPosition[curExprBegin], tables.lextab, vec); 
				if (lexCount > 1)
				{
					bool rc = setPolishNotation(tables.idtab, log, vPosition[curExprBegin], vec); 
					if (!rc)
					{
						return false;
					}
				}
				addToTable(new_table, tables.idtab, vec);
				i += lexCount - 1;
				curExprBegin++;
				continue;
			}
			if (tables.lextab.table[i].lexema == LEX_ID || tables.lextab.table[i].lexema == LEX_LITERAL)
			{
				int firstIndef = LexAnalize::getIndexInLT(new_table, tables.lextab.table[i].idxTI);
				if (firstIndef == -1)
				{
					firstIndef = new_table.size;
				}
				tables.idtab.table[tables.lextab.table[i].idxTI].idxfirstLE = firstIndef;
			}
			LT::Add(new_table, tables.lextab.table[i]);
		}

		tables.lextab = new_table;
		return true;
	}

	int fillVector(int posExprBegin, LT::LexTable& lextab, ltVec& vec)
	{
		vec.clear();
		for (int i = posExprBegin; i < lextab.size; i++)
		{
			if (lextab.table[i].lexema == LEX_SEMICOLON)
			{
				break;
			}
			vec.push_back(LT::Entry(lextab.table[i]));
		}
		return vec.size();
	}

	void addToTable(LT::LexTable& new_table, IT::IdTable& idtable, ltVec& vec)
	{
		for (int i = 0; i < vec.size(); i++)
		{
			LT::Add(new_table, vec[i]);

			if (vec[i].lexema == LEX_ID || vec[i].lexema ==LEX_LITERAL)
			{
				int firstIndef = LexAnalize::getIndexInLT(new_table, vec[i].idxTI);
				idtable.table[vec[i].idxTI].idxfirstLE = firstIndef;
			}
		}
	}

	intVec getExprPositions(LexAnalize::LEX& tables)
	{
		intVec vec;
		bool f_begin = false;
		bool f_end = false; 
		int begin = 0;
		int end = 0;
		for (int i = 0; i < tables.lextab.size; i++)
		{
			if (tables.lextab.table[i].lexema == LEX_ASSIGN)
			{
				begin = i + 1;
				f_begin = true;
				continue;
			}
			if (f_begin && tables.lextab.table[i].lexema == LEX_SEMICOLON)
			{
				end = i;
				f_end = true;
				continue;
			}
			if (f_begin && f_end)
			{
				vec.push_back(begin);
				f_begin = false;
				f_end = false;
			}
		}
		return vec;
	}

	
	bool setPolishNotation(IT::IdTable& idtabl, Log::LOG& log, int lextab_pos, ltVec& vec)
	{
		vector<LT::Entry> result;
		stack<LT::Entry> st;
		bool ignore = false;

		for (int i = 0; i < vec.size(); i++)
		{
			if (ignore)
			{
				result.push_back(vec[i]);
				if (vec[i].lexema == LEX_RIGHTHESIS)
				{
					ignore = false;
				}
				continue;
			}

			int priority = getPriority(vec[i]);

			if (vec[i].lexema == LEX_LEFTHESIS || vec[i].lexema == LEX_RIGHTHESIS || vec[i].lexema == LEX_PLUS || vec[i].lexema == LEX_MINUS || vec[i].lexema == LEX_MULTIPLICATION || vec[i].lexema == LEX_DIRSLASH || vec[i].lexema == LEX_PERSENT || vec[i].lexema == LEX_LEFT || vec[i].lexema == LEX_RIGHT)
			{
				if (st.empty() || vec[i].lexema == LEX_LEFTHESIS)
				{
					st.push(vec[i]);
					continue;
				}

				if (vec[i].lexema == LEX_RIGHTHESIS)
				{

					while (!st.empty() && st.top().lexema != LEX_LEFTHESIS)
					{
						result.push_back(st.top());
						st.pop();
					}
					if (st.top().lexema == LEX_LEFTHESIS)
					{
						st.pop();
					}
					continue;
				}

				while (!st.empty() && getPriority(st.top()) >= priority)
				{
					result.push_back(st.top()); st.pop();
				}
				st.push(vec[i]);
			}

			if (vec[i].lexema == LEX_LITERAL || vec[i].lexema == LEX_ID)
			{
				if (idtabl.table[vec[i].idxTI].idtype == IT::F || idtabl.table[vec[i].idxTI].idtype == IT::S)
				{
					ignore = true;
				}
				result.push_back(vec[i]);	
			}
			if (vec[i].lexema != LEX_LEFTHESIS && vec[i].lexema != LEX_RIGHTHESIS && vec[i].lexema != LEX_PLUS && vec[i].lexema != LEX_MINUS && vec[i].lexema != LEX_MULTIPLICATION && vec[i].lexema != LEX_DIRSLASH && vec[i].lexema != LEX_PERSENT && vec[i].lexema != LEX_ID && vec[i].lexema != LEX_LITERAL && vec[i].lexema != LEX_LEFT && vec[i].lexema != LEX_RIGHT)
			{
				Log::WriteError(log.stream, Error::geterror(1));
				return false;
			}

		}

		while (!st.empty())
		{
			result.push_back(st.top());
			st.pop();
		}
		vec = result;

		return true;
	}

};
