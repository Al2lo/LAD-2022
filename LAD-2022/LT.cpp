#include "stdafx.h"


namespace LT
{
	Entry::Entry()
	{
		lexema = NULL;
		sn = NULL;
		idxTI = LT_TI_NULLIDX;
	}

	Entry::Entry(char lexema, int snn, int idxti)
	{
		this->lexema = lexema;
		this->sn = snn;
		this->idxTI = idxti;
	}

	LexTable Create(int size)
	{
		if (size > LT_MAXSIZE)
		{
			throw ERROR_THROW(202);
		}
		LexTable* lexTable = new LexTable;
		lexTable->maxsize = size;
		lexTable->size = 0;
		lexTable->table = new Entry[size];
		return *lexTable;
	}

	void Add(LexTable& lextable, Entry entry)
	{
		if (lextable.size + 1 > lextable.maxsize)
		{
			throw ERROR_THROW(202);
		}
		lextable.table[lextable.size++] = entry;
	}

	Entry GetEntry(LexTable& lextable, int n)
	{
		return lextable.table[n];
	}

	void Delete(LexTable& lextable)
	{
		delete[]lextable.table;
	}

	void writeLexTable(std::ostream* stream, LT::LexTable& lextable)
	{
		*stream << "------------------------------ ÒÀÁËÈÖÀ ËÅÊÑÅÌ  ------------------------\n" << std::endl;
		*stream << "|  N | ËÅÊÑÅÌÀ | ÑÒÐÎÊÀ | ÈÍÄÅÊÑ Â ÒÈ |" << std::endl;
		for (int i = 0; i < lextable.size; i++)
		{
			*stream << "|" << std::setw(3) << i << " | " << std::setw(4) << lextable.table[i].lexema << "    |  " << std::setw(3)
				<< lextable.table[i].sn << "   |";
			if (lextable.table[i].idxTI == -1)
				*stream << "             |" << std::endl;
			else
				*stream << std::setw(8) << lextable.table[i].idxTI << "     |" << std::endl;
		}
	}

	void writeLexemsOnLines(std::ostream* stream, LT::LexTable& lextable)
	{
		*stream << "\n-----------------  ËÅÊÑÅÌÛ ÑÎÎÒÂÅÒÑÒÂÓÞÙÈÅ ÈÑÕÎÄÍÎÌÓ ÊÎÄÓ ---------------------\n" << std::endl;
		for (int i = 0; i < lextable.size; )
		{
			int line = lextable.table[i].sn;
			*stream << std::setw(3) << line << " | ";
			while (lextable.table[i].sn == line)
			{
				*stream << lextable.table[i].lexema;
				if (lextable.table[i].idxTI != LT_TI_NULLIDX)
					*stream << "[" << lextable.table[i].idxTI << "]";
				i++;
			}
			*stream << std::endl;
		}
		*stream << "-------------------------------------------------------------------------\n\n";
	}
};