#include "stdafx.h"

#pragma warning(disable : 4996 )


struct Entry {
	char* name;
};

struct Scope {
	int id = 0;
	int size = 10;
	Entry* scope = new Entry[size];
};

void Add(Scope* scope, char* str)
{
	scope->scope[scope->id].name = str;
	scope->id++;
}

int decimalNotation(string input, int scaleOfNot)
{
	string lich = input.substr(1, input.size());
	int n = stoi(lich, nullptr, scaleOfNot);
	return stoi(lich,nullptr,scaleOfNot);
}
namespace m {
	bool main = false;
}
namespace LexAnalize
{
	Graph graphs[] =
	{
		{ LEX_SEPARATORS, FST::FST(GRAPH_SEPARATORS) },
		{ LEX_LITERAL, FST::FST(GRAPH_INT_LITERAL) },
		{ LEX_LITERAL, FST::FST(GRAPH_STRING_LITERAL) },
		{ LEX_LITERAL, FST::FST(GRAPH_CHAR_LITERAL) },
		{ LEX_DECLARE, FST::FST(GRAPH_NEW) },
		{ LEX_MAINFUNC, FST::FST(GRAPH_MAIN) },
		{ LEX_TYPE, FST::FST(GRAPH_INT) },
		{ LEX_TYPE, FST::FST(GRAPH_UINT) },
		{ LEX_TYPE, FST::FST(GRAPH_STRING) },
		{ LEX_TYPE, FST::FST(GRAPH_CHAR) },
		{ LEX_FUNCTION, FST::FST(GRAPH_FUNCTION) },
		{ LEX_PROC, FST::FST(GRAPH_PROCEDURE) },
		{ LEX_RETURN, FST::FST(GRAPH_RETURN) },
		{ LEX_PRINT, FST::FST(GRAPH_WRITE) },
		{ LEX_NEWLINE, FST::FST(GRAPH_NEWLINE) },
		{ LEX_STATE, FST::FST(GRAPH_STATE) },
		{ LEX_CYCLE, FST::FST(GRAPH_CYCLE) },
		{ LEX_ISFALSE, FST::FST(GRAPH_ISFALSE) },
		{ LEX_ISTRUE, FST::FST(GRAPH_ISTRUE) },
		{ LEX_LITERAL_16, FST::FST(GRAPH_HEX_LITERAL) },
		{ LEX_ID, FST::FST(GRAPH_ID) }
	};


	char* getScopeName(IT::IdTable idtable, char* prevword)// имя текущей области видимости (если находимся внутри блока)
	{
		char* main = new char[5];
		main[0] = 'm';
		main[1] = 'a';
		main[2] = 'i';
		main[3] = 'n';
		main[4] = '\0';
		if (strcmp(prevword,MAIN) == 0)
		{
			m::main = true;
			return main;
		}
		if (m::main == true)
		{
			return main;
		}

		for (int i = idtable.size - 1; i >=0; i--)
		{
			if(idtable.table[i].idtype == IT::IDTYPE::F)
			{
				break;
			}
		}
		for (int i = idtable.size-1; i >= 0; i--)
		{
			if (idtable.table[i].idtype == IT::IDTYPE::F)
			{
				return idtable.table[i].id;
			}
		}
		return nullptr; // не найдено имя функции
	}
	 

	int getLiteralIndex(IT::IdTable idtable, char* value, IT::IDDATATYPE type, Log::LOG log, int line)// получаем индекс повторного литерала(по значению)
	{
		for (int i = 0; i < idtable.size; i++)
		{
			if (idtable.table[i].idtype == IT::IDTYPE::L && idtable.table[i].iddatatype == type)
			{
				switch (type)
				{
					case IT::INT: 
					{
						if (idtable.table[i].value.vint == atoi(value))
						{
							return i;
						}
						break;
					}
					case IT::STR:
					{
						char buff[TI_STR_MAXSIZE];
						for (unsigned int j = 1; j < strlen(value) - 1; j++)// без кавычек
						{
							buff[j - 1] = value[j];
						}
						buff[strlen(value) - 2] = '\0';
						if (strcmp(idtable.table[i].value.vstr.str, buff) == 0)
						{
							return i;
						}
						break;
					}
					case IT::CHAR:
					{
						char buff[TI_STR_MAXSIZE];
						for (unsigned int j = 1; j < strlen(value) -1; j++)// без кавычек
						{
							buff[j-1] = value[j];
						}
						buff[strlen(value) - 2] = '\0';
						if (strcmp(idtable.table[i].value.vstr.str, buff) == 0)
							return i;
						break;
					}
					case IT::UINT:
					{
						if (atoi(value) < 0)
						{
							Log::WriteError(log.stream, Error::geterrorin(319, line, 0));
						}
						if (idtable.table[i].value.vint == atoi(value))
						{
							return i;
						}
						break;
					}
				}
			}
		}
		return LT_TI_NULLIDX;
	}

