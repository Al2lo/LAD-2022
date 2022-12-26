#include "stdafx.h"


namespace Gener
{
	int point = 0;
	static int conditionsNum = 0;
	static int conditionsNumProverka = 0;

	void Head(std::ofstream* stream, LexAnalize::LEX table)
	{
		*stream << ".586\n";
		*stream << ".model flat, stdcall\n";
		*stream << "includelib libucrt.lib\n";
		*stream << "includelib kernel32.lib\n";
		*stream << "includelib ..\\LAD-2022\\Debug\\StaticLibrary.lib\n";
		*stream << "ExitProcess PROTO : DWORD\n";
		*stream << ".stack 4096\n\n";
		*stream << "printints PROTO : DWORD\n";
		*stream << "proverkaUint PROTO : DWORD\n";
		*stream << "proverkaDel PROTO : DWORD\n";
		*stream << "printstr PROTO : DWORD\n";
		*stream << "paused PROTO : DWORD, : DWORD\n";
		*stream << "sqrtNumb PROTO : DWORD, : DWORD\n";
		*stream << "powNumb PROTO : DWORD, : DWORD, : DWORD\n\n";
	}

	void getConstant(std::ofstream* stream, LexAnalize::LEX table)
	{
		*stream << ".CONST\n\t\tnewline byte 13, 10, 0\n proverka DWORD 0\n";
		for (int i = 0; i < table.idtab.size; i++)
		{
			IT::Entry e = table.idtab.table[i];

			if (e.idtype == IT::L)
			{
				*stream << "\t\t" << e.id;
				switch (e.iddatatype)
				{
				case IT::INT:
					*stream << " dword " << e.value.vint << " ;int";
					break;
				case IT::UINT:
					*stream << " dword " << e.value.vint << " ;uint";
					break;
				case IT::STR:
					*stream << " byte '" << e.value.vstr.str << "', 0 " << " ;str";
					break;
				case IT::CHAR:
					*stream << " byte '" << e.value.vstr.str << "', 0 " << " ;symbol";
					break;

				}
				*stream << "\n\n";
			}
		}
	}

	void getData(std:: ofstream* stream, LexAnalize::LEX table)
	{
		*stream << ".DATA\n\t\ttemp sdword ?\n\t\tbuffer byte 256 dup(0)\n";
		for (int i = 0; i < table.idtab.size; i++)
		{
			IT::Entry e = table.idtab.table[i];

			if (e.idtype == IT::V)
			{
				*stream << "\t\t" << e.id;
				switch (e.iddatatype)
				{
				case IT::INT:
					*stream << " dword 0 ;int";
					break;
				case IT::UINT:
					*stream << " dword 0 ;uint";
					break;
				case IT::CHAR:
					*stream << " dword ? ;symbol";
					break;
				case IT::STR:
					*stream << " dword ? ;str";
					break;
				}
				*stream << "\n\n";
			}
		}
	}

	void getFuncCode(std::ofstream* stream, LexAnalize::LEX table, int i)
	{
		IT::Entry e = table.idtab.table[table.lextab.table[i + 1].idxTI];
		IT::IDDATATYPE type = e.iddatatype;

		*stream << "\n;-------" << e.id << "--------\n\n";
		*stream << e.id<<"_" << " PROC,\n\t";
		//дальше параметры
		int j = i + 3;
		int count = 0; // параметр по счету
		while (table.lextab.table[j].lexema != LEX_RIGHTHESIS)
		{
			if (table.lextab.table[j].lexema == LEX_ID) // параметр
			{
				if (count == 0)
				{
					*stream << table.idtab.table[table.lextab.table[j].idxTI].id << (table.idtab.table[table.lextab.table[j].idxTI].iddatatype == IT::INT ? " : sdword" : " : dword");
					count++;
				}
				else
				{
					*stream <<"\, " << table.idtab.table[table.lextab.table[j].idxTI].id << (table.idtab.table[table.lextab.table[j].idxTI].iddatatype == IT::INT ? " : sdword" : " : dword");
				}
			}
			j++;
		}
		*stream << "\n;---------save register ----\npush ebx\npush edx\n;-----------\n";
	}

