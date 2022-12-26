#include "stdafx.h"

namespace SemAnalize
{
	bool semanticAnalize(LexAnalize::LEX& table, Log::LOG& log)
	{
		bool sem_good = true;

		for (int i = 0; i < table.lextab.size; i++)
		{
			switch (table.lextab.table[i].lexema)
			{
			case LEX_DECLARE:
			{
				if (table.lextab.table[i + 1].lexema != LEX_TYPE)
				{
					sem_good = false;
					Log::WriteError(log.stream, Error::geterrorin(303, table.lextab.table[i].sn, 0));
				}
			}
			case LEX_DIRSLASH:
			{
				int k = i;
				if (table.lextab.table[i + 1].lexema == LEX_ID)
				{
					for (k; k > 0; k--)
					{
						if (table.lextab.table[k].lexema == LEX_ID)
						{
							if (table.idtab.table[table.lextab.table[k].idxTI].id == table.idtab.table[table.lextab.table[i + 1].idxTI].id)
							{
								if (table.lextab.table[k + 2].lexema == LEX_LITERAL && table.idtab.table[table.lextab.table[k + 2].idxTI].value.vint == 0)
								{
									sem_good = false;
									Log::WriteError(log.stream, Error::geterrorin(319, table.lextab.table[k].sn, 0));
								}
							}
						}
					}
				}
				if (table.lextab.table[i + 1].lexema == LEX_LITERAL)
				{
					if (table.idtab.table[table.lextab.table[i + 1].idxTI].value.vint == 0)
					{
						sem_good = false;
						Log::WriteError(log.stream, Error::geterrorin(319, table.lextab.table[k].sn, 0));
					}
					if (table.idtab.table[table.lextab.table[i + 1].idxTI].iddatatype == IT::CHAR || table.idtab.table[table.lextab.table[i + 1].idxTI].iddatatype == IT::STR)
					{
						sem_good = false;
						Log::WriteError(log.stream, Error::geterrorin(322, table.lextab.table[k].sn, 0));
					}
				}
				break;
			}
			case LEX_ASSIGN: //выражение
			{
				if (i > 0 && table.lextab.table[i - 1].idxTI != TI_NULLIDX) // левый операнд
				{
					IT::IDDATATYPE lefttype = table.idtab.table[table.lextab.table[i - 1].idxTI].iddatatype;
					bool ignore = false;

					for (int k = i + 1; table.lextab.table[k].lexema != LEX_SEMICOLON; k++)
					{
						if (k == table.lextab.size)
							break; // синтакс ошибка - нет ;
						if (table.lextab.table[k].idxTI != TI_NULLIDX) // если ид - проверить совпадение типов
						{
							if (!ignore)
							{
								IT::IDDATATYPE righttype = table.idtab.table[table.lextab.table[k].idxTI].iddatatype;
								if (lefttype != righttype) // типы данных в выражении не совпадают
								{
									if (lefttype == IT::IDDATATYPE::UINT && righttype == IT::IDDATATYPE::INT && table.lextab.table[k].lexema == LEX_LITERAL)
									{
										if (lefttype == IT::IDDATATYPE::UINT && table.idtab.table[table.lextab.table[i + 1].idxTI].value.vint < 0 )
										{
											Log::WriteError(log.stream, Error::geterrorin(320, table.lextab.table[k].sn, 0));
											sem_good = false;
											break;
										}
										else
											break;
									}
									else {
										Log::WriteError(log.stream, Error::geterrorin(314, table.lextab.table[k].sn, 0));
										sem_good = false;
										break;
									}
								}
								else if (lefttype == IT::INT && table.idtab.table[table.lextab.table[i + 1].idxTI].value.vint > 32767)
								{
									Log::WriteError(log.stream, Error::geterrorin(320, table.lextab.table[k].sn, 0));
									sem_good = false;
									break;

								}
							}
							// если лексема сразу после идентиф скобка - это вызов функции
							if (table.lextab.table[k + 1].lexema == LEX_LEFTHESIS)
							{
								ignore = true;
								continue;
							}
							// закрывающая скобка после списка параметров
							if (ignore && table.lextab.table[k + 1].lexema == LEX_RIGHTHESIS)
							{
								ignore = false;
								continue;
							}
						}
						if (lefttype == IT::IDDATATYPE::STR) // справа только литерал, ид или вызов строковой ф-ции
						{
							if (table.lextab.table[k].lexema == LEX_PLUS || table.lextab.table[k].lexema == LEX_MINUS || table.lextab.table[k].lexema == LEX_MULTIPLICATION || table.lextab.table[k].lexema == LEX_DIRSLASH) // выражения недопустимы
							{
								Log::WriteError(log.stream, Error::geterrorin(316, table.lextab.table[k].sn, 0));
								sem_good = false;
								break;
							}
						}
						if (lefttype == IT::IDDATATYPE::CHAR) // справа только литерал, ид или вызов строковой ф-ции
						{
							if (table.lextab.table[k].lexema == LEX_PLUS || table.lextab.table[k].lexema == LEX_MINUS || table.lextab.table[k].lexema == LEX_MULTIPLICATION || table.lextab.table[k].lexema == LEX_DIRSLASH) // выражения недопустимы
							{
								Log::WriteError(log.stream, Error::geterrorin(317, table.lextab.table[k].sn, 0));
								sem_good = false;
								break;
							}
						}
					}
				}
				break;
			}
			case LEX_ID: // проверка типа возвращаемого значения  
			{
				IT::Entry e = table.idtab.table[table.lextab.table[i].idxTI];

				if (i > 0 && table.lextab.table[i - 1].lexema == LEX_FUNCTION)
				{
					if (e.idtype == IT::IDTYPE::F) //функция, не процедура
					{
						for (int k = i + 1; ; k++)
						{
							char l = table.lextab.table[k].lexema;
							if (l == LEX_RETURN)
							{
								int next = table.lextab.table[k + 1].idxTI; // след. за return
								if (next != TI_NULLIDX)
								{
									// тип функции и возвращаемого значения не совпадают
									if (table.idtab.table[next].iddatatype != e.iddatatype || (table.idtab.table[next].iddatatype == IT::INT && table.idtab.table[table.lextab.table[k+1].idxTI].value.vint > 32767))
									{
										if (table.idtab.table[next].iddatatype == IT::INT && e.iddatatype == IT::UINT && table.idtab.table[table.lextab.table[k+1].idxTI].value.vint >= 0)
										{
											break;
										}
										Log::WriteError(log.stream, Error::geterrorin(315, table.lextab.table[k].sn, 0));
										sem_good= false;
										break;
									}
								}
								break; // нашли exit
							}

							if (k == table.lextab.size) break;
						}
					}
				}
				if (table.lextab.table[i + 1].lexema == LEX_LEFTHESIS && table.lextab.table[i - 1].lexema != LEX_FUNCTION) // именно вызов
				{
					if (e.idtype == IT::IDTYPE::F || e.idtype == IT::IDTYPE::S) // точно функция
					{
						int paramscount = NULL;
						// проверка передаваемых параметров
						for (int j = i + 1; table.lextab.table[j].lexema != LEX_RIGHTHESIS; j++)
						{
							// проверка соответствия передаваемых параметров прототипам
							if (table.lextab.table[j].lexema == LEX_ID || table.lextab.table[j].lexema == LEX_LITERAL)
							{
								paramscount++;
								if (e.value.params.count == NULL)
									break;
								IT::IDDATATYPE ctype = table.idtab.table[table.lextab.table[j].idxTI].iddatatype;
								int value = table.idtab.table[table.lextab.table[j].idxTI].value.vint;
								if (ctype != e.value.params.types[paramscount - 1] || (ctype == IT::INT && value > 32767))
								{
									if (table.lextab.table[j].lexema == LEX_LITERAL && ctype == IT::INT && e.value.params.types[paramscount - 1] ==IT::UINT && value >=0)
									{
										continue;
									}
									// Несовпадение типов передаваемых параметров
									Log::WriteError(log.stream, Error::geterrorin(309, table.lextab.table[i].sn, 0));
									sem_good= false;
									break;
								}
							}
							if (j == table.lextab.size)
								break;
						}
						if (paramscount != e.value.params.count)
						{
							// Количество передаваемых и принимаемых параметров не совпадает
							Log::WriteError(log.stream, Error::geterrorin(308, table.lextab.table[i].sn, 0));
							sem_good= false;
						}
						if (paramscount > 3)
						{
							// Слишком много параметров в вызове
							Log::WriteError(log.stream, Error::geterrorin(307, table.lextab.table[i].sn, 0));
							sem_good= false;
						}
					}
				}
				break;
			}
			case LEX_MORE:
			case LEX_LESS:
			case LEX_EQUAL:
			case LEX_DONT_EQUAL:
			case LEX_MORE_AND_EQAL:
			case LEX_LESS_AND_EQAL:
			{
				// левый и правый операнд - числовой тип
				bool flag = true;
				if (i > 1 && table.lextab.table[i - 1].idxTI != TI_NULLIDX)
				{
					if ((table.idtab.table[table.lextab.table[i - 1].idxTI].iddatatype != IT::IDDATATYPE::INT) && (table.idtab.table[table.lextab.table[i - 1].idxTI].iddatatype != IT::IDDATATYPE::UINT) && (table.idtab.table[table.lextab.table[i - 1].idxTI].iddatatype != IT::IDDATATYPE::STR) && (table.idtab.table[table.lextab.table[i - 1].idxTI].iddatatype != IT::IDDATATYPE::CHAR))
						flag = false;
				}
				if (table.lextab.table[i + 1].idxTI != TI_NULLIDX)
				{
					if ((table.idtab.table[table.lextab.table[i + 1].idxTI].iddatatype != IT::IDDATATYPE::INT) && (table.idtab.table[table.lextab.table[i + 1].idxTI].iddatatype != IT::IDDATATYPE::UINT) && (table.idtab.table[table.lextab.table[i + 1].idxTI].iddatatype != IT::IDDATATYPE::STR) && (table.idtab.table[table.lextab.table[i + 1].idxTI].iddatatype != IT::IDDATATYPE::CHAR))
						flag = false;
				}
				if ((table.idtab.table[table.lextab.table[i - 1].idxTI].iddatatype == IT::IDDATATYPE::STR && table.idtab.table[table.lextab.table[i + 1].idxTI].iddatatype == IT::IDDATATYPE::CHAR) ||(table.idtab.table[table.lextab.table[i + 1].idxTI].iddatatype == IT::IDDATATYPE::STR && table.idtab.table[table.lextab.table[i - 1].idxTI].iddatatype == IT::IDDATATYPE::CHAR) || (table.idtab.table[table.lextab.table[i + 1].idxTI].iddatatype == IT::IDDATATYPE::STR && table.idtab.table[table.lextab.table[i - 1].idxTI].iddatatype == IT::INT))
				{
					flag = false;
				}
				if (table.idtab.table[table.lextab.table[i + 1].idxTI].iddatatype != table.idtab.table[table.lextab.table[i - 1].idxTI].iddatatype  &&  (table.lextab.table[i - 1].lexema == LEX_ID && table.lextab.table[i + 1].lexema == LEX_ID))
				{
					flag = false;
				}
				if (!flag)
				{
					// строка или неизвестный ид в условии
					Log::WriteError(log.stream, Error::geterrorin(318, table.lextab.table[i].sn, 0));
					sem_good= false;
				}
				break;
			}
			case LEX_MINUS:
			case LEX_PLUS:
			case LEX_MULTIPLICATION:
			case LEX_PERSENT:
			{
				// левый и правый операнд - числовой тип
				bool flag = true;
				if (i > 1 && table.lextab.table[i - 1].idxTI != TI_NULLIDX)
				{
					if ((table.idtab.table[table.lextab.table[i - 1].idxTI].iddatatype != IT::IDDATATYPE::INT) && (table.idtab.table[table.lextab.table[i - 1].idxTI].iddatatype != IT::IDDATATYPE::UINT))
						flag = false;
				}
				if (table.lextab.table[i + 1].idxTI != TI_NULLIDX)
				{
					if ((table.idtab.table[table.lextab.table[i + 1].idxTI].iddatatype != IT::IDDATATYPE::INT) && (table.idtab.table[table.lextab.table[i + 1].idxTI].iddatatype != IT::IDDATATYPE::UINT))
						flag = false;
				}
				if (table.idtab.table[table.lextab.table[i + 1].idxTI].iddatatype != table.idtab.table[table.lextab.table[i - 1].idxTI].iddatatype && (table.lextab.table[i - 1].lexema == LEX_ID && table.lextab.table[i + 1].lexema == LEX_ID))
				{
					flag = false;
				}
				if (!flag)
				{
					// строка или неизвестный ид в условии
					Log::WriteError(log.stream, Error::geterrorin(322, table.lextab.table[i].sn, 0));
					sem_good = false;
				}
				break;
			}
			}
		}

		return sem_good;
	}
};