	IT::IDDATATYPE getType(char* curword, char* idtype)
	{
		if (!strcmp(TYPE_PROCEDURE, idtype))
			return IT::IDDATATYPE::PROC; // процедуры
		if (!strcmp(TYPE_STRING, idtype))
			return IT::IDDATATYPE::STR;  // строковый ид
		if (!strcmp(TYPE_CHAR, idtype))
			return IT::IDDATATYPE::CHAR;  // символьный ид
		if (!strcmp(TYPE_INT, idtype))
			return IT::IDDATATYPE::INT;	 // числовой  ид
		if (!strcmp(TYPE_UINT, idtype))
			return IT::IDDATATYPE::UINT;	 // беззнаковый числовой  ид
		if (isdigit(*curword) || *curword == LEX_MINUS)
			return IT::IDDATATYPE::INT;				// числовой литерал
		else if (*curword == '\"')
			return IT::IDDATATYPE::STR;	// строковый литерал
		else if (*curword == '\'')
			return IT::IDDATATYPE::CHAR;	// символьный литерал

		return IT::IDDATATYPE::UNDEF;		// неопределенный тип, индикатор ошибки
	}

	int getIndexInLT(LT::LexTable& lextable, int itTableIndex)// индекс первой встречи в таблице лексем
	{
		if (itTableIndex == LT_TI_NULLIDX)
		{
			return lextable.size;
		}
		for (int i = 0; i < lextable.size; i++)
		{
			if (itTableIndex == lextable.table[i].idxTI)
			{
				return i;
			}
		}
		return LT_TI_NULLIDX;
	}

	bool isLiteral(char* id)
	{
		if (isdigit(*id) || *id == '\"' || *id == '\'' || *id == LEX_MINUS || strcmp(id, "f") == 0)
		{
			return true;
		}
		return false;
	}

	IT::STD_FUNC getStandartFunction(char* id)
	{
		if (!strcmp(POW,id))
		{
			return IT::F_POW;
		}
		else if (!strcmp(CONCAT,id))
		{
			return IT::F_CONCAT;
		}
		else if (!strcmp(PAUSE, id))
		{
			return IT::F_PAUSE;
		}
		else if (!strcmp(SQRT, id))
		{
			return IT::F_SQRT;
		}
		return IT::F_NOT_STD;
	}


	char* getNextLiteralName()						// сгенерировать следующее имя литерала
	{
		static int literalDigit = 1;
		char* buff = new char[SCOPED_ID_MAXSIZE];
		char embl[3];
		strcpy_s(buff, ID_MAXSIZE,"LITRL");
		_itoa_s(literalDigit++, embl, 10);
		strcat(buff, embl);
		return buff;
	}

