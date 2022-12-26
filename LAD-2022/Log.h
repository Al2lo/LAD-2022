#pragma once
#include "In.h"
#include "Parm.h"
#include "Error.h"

namespace Log	// ������ � ����������
{
	struct LOG // ��������
	{
		wchar_t logfile[PARM_MAX_SIZE]; // ��� ����� ���������
		std::ofstream* stream;	// �������� ����� ���������
	};

	static const LOG INITLOG{ L"", NULL };// ��������� ��� ��������� ������������� LOG
	LOG getlog(wchar_t logfile[]);// ������������ ��������� LOG
	void writeLine(std::ostream* stream, char* c, ...);			//�������  ������ � �����
	void WriteLine(LOG log, wchar_t* �, ...); //������� � �������� ������������ �����
	void WriteLog(LOG log);// ������� � �������� ���������
	void WriteParm(LOG& log, Parm::PARM& parm); // ������� � �������� ���������� � ������� ����������
	void WriteIn(LOG log, In::IN in);// ������� � �������� ���������� � ������� ������
	void WriteError(std::ostream* stream, Error::ERROR error); // ������� � �������� ���������� �� ������
	void Close(LOG log);
};