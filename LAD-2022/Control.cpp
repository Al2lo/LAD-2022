#include "stdafx.h"

int _tmain(int argc, wchar_t* argv[])
{
	ofstream outFile("..\\Asm\\out.asm");

	char LEXERROR[] = "����������� ������ �������� � ��������";
	char SYNTERROR[] = "�������������� ������ �������� � ��������";
	char SEMERROR[] = "���������� ������������� ������";
	char POLISHERROR[] = "������ ��� ������� �������������� ���������";
	char LEXGOOD[] = "����������� ������ �������� ��� ������";
	char SYNTGOOD[] = "�������������� ������ �������� ��� ������";
	char SEMGOOD[] = "������������� ������ �������� ��� ������";
	char POLISHGOOD[] = "�������������� ��������� ��������� ��� ������";
	char MESSAGE[] = "--------------------�������� ������� ������ � ���������������-------------------";
	char STOP[] = "\n�������� ��������� �����������";
	char ALLGOOD[] = "��������� ������� ���������!";
	setlocale(LC_ALL, "Russian");
	Log::LOG log;

	try
	{
		Parm::PARM parm = Parm::getparm(argc, argv);                            //�������� ���������
		log = Log::getlog(parm.log);
		Log::WriteLog(log);														//�������� ��������� �������
		Log::WriteParm(log, parm);												//�������� � ������ ���������
		In::IN in = In::getin(parm.in, log.stream);
		Log::WriteIn(log, in);

		in.words = In::getWordsTable(log.stream, in.text, in.code, in.size);	//��������� �� ������
		In::printTable(in.words);
		LexAnalize::LEX tables;
		bool lex_ok = LexAnalize::LexAnalize(tables, in, log, parm);					//��������� ����������� ������
		cout << lex_ok;
		LT::writeLexTable(&std::cout, tables.lextab);
		LT::writeLexTable(log.stream, tables.lextab);							//�������� � ������ ������� ������ � ��������������� 
		IT::writeIdTable(log.stream, tables.idtab);							//� ����� ������������ ������� � ������
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
		MFST::Mfst mfst(tables, GRB::getGreibach());							//��������� �������������� ������
		bool synt_ok = mfst.start(log);
		mfst.savededucation();
		mfst.printrules(log);													//������� ������ �������
		if (!synt_ok)
		{
			Log::writeLine(log.stream, SYNTERROR, "");
			Log::writeLine(&std::cout, SYNTERROR, STOP, "");
			return 0;
		}
		else Log::writeLine(&std::cout, SYNTGOOD, "");
		bool sem_good = SemAnalize::semanticAnalize(tables, log);			//��������� ������������� ������
		if (!sem_good)
		{
			Log::writeLine(log.stream, SEMERROR, "");
			Log::writeLine(&std::cout, SEMERROR, STOP, "");
			return 0;
		}
		else Log::writeLine(&std::cout, SEMGOOD, "");
		bool polish_good = PolishNot::PolishNotation(tables, log);					//��������� �������������� ��������� � �����
		if (!polish_good)
		{
			Log::writeLine(log.stream, POLISHERROR, "");
			Log::writeLine(&std::cout, POLISHERROR, STOP, "");
			return 0;
		}
		Log::writeLine(&std::cout, POLISHGOOD, "");
		Log::writeLine(log.stream, MESSAGE, "");
		LT::writeLexTable(log.stream, tables.lextab);							//�������� � ������ ����� ������� ������ � ���������������
		IT::writeIdTable(log.stream, tables.idtab);
		LT::writeLexemsOnLines(log.stream, tables.lextab);					//� ����� ������������ ������� � ������
		//Log::writeLine(&std::cout, MESSAGE, "");
		//IT::writeIdTable(&std::cout, tables.idtable);							//�������� � ��������� ������ ����� ������� ������ � ��������������� 
		//LT::writeLexTable(&std::cout, tables.lextable);							//� ����� ������������ ������� � ������
		//LT::writeLexemsOnLines(&std::cout, tables.lextable);

		Gener::codeGeneration(&outFile,tables);							//��������� ��������� ����
		Log::writeLine(log.stream, ALLGOOD, "");									//���� ������ ���������
		Log::writeLine(&std::cout, ALLGOOD, "");
		Log::Close(log);													    //������� ������
	}
	catch (Error::ERROR e)
	{
		std::cout << "������ " << e.id << ": " << e.message << ", ������ "<<e.inext.line << std::endl;
		system("pause");
	}
	system("pause");
	return 0;
}