	IT::Entry* getEntry(		// формирует и возвращает строку ТИ
		LexAnalize::LEX& tables,						// ТЛ + ТИ
		char lex,								// лексема
		char* id,								// идентификатор
		char* idtype,							// предыдущая (тип)
		bool isParam,							// признак параметра функции
		bool isFunc,							// признак функции
		Log::LOG log,							// протокол
		int line,								// строка в исходном тексте
		bool& lex_good)							// флаг ошибки(по ссылке)
		{
		// тип данных
			IT::IDDATATYPE type = getType(id, idtype);
			int index = IT::IsId(tables.idtab, id);// индекс существующего идентификатора или -1
			if (lex == LEX_LITERAL)
			{
				index = getLiteralIndex(tables.idtab,id,type,log,line);
			}
			if (index != LT_TI_NULLIDX)// уже существует
			{
				return nullptr;
			}

			IT::Entry* itEntry = new IT::Entry;
			itEntry->iddatatype = type;// сохраняем тип данных
			
			itEntry->idxfirstLE = getIndexInLT(tables.lextab, index);// индекс первой строки в таблице лексем 

			if (lex == LEX_LITERAL)
			{
				bool int_good = IT::SetValue(itEntry, id);
				if (!int_good)
				{
					// превышен максимальный размер числового литерала
					Log::WriteError(log.stream, Error::geterrorin(313, line, 0));
					lex_good = false;
				}
				if (itEntry ->iddatatype == IT::UINT && itEntry->value.vint < 0)
				{
					//несоответствие беззнаковому типу 
					Log::WriteError(log.stream, Error::geterrorin(319, line, 0));
					lex_good = false;
				}
				if ((itEntry->iddatatype == IT::IDDATATYPE::CHAR || itEntry->iddatatype == IT::IDDATATYPE::STR) && itEntry->value.vstr.len == 0)
				{
					// пустой строковый литерал
					Log::WriteError(log.stream, Error::geterrorin(310, line, 0));
					lex_good = false;
				}
				strcpy_s(itEntry->id, getNextLiteralName());
				itEntry->idtype = IT::L;
			}
			else// идентификатор 
			{
				switch (type)
				{
				case IT::INT:
					itEntry->value.vint = TI_INT_DEFAULT;
					break;
				case IT::STR:
					strcpy_s(itEntry->value.vstr.str, "");
					itEntry->value.vstr.len = TI_STR_DEFAULT;
					break;
				case IT::CHAR:
					strcpy_s(itEntry->value.vstr.str, "");
					itEntry->value.vstr.len = TI_STR_DEFAULT;
					break;
				case IT::UINT:
					itEntry->value.vint = TI_INT_DEFAULT;
					break;
				}

					if (isFunc)
					{
						switch (getStandartFunction(id))
						{
							case IT::F_POW:
							{
								itEntry->idtype = IT::S;
								itEntry->iddatatype = POW_TYPE_UINT;
								itEntry->value.params.count = POW_PARAMS_CNT;
								itEntry->value.params.types = new IT::IDDATATYPE[POW_PARAMS_CNT];
								for (int j = 0; j < POW_PARAMS_CNT; j++)
									itEntry->value.params.types[j] = IT::POW_PARAMS_UINT[j];
								break;
							}
							case IT::F_CONCAT:
							{
								itEntry->idtype = IT::IDTYPE::S;
								itEntry->iddatatype = CONCAT_TYPE;
								itEntry->value.params.count = CONCAT_PARAMS_CNT;
								itEntry->value.params.types = new IT::IDDATATYPE[CONCAT_PARAMS_CNT];
								for (int k = 0; k < CONCAT_PARAMS_CNT; k++)
									itEntry->value.params.types[k] = IT::CONCAT_PARAMS[k];
								break;
							}
							case IT::F_SQRT:
							{
								itEntry->idtype = IT::IDTYPE::S;
								itEntry->iddatatype = SQRT_TYPE_UINT;
								itEntry->value.params.count = LENGHT_PARAMS_SQRT ;
								itEntry->value.params.types = new IT::IDDATATYPE[LENGHT_PARAMS_SQRT ];
								for (int k = 0; k < LENGHT_PARAMS_SQRT; k++)
									itEntry->value.params.types[k] = IT::SQRT_PARAMS_UINT[k];
								break;
							}
							case IT::F_PAUSE:
							{
								itEntry->idtype = IT::IDTYPE::S;
								itEntry->iddatatype = PAUSE_TYPE_UINT;
								itEntry->value.params.count = PAUSE_PARAMS_CNT;
								itEntry->value.params.types = new IT::IDDATATYPE[PAUSE_PARAMS_CNT];
								for (int k = 0; k < PAUSE_PARAMS_CNT; k++)
									itEntry->value.params.types[k] = IT::PAUSE_PARAMS_UINT[k];
								break;
							}
							case IT::F_NOT_STD:
								itEntry->idtype = IT::IDTYPE::F;
								break;
						}
					}
					else if (isParam)
					{
						itEntry->idtype = IT::P;
					}
					else
					{
						itEntry->idtype = IT::IDTYPE::V;
					}
					strncpy_s(itEntry->id, id, SCOPED_ID_MAXSIZE);
			}
			
			int i = tables.lextab.size;// индекс в ТЛ текущего ИД
			if (i >= 1 && ((itEntry->idtype == IT::IDTYPE::F && tables.lextab.table[i - 1].lexema != LEX_FUNCTION) || (itEntry->idtype == IT::IDTYPE::V && tables.lextab.table[i - 1].lexema != LEX_TYPE)) )
			{
				// в объявлении не указан тип функции
				Log::WriteError(log.stream, Error::geterrorin(303, line, 0));
				lex_good = false;
			}
			if (i > 1 && itEntry->idtype == IT::V && tables.lextab.table[i-2].lexema != LEX_DECLARE)
			{
				cout << tables.lextab.table[i].lexema;
				// в объявлении отсутствует ключевое слово new
				Log::WriteError(log.stream, Error::geterrorin(304, line, 0));
				lex_good = false;
			}
			if (itEntry->iddatatype == IT::IDDATATYPE::UNDEF)
			{
				// невозможно определелить тип
				Log::WriteError(log.stream, Error::geterrorin(300, line, 0));
				lex_good = false;
			}
			return itEntry;
	}