	void getExitCode(std::ofstream* stream, LexAnalize::LEX table, int i, char* funcName)
	{
		*stream << ";----- restore registers -----\npop edx\npop ebx\n;------------------\n";
		if (table.lextab.table[i + 1].lexema != LEX_SEMICOLON)// выход из функции (вернуть значение)
		{
			if (table.idtab.table[table.lextab.table[i + 1].idxTI].iddatatype == IT::CHAR || table.idtab.table[table.lextab.table[i + 1].idxTI].iddatatype == IT::STR)
				*stream << "mov eax, offset " << table.idtab.table[table.lextab.table[i + 1].idxTI].id << "\n";
			else
				*stream << "mov eax, " << table.idtab.table[table.lextab.table[i + 1].idxTI].id << "\n";
		}
		*stream << "ret\n";
		*stream << funcName <<"_" << " ENDP" << "\n\n;--------------\n";
	}

	void getConditionsCode(std::ofstream* stream, LexAnalize::LEX table, int i, char* cycleCode)
	{
		conditionsNum++;
		IT::Entry left = table.idtab.table[table.lextab.table[i + 2].idxTI];
		IT::Entry rght = table.idtab.table[table.lextab.table[i + 4].idxTI];
		strcpy(cycleCode, "");
		bool w = false;
		bool r = false;
		bool c = false;
		char wstr[256] = "";
		char rstr[256] = "";

		for (int j = i+7; table.lextab.table[j].lexema != LEX_DIEZ; j++)// пропустили открывающую решетку
		{
			if (table.lextab.table[j].lexema == LEX_ISTRUE)
				w = true;
			if (table.lextab.table[j].lexema == LEX_ISFALSE)
				r = true;
			if (table.lextab.table[j].lexema == LEX_CYCLE)
				c = true;
		}
		if (left.iddatatype == IT::INT || left.iddatatype == IT::UINT || (left.iddatatype == IT::STR && table.lextab.table[i + 2].lexema != LEX_LITERAL))
		{
			*stream << "mov edx, " << left.id << "\ncmp edx, " << rght.id << "\n";
			strcpy(cycleCode, "mov edx , ");
			strcat(cycleCode, left.id);
			strcat(cycleCode, "\ncmp edx, ");
			strcat(cycleCode, rght.id);
		}
		else
		{
			*stream << "mov edx, offset " << left.id << "\ncmp edx, offset " << rght.id << "\n";
			strcpy(cycleCode, "mov edx, offset ");
			strcat(cycleCode, left.id);
			strcat(cycleCode, "\ncmp edx, offset ");
			strcat(cycleCode, rght.id);
		}
		switch (table.lextab.table[i + 3].lexema)
		{
		case LEX_MORE:
		{
			strcpy_s(rstr, "jg");
			strcpy_s(wstr, "jl");
			break;
		}
		case LEX_LESS:
		{
			strcpy_s(rstr, "jl");
			strcpy_s(wstr, "jg");
			break;
		}
		case LEX_EQUAL:
		{
			strcpy_s(rstr, "jz");
			strcpy_s(wstr, "jnz");
			break;
		}
		case LEX_DONT_EQUAL:
		{
			strcpy_s(rstr, "jnz");
			strcpy_s(wstr, "jz");
			break;
		}
		case LEX_MORE_AND_EQAL:
		{
			strcpy_s(rstr, "jge");
			strcpy_s(wstr, "jle");
			break;
		}
		case LEX_LESS_AND_EQAL:
		{
			strcpy_s(rstr, "jle");
			strcpy_s(wstr, "jge");
			break;
		}
		}

		if (!c && w)
			*stream << "\n" << rstr << " right" << conditionsNum << "\n";
		if (!c && r)
			*stream << "\n" << wstr << " wrong" << conditionsNum << "\n";
		if (c)
		{
			*stream << "\n" << rstr << " round" << conditionsNum << "\n";
			strcat(cycleCode, "\n\n");
			strcat(cycleCode, rstr);
			strcat(cycleCode, " round");
			char* buff = new char[255];
			itoa(conditionsNum, buff, 10);
			strcat(cycleCode,buff);
 			*stream << "\njmp roundnext" << conditionsNum << "\n";
		}
		else if (!r || !w)
			*stream << "\njmp roundnext" << conditionsNum << "\n";
	}

