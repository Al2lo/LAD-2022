#include "stdafx.h"

namespace Log
{
	LOG getlog(wchar_t logfile[]) // ������ � ��������� ���
	{
		LOG log;
		log.stream = new std::ofstream;
		log.stream->open(logfile);
		if (!log.stream->is_open())
		{
			throw ERROR_THROW(112);
		}
		wcscpy_s(log.logfile, logfile);
		return log;

	}// ������ � ��������� LOG

	void writeLine(std::ostream* stream, char* c, ...)		// ������� � �������� ������������ �����
	{
		char** ptr = &c;			// ��������� ��� ������� � ����������
		char* result;				// ������ ����������
		result = new char[15];
		int size = 0;

		while (strcmp(*ptr, "") != 0)
		{
			size_t slen = strlen(*ptr);
			result = (char*)realloc(result, size + slen);
			result[size] = '\0';
			size += slen; // size - ������ ������ ������
			strcat_s(result, size + 1, *ptr);
			ptr++;
		}
		*stream << result << std::endl;
	}
	void WriteLine(LOG log, wchar_t* c, ...)
	{
		wchar_t** pc = &c;
		char l[1024] = "";
		char f[1024] = "";
		while (*pc != L"")
		{
			wcstombs_s(0, l, *pc, IN_MAX_LEN_TEXT);
			strcat_s(f, l);
			pc++;
		}
		*log.stream << f;
	}
	void WriteLog(LOG log)
	{
		char date[100];
		tm local;
		time_t currentTime;
		currentTime = time(NULL);
		localtime_s(&local, &currentTime);
		strftime(date, 100, "%d.%m.%Y %H:%M:%S", &local);
		*log.stream << "�----- �������� �- ����: " << date << "-----------------------------------------" << std::endl;
	}
	void WriteParm(LOG& log, Parm::PARM& parm)
	{
		char inTxt[PARM_MAX_SIZE],
			outTxt[PARM_MAX_SIZE],
			logTxt[PARM_MAX_SIZE];
		wcstombs(inTxt, parm.in, wcslen(parm.in) + 1);
		wcstombs(outTxt, parm.out, wcslen(parm.out) + 1);
		wcstombs(logTxt, parm.log, wcslen(parm.log) + 1);
		*log.stream << "\n----- ��������� --------";
		*log.stream << "\n-in: " << inTxt
			<< "\n-out: " << outTxt
			<< "\n-log: " << logTxt;
		//<< "\n�������������� �����:\n" <<
		//(parm.f_LEX ? "-l  " : "") << (parm.f_ID ? "-i  " : "")
		//<< (parm.f_RULES ? "-r  " : "") << (parm.f_COUT ? "-c  " : "") << "\n\n";
	}
	void WriteIn(LOG log, In::IN in)
	{
		*log.stream << "-�---- �������� ������----------- " << std::endl;
		*log.stream << in.text << std::endl;
		*log.stream << "���������� ��������: " << in.size << std::endl;
		*log.stream << "���������������: " << in.ignor << std::endl;
		*log.stream << "���������� �����: " << in.lines << std::endl;
	}
	void WriteError(std::ostream* stream, Error::ERROR e)
	{
		if (stream == NULL)
		{
			if (e.inext.col == -1 || e.inext.line == -1)
				std::cout << std::endl << "������ N" << e.id << ": " << e.message << std::endl;
			else if (e.inext.col == NULL)
				std::cout << std::endl << "������ N" << e.id << ": " << e.message
				<< " ������: " << e.inext.line << std::endl;
			else
				std::cout << std::endl << "������ N" << e.id << ": " << e.message
				<< " ������: " << e.inext.line
				<< " ������� � ������: " << e.inext.col << std::endl;
			system("pause");
		}
		else
		{
			if (e.inext.col == -1 || e.inext.line == -1)
				*stream << std::endl << "������ N" << e.id << ": " << e.message;
			else if (e.inext.col == NULL)
				*stream << std::endl << "������ N" << e.id << ": " << e.message
				<< " ������: " << e.inext.line;
			else
				*stream << std::endl << "������ N" << e.id << ": " << e.message
				<< " ������: " << e.inext.line
				<< " ������� � ������: " << e.inext.col;
			*stream << std::endl;
		}
		throw ERROR_THROW_IN(e.id, e.inext.line, e.inext.col);
	}

	void writeWords(std::ostream* stream, In::inWords* words)
	{
		*stream << " ------------------ ������� ����: ------------------" << std::endl;
		for (int i = 0; i < words->size; i++)
			*stream << std::setw(2) << i << std::setw(3) << words[i].line << " |  " << words[i].word << std::endl;
		*stream << "\n-------------------------------------------------------------------------\n\n";
	}
	void Close(LOG log)
	{
		log.stream->close();
	}
};