	bool LexAnalize(LEX& tables, In::IN& in, Log::LOG& log, Parm::PARM& parm)
	{
		Scope* sc = new Scope;
		static bool lex_good = true;
		tables.lextab = LT::Create(LT_MAXSIZE);
		tables.idtab = IT::Create(TI_MAXSIZE);

		bool isParam = false, isFunc = false;
		int enterPoint = NULL;
		char curword[TI_STR_MAXSIZE], nextword[TI_STR_MAXSIZE], prevword[TI_STR_MAXSIZE];
		int curline;
		std::stack <char*> scopes;			// стек для хранения имени текущей области видимости

		for (int i = 0; i < In::inWords::size; i++)
		{
			strcpy_s(curword, in.words[i].word);
			if (i < In::inWords::size - 1)
				strcpy_s(nextword, in.words[i + 1].word);
			if (i > 1)
				strcpy_s(prevword, in.words[i - 1].word);//strcpy_s(prevword, in.words[i - 2].word);
			curline = in.words[i].line;
			isFunc = false;
			int idxTI = TI_NULLIDX;

			for (int j = 0; j < N_GRAPHS; j++)
			{
				FST::FST fst(curword, graphs[j].graph);
				if (FST::execute(fst))
				{
					char lexema = graphs[j].lexema;
					switch (lexema)
					{
					case LEX_MAINFUNC:
						enterPoint++;
						break;
					case LEX_SEPARATORS:
					{
						switch (*curword)
						{
						case LEX_LEFTHESIS:		// открывающая скобка - параметры  функции
						{
							if (strcmp(prevword, "function") == 0)
							{
								Log::WriteError(log.stream, Error::geterrorin(614, curline, 0));
								lex_good = false;
							}
							isParam = true;
							if (*nextword == LEX_RIGHTHESIS || ISTYPE(nextword))
							{
								char* functionname = new char[ID_MAXSIZE];
								char* scopename = getScopeName(tables.idtab, in.words[i - 1].word);
								if (scopename == nullptr)
									break;
								strcpy_s(functionname, ID_MAXSIZE, scopename);
								scopes.push(functionname);
							}
							break;
						}
						case LEX_RIGHTHESIS:	// закрывающая скобка
						{
							isParam = false;
							// конец области видимости
							if (*in.words[i - 1].word == LEX_LEFTHESIS || (i > 2 && (tables.lextab.table[tables.lextab.size - 2].lexema == LEX_TYPE)))
								scopes.pop();
							break;
						}
						case LEX_LEFTBRACE:		// начало области видимости
						{
							if (i > 0 && *in.words[i - 1].word == LEX_ISFALSE || *in.words[i - 1].word == LEX_ISTRUE || *in.words[i - 1].word == LEX_CYCLE)
							{
								break;
							}
							char* functionname = new char[ID_MAXSIZE];
							char* scopename = getScopeName(tables.idtab, in.words[i - 1].word);
							if (scopename == nullptr)  break;
							strcpy_s(functionname, ID_MAXSIZE, scopename);
							scopes.push(functionname);
							break;
						}
						case LEX_RIGHTBRACE:		// конец области видимости
						{
							// только в этом случае закрываем область видимости
							if (*in.words[i + 1].word == LEX_TYPE || *in.words[i + 1].word == LEX_PROC || *in.words[i + 1].word == LEX_MAINFUNC)
							{
								if (!scopes.empty())
									scopes.pop();
								
							}
							break;
						}
						}
						lexema = *curword;
						break;
					}
					case LEX_LITERAL_16:
					{
						int value;
						value = decimalNotation(curword, 16);
						
						if (value > INT_MAXSIZE || value < INT_MINSIZE)
							throw ERROR_THROW(204);
						tables.idtab.table[tables.idtab.size - 1].value.vint = value;
						int x = 0;
						char* str = (char*)malloc(10 * sizeof(char));
						while (value > 9) {
							str[x++] = (value % 10) + '0';
							value = value / 10;
						}
						str[x++] = value + '0';
						str[x] = '\0';
						char t;
						for (int u = 0; u < x / 2; u++) {
							t = str[u];
							str[u] = str[x - 1 - u];
							str[x - 1 - u] = t;
						}
						strcpy_s(curword, "");
						strcat(curword, str);
						lexema = 'l';
					}
					case LEX_ID:
					case LEX_LITERAL:
					{
						int  flag = false;
						char id[TI_STR_MAXSIZE] = "";
						if (!(idxTI = IT::IsId(tables.idtab, id)))
						{
							idxTI = TI_NULLIDX;  // индекс идентификатора в ТИ
						}
						if (*nextword == LEX_LEFTHESIS)
							isFunc = true;						// идентификатор функции
						char* idtype = (isFunc && i > 1) ?	// тип идентификатора
						in.words[i - 2].word : in.words[i - 1].word;		// пропускаем ключевое слово function
							for (int i = 0; i < sc->id; i++)
								if (strcmp(sc->scope[i].name, curword) == 0) { flag = true; }
							if (!flag && !isFunc && !scopes.empty())
							{
								strncpy_s(id, scopes.top(), ID_MAXSIZE);
								strcat(id, "_");
							} 
						strncat(id, curword, ID_MAXSIZE);
						if (isLiteral(curword))
							strcpy_s(id, curword); // литерал заменяется своим значением

						IT::Entry* itentry = getEntry(tables, lexema, id, idtype, isParam, isFunc, log, curline, lex_good);
						if (itentry != nullptr) // первая встреча - объявление
						{
							if (isFunc) // если функция - сохранить список параметров
							{
								if (getStandartFunction(id) == IT::F_NOT_STD) // стандартная функция или нет
								{
									itentry->value.params.count = NULL;
									itentry->value.params.types = new IT::IDDATATYPE[MAX_PARAMS_COUNT];
									for (int k = i; in.words[k].word[0] != LEX_RIGHTHESIS; k++)
									{
										if (k == In::inWords::size || in.words[k].word[0] == LEX_SEMICOLON)
											break;
										if (ISTYPE(in.words[k].word))
										{
											if (itentry->value.params.count >= MAX_PARAMS_COUNT)
											{
												Log::WriteError(log.stream, Error::geterrorin(306, curline, 0));
												lex_good = false;
												break;
											}
											itentry->value.params.types[itentry->value.params.count++] = getType(NULL, in.words[k].word);
										}
									}
								}
							}
							IT::Add(tables.idtab, *itentry);
							idxTI = tables.idtab.size - 1;
						}
						else // повторный идентификатор (уже есть)
						{
							int i = tables.lextab.size - 1; // попытка переопределить идентификатор
							if (i > 0 && tables.lextab.table[i - 1].lexema == LEX_DECLARE || tables.lextab.table[i].lexema == LEX_DECLARE
								|| tables.lextab.table[i - 1].lexema == LEX_FUNCTION ||  tables.lextab.table[i].lexema == LEX_FUNCTION
								|| tables.lextab.table[i - 1].lexema == LEX_TYPE ||   tables.lextab.table[i].lexema == LEX_TYPE
								|| tables.lextab.table[i - 1].lexema == LEX_PROC || tables.lextab.table[i].lexema == LEX_PROC)
							{
								Log::WriteError(log.stream, Error::geterrorin(305, curline, 0));
								lex_good = false;
							}
							idxTI = IT::IsId(tables.idtab, id);	// индекс существующего идентификатора
							if (lexema == LEX_LITERAL)
								idxTI = getLiteralIndex(tables.idtab, curword, getType(id, in.words[i - 1].word), log, curline); // или литерала
						}
					}
					break;
					}
						LT::Entry* ltentry = new LT::Entry(lexema, curline, idxTI);
						LT::Add(tables.lextab, *ltentry);
					break;
				}
				else if (j == N_GRAPHS - 1) // цепочка не распознана
				{
					Log::WriteError(log.stream, Error::geterrorin(201, curline, 0));
					lex_good = false;
				}
			}
		}

		if (enterPoint == NULL) // не найдена точка входа
		{
			Log::WriteError(log.stream, Error::geterror(301));
			lex_good = false;
		}
		if (enterPoint > 1) //больше 1 точки входа
		{
			Log::WriteError(log.stream, Error::geterrorin(302, curline, 0));
			lex_good = false;
		}
		return lex_good;
	}
};
