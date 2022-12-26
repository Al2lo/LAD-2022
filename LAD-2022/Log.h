#pragma once
#include "In.h"
#include "Parm.h"
#include "Error.h"

namespace Log	// –абота с протоколом
{
	struct LOG // протокол
	{
		wchar_t logfile[PARM_MAX_SIZE]; // им€ файла протокола
		std::ofstream* stream;	// выходной поток протокола
	};

	static const LOG INITLOG{ L"", NULL };// структура дл€ начальной инициализации LOG
	LOG getlog(wchar_t logfile[]);// сформировать структуру LOG
	void writeLine(std::ostream* stream, char* c, ...);			//вывести  строку в поток
	void WriteLine(LOG log, wchar_t* с, ...); //вывести в протокол конкатенацию строк
	void WriteLog(LOG log);// вывести в протокол заголовок
	void WriteParm(LOG& log, Parm::PARM& parm); // вывести в протокол информацию о входных параметрах
	void WriteIn(LOG log, In::IN in);// вывести в протокол информацию о входном потоке
	void WriteError(std::ostream* stream, Error::ERROR error); // вывести в протокол информацию об ошибке
	void Close(LOG log);
};