	void genCallFuncCode(std::ofstream* stream, LexAnalize::LEX& table, int i)
	{
		IT::Entry e = table.idtab.table[table.lextab.table[i].idxTI];
		stack<IT::Entry> temp;
		bool standart = (e.idtype == IT::S);

		for (int j = i + 1; table.lextab.table[j].lexema != LEX_RIGHTHESIS; j++)
		{
			if (table.lextab.table[j].lexema == LEX_ID || table.lextab.table[j].lexema == LEX_LITERAL || table.lextab.table[j].lexema == LEX_LITERAL_16)
				temp.push(table.idtab.table[table.lextab.table[j].idxTI]);
		}
		*stream << "\n";

		// раскручиваем стек
		while (!temp.empty())
		{
			if (temp.top().idtype == IT::L && (temp.top().iddatatype == IT::STR || temp.top().iddatatype == IT::IDDATATYPE::CHAR))
			{
				*stream << "push offset " << temp.top().id << "\n";
			}
			else
			{
				*stream << "push " << temp.top().id << "\n";
			}
			temp.pop();
		}

		if (standart)
			*stream << "push offset buffer\n";

		if (e.idtype == IT::S && (strcmp(e.id,"pow") == 0|| strcmp(e.id, "sqrt") == 0))
			*stream << "call " << e.id << "Numb" << "\n";
		else
			*stream << "call " << e.id<<"_" << "\n";
	}

