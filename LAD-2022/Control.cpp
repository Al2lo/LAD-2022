#include "stdafx.h"

int _tmain(int argc, wchar_t* argv[])
{
	ofstream outFile("..\\Asm\\out.asm");

	char LEXERROR[] = "Лексический анализ завершен с ошибками";
	char SYNTERROR[] = "Синтаксический анализ завершен с ошибками";
	char SEMERROR[] = "Обнаружены семантические ошибки";
	char POLISHERROR[] = "Ошибка при попытке преобразования выражения";
	char LEXGOOD[] = "Лексический анализ завершен без ошибок";
	char SYNTGOOD[] = "Синтаксический анализ завершен без ошибок";
	char SEMGOOD[] = "Семантический анализ завершен без ошибок";
	char POLISHGOOD[] = "Преобразование выражений завершено без ошибок";
	char MESSAGE[] = "--------------------КОНЕЧНЫЕ ТАБЛИЦЫ ЛЕКСЕМ И ИДЕНТИФИКАТОРОВ-------------------";
	char STOP[] = "\nВыполние программы остановлено";
	char ALLGOOD[] = "Программа успешно завершена!";
	setlocale(LC_ALL, "Russian");
	Log::LOG log;

	try
	{
		Parm::PARM parm = Parm::getparm(argc, argv);                            //получить параметры
		log = Log::getlog(parm.log);
		Log::WriteLog(log);														//написать заголовок журнала
		Log::WriteParm(log, parm);												//записать в журнал параметры
		In::IN in = In::getin(parm.in, log.stream);
		Log::WriteIn(log, in);

		in.words = In::getWordsTable(log.stream, in.text, in.code, in.size);	//разобрать на токены
		In::printTable(in.words);
		LexAnalize::LEX tables;
		bool lex_ok = LexAnalize::LexAnalize(tables, in, log, parm);					//выполнить лексический анализ
		cout << lex_ok;
		LT::writeLexTable(&std::cout, tables.lextab);
		LT::writeLexTable(log.stream, tables.lextab);							//записать в журнал таблицы лексем и идентификаторов 
		IT::writeIdTable(log.stream, tables.idtab);							//а также соответствие токенов и лексем
		LT::writeLexemsOnLines(log.stream, tables.lextab);
		if (!lex_ok)
		{
			Log::writeLine(log.stream, SYNTERROR, "");
			Log::writeLine(&std::cout, SYNTERROR, STOP, "");
			return 0;
		}
		else
		{
			Log::writeLine(&std::cout, LEXGOOD, "");
		}
		MFST_TRACE_START(log.stream);
		MFST::Mfst mfst(tables, GRB::getGreibach());							//выполнить синтаксический анализ
		bool synt_ok = mfst.start(log);
		mfst.savededucation();
		mfst.printrules(log);													//вывести дерево разбора
		if (!synt_ok)
		{
			Log::writeLine(log.stream, SYNTERROR, "");
			Log::writeLine(&std::cout, SYNTERROR, STOP, "");
			return 0;
		}
		else Log::writeLine(&std::cout, SYNTGOOD, "");
		bool sem_good = SemAnalize::semanticAnalize(tables, log);			//выполнить семантический анализ
		if (!sem_good)
		{
			Log::writeLine(log.stream, SEMERROR, "");
			Log::writeLine(&std::cout, SEMERROR, STOP, "");
			return 0;
		}
		else Log::writeLine(&std::cout, SEMGOOD, "");
		bool polish_good = PolishNot::PolishNotation(tables, log);					//выполнить преобразование выражений в ПОЛИЗ
		if (!polish_good)
		{
			Log::writeLine(log.stream, POLISHERROR, "");
			Log::writeLine(&std::cout, POLISHERROR, STOP, "");
			return 0;
		}
		Log::writeLine(&std::cout, POLISHGOOD, "");
		Log::writeLine(log.stream, MESSAGE, "");
		LT::writeLexTable(log.stream, tables.lextab);							//записать в журнал новые таблицы лексем и идентификаторов
		IT::writeIdTable(log.stream, tables.idtab);
		LT::writeLexemsOnLines(log.stream, tables.lextab);					//а также соответствие токенов и лексем
		//Log::writeLine(&std::cout, MESSAGE, "");
		//IT::writeIdTable(&std::cout, tables.idtable);							//записать в командную строку новые таблицы лексем и идентификаторов 
		//LT::writeLexTable(&std::cout, tables.lextable);							//а также соответствие токенов и лексем
		//LT::writeLexemsOnLines(&std::cout, tables.lextable);

		Gener::codeGeneration(&outFile,tables);							//выполнить генерацию кода
		Log::writeLine(log.stream, ALLGOOD, "");									//итог работы программы
		Log::writeLine(&std::cout, ALLGOOD, "");
		Log::Close(log);													    //закрыть журнал
	}
	catch (Error::ERROR e)
	{
		std::cout << "Ошибка " << e.id << ": " << e.message << ", строка "<<e.inext.line << std::endl;
		system("pause");
	}
	system("pause");
	return 0;
}