	void genEqualCode(ofstream* stream, LexAnalize::LEX table, int i, int var)
	{
		IT::Entry e1 = table.idtab.table[table.lextab.table[i - 1].idxTI];
		switch (e1.iddatatype)
		{
		case IT::IDDATATYPE::INT: 
		{
			bool first = true;
			for (int j = i + 1; table.lextab.table[j].lexema != LEX_SEMICOLON; j++)
			{
				switch (table.lextab.table[j].lexema)
				{
				case LEX_LITERAL:
				case LEX_LITERAL_16:
				case LEX_ID:
				{
					if (table.idtab.table[table.lextab.table[j].idxTI].idtype == IT::F || table.idtab.table[table.lextab.table[j].idxTI].idtype == IT::S)
					{
						genCallFuncCode(stream, table, j);
						*stream << "push eax\n";
						while (table.lextab.table[j].lexema != LEX_RIGHTHESIS)
							j++;
						break;
					}
					else
						*stream << "push " << table.idtab.table[table.lextab.table[j].idxTI].id << "\n";
					break;
				}
				case LEX_PLUS:
				{
					*stream << "pop ebx\npop eax\nadd eax, ebx\npush eax\n";
					break;
				}
				case LEX_MINUS:
				{
					*stream << "pop ebx\npop eax\nsub eax, ebx\npush eax\n";
					break;
				}
				case LEX_MULTIPLICATION:
				{
					*stream << "pop ebx\npop eax\nimul eax, ebx\npush eax\n";
					break;
				}
				case LEX_DIRSLASH:
				{				
					*stream << "\npush eax\ncall proverkaDel\n\n";
					*stream << "cmp eax, proverka\njz rrright" << conditionsNumProverka << "\njnz wwwrong" << conditionsNumProverka << "\nrrright" << conditionsNumProverka << ":\n" << "push 0\ncall ExitProcess\n\n" << "wwwrong" << conditionsNumProverka << ":\n";
					conditionsNumProverka++;
					//
					*stream << "pop ebx\npop eax\ncdq\nidiv ebx\npush eax\n";
					//
					break;
				}
				case LEX_PERSENT:
				{
					*stream << "pop ebx\npop eax\ncdq\nmov edx,0\nidiv ebx\npush edx\n";
					break;
				}
				case LEX_RIGHT:
				{
					*stream << "pop ebx \npop eax \nmov cl, bl \nshr eax, cl\npush eax\n";
					break;
				}
				case LEX_LEFT:
				{
					*stream << "pop ebx \npop eax \nmov cl, bl \nshl eax, cl\npush eax\n";
					break;
				}
				}
			}// цикл вычисления
			*stream<<  "\npop ebx\nmov "<< e1.id << ", ebx\n";
			break;
		}
		case IT::UINT:
		{
			bool first = true;
			for (int j = i + 1; table.lextab.table[j].lexema != LEX_SEMICOLON; j++)
			{
				switch (table.lextab.table[j].lexema)
				{
				case LEX_LITERAL:
				case LEX_LITERAL_16:
				case LEX_ID:
				{
					if (table.idtab.table[table.lextab.table[j].idxTI].idtype == IT::F || table.idtab.table[table.lextab.table[j].idxTI].idtype == IT::S )
					{
						genCallFuncCode(stream, table, j);
						*stream << "push eax\n";
						while (table.lextab.table[j].lexema != LEX_RIGHTHESIS)
							j++;
						break;
					}
					else
						*stream << "push " << table.idtab.table[table.lextab.table[j].idxTI].id << "\n";
					break;
				}
				case LEX_PLUS:
				{
					*stream << "pop ebx\npop eax\nadd eax, ebx\npush eax\n";
					break;
				}
				case LEX_MINUS:
				{
					//*stream << "pop ebx\npop eax\nsub eax, ebx\njnc negat" << point << "\nneg eax\nnegat" << point << ":\npush eax\n";
					*stream << "pop ebx\npop eax\nsub eax, ebx\npush eax\n";
					point++;
					break;
				}
				case LEX_MULTIPLICATION:
				{
					*stream << "pop ebx\npop eax\nimul eax, ebx\npush eax\n";
					break;
				}
				case LEX_DIRSLASH:
				{
					*stream << "\npush eax\ncall proverkaDel\n\n";
					*stream << "cmp eax, proverka\njz rrright" << conditionsNumProverka << "\njnz wwwrong" << conditionsNumProverka << "\nrrright" << conditionsNumProverka << ":\n" << "push 0\ncall ExitProcess\n\n" << "wwwrong" << conditionsNumProverka << ":\n";
					conditionsNumProverka++;
					*stream << "pop ebx\npop eax\ncdq\nidiv ebx\npush eax\n";
					break;
				}
				case LEX_PERSENT:
				{
					*stream << "pop ebx\npop eax\ncdq\nmov edx,0\nidiv ebx\npush edx\n";
					break;
				}
				case LEX_RIGHT:
				{
					*stream << "pop ebx \npop eax \nmov cl, bl \nshr eax, cl\npush eax\n";
					break;
				}
				case LEX_LEFT:
				{
					*stream << "pop ebx \npop eax \nmov cl, bl \nshl eax, cl\npush eax\n";
					break;
				}
				}
			}
			*stream << "\npop ebx\nmov " << e1.id << ", ebx\n";
			*stream << "\npush "<<e1.id << "\ncall proverkaUint\n\n";
			*stream << "cmp eax, proverka\njz rright" << conditionsNumProverka << "\njnz wwrong" << conditionsNumProverka << "\nrright" << conditionsNumProverka << ":\n" << "push 0\ncall ExitProcess\n\n" << "wwrong" << conditionsNumProverka<<":\n";
			conditionsNumProverka++;
			break;
		}
		case IT::STR:
		{
			char lex = table.lextab.table[i + 1].lexema;
			IT::Entry e2 = table.idtab.table[table.lextab.table[i + 1].idxTI];
			if (lex == LEX_ID && (e2.idtype == IT::IDTYPE::F || e2.idtype == IT::IDTYPE::S))
			{
				genCallFuncCode(stream, table, i + 1);
				*stream << "mov " << e1.id << ", eax" << "\n";
			}
			else if (lex == LEX_LITERAL)
			{
				*stream << "mov " << e1.id << ", offset " << e2.id << "\n";
			}
			else // ид(переменная) - через регистр
			{
				*stream << "mov ecx, " << e2.id << "\nmov " << e1.id << ", ecx" << "\n";
			}
			break;
		}
		case IT::CHAR:
		{
			char lex = table.lextab.table[i + 1].lexema;
			IT::Entry e2 = table.idtab.table[table.lextab.table[i + 1].idxTI];
			if (lex == LEX_ID && (e2.idtype == IT::IDTYPE::F || e2.idtype == IT::IDTYPE::S))
			{
				genCallFuncCode(stream, table, i + 1);
				*stream << "mov " << e1.id << ", eax";
			}
			else if (lex == LEX_LITERAL)
			{
				*stream << "mov " << e1.id << ", offset " << e2.id;
			}
			else // ид(переменная) - через регистр
			{
				*stream << "mov ecx, " << e2.id << "\nmov " << e1.id << ", ecx";
			}
			break;
		}
		}
	}

	void codeGeneration(ofstream* stream, LexAnalize::LEX& table)
	{
		char cycle[255];
		Head(stream, table);
		getConstant(stream, table);
		getData(stream, table);
		*stream << ".code\n\n";
		//ofstream ofile("D:\\KPOkyrs\\OusAsmFile\\outFile.asm");
		char funcName[255];
		char cyclecod[255];
		int pCount;

		for (int i = 0; i < table.lextab.size; i++)
		{
			switch (table.lextab.table[i].lexema)
			{
			case LEX_MAINFUNC:
			{
				*stream << "\n;--------------" << "MAIN" << "------------\n" << "main PROC\n";
				break;

			}
			case LEX_FUNCTION:
			{
				strcpy_s(funcName, table.idtab.table[table.lextab.table[i + 1].idxTI].id);
				pCount = table.idtab.table[table.lextab.table[i + 1].idxTI].value.params.count;
				getFuncCode(stream, table, i);
				break;
			}
			case LEX_RETURN:
			{
				getExitCode(stream, table, i, funcName);
				break;
			}
			case LEX_ID:
			{
				if (table.lextab.table[i + 1].lexema == LEX_LEFTHESIS && table.lextab.table[i - 1].lexema != LEX_FUNCTION)// вызов функции
				{
					genCallFuncCode(stream, table, i);
				}
				break;
			}
			case LEX_STATE:
			{
				getConditionsCode(stream, table, i, cycle);
				break;
			}
			case LEX_RIGHTBRACE:	// переход на метку в конце кондишна
			{
				if (table.lextab.table[i + 1].lexema == LEX_ISFALSE || table.lextab.table[i + 1].lexema == LEX_ISTRUE)
				{
					*stream << "jmp roundnext" << conditionsNum<<"\n";
				}
			}
			case LEX_DIEZ:		// поставить метки в конце кондишна
			{
				if (table.lextab.table[i - 1].lexema == LEX_RIGHTBRACE)
				{
					bool c = false;
					for (int j = i; j > 0 && table.lextab.table[j].lexema != LEX_STATE; j--)
					{
						if (table.lextab.table[j].lexema == LEX_CYCLE)
						{
							c = true;
						}
					}
					if (c)
					{
						*stream << cycle << "\nroundnext" << conditionsNum << ":";
					}
					else
					{
						*stream << "roundnext" << conditionsNum << ":" << "\n";
					}
				}
				break;
			}
			case LEX_ISTRUE:
			{
				*stream << "right" << conditionsNum << ":" << "\n";
				break;
			}
			case LEX_ISFALSE: // условие неверно(метка)
			{
				*stream << "wrong" << conditionsNum << ":" << "\n";
				break;
			}
			case LEX_CYCLE: // цикл с условием (метка)
			{
				*stream << "round" << conditionsNum << ":"<<"\n";
				break;
			}
			case LEX_ASSIGN: // присваивание (вычисление выражений)
			{
				genEqualCode(stream, table, i, i - 1);
				while (table.lextab.table[++i].lexema != LEX_SEMICOLON);	// пропускаем выражение
				break;
			}
			case LEX_NEWLINE: // перевод строки 
			{
				*stream << "push offset newline\ncall printrow\n";
				break;
			}
			case LEX_PRINT: // вывод
			{
				IT::Entry e = table.idtab.table[table.lextab.table[i + 1].idxTI];
				switch (e.iddatatype)
				{
				case IT::IDDATATYPE::INT:
					*stream << "\npush " << e.id << "\ncall printints\n";
					break;
				case IT::IDDATATYPE::UINT:
					*stream << "\npush " << e.id << "\ncall printints\n";
					break;
				case IT::IDDATATYPE::STR:
					if (e.idtype == IT::IDTYPE::L)*stream << "\npush offset " << e.id << "\ncall printstr\n";
					else*stream << "\npush " << e.id << "\ncall printstr\n";
					break;
				case IT::IDDATATYPE::CHAR:
					if (e.idtype == IT::IDTYPE::L) *stream << "\npush offset " << e.id << "\ncall printstr\n";
					else *stream << "\npush " << e.id << "\ncall printstr\n";
					break;
				}
				break;
			}

			}
		}
		*stream << "\npush 0\ncall ExitProcess\nmain ENDP\nend main";
	}

















}