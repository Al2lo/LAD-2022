#pragma once
#include "stdafx.h"

#define TYPE_INT		"int"
#define TYPE_UINT		"uint"
#define TYPE_STRING		"str"
#define TYPE_CHAR		"symbol"
#define TYPE_PROCEDURE	"proc"
#define POW				"pow"
#define CONCAT			"concat"
#define SQRT			"sqrt"
#define PAUSE			"paused"
#define MAIN			"main"

#define ISTYPE(str) ( !strcmp(str, TYPE_INT) || !strcmp(str, TYPE_UINT) || !strcmp(str, TYPE_STRING)|| !strcmp(str, TYPE_CHAR) )



namespace LexAnalize
{
	struct LEX
	{
		LT::LexTable lextab;
		IT::IdTable idtab;
		LEX() {}
	};

	struct Graph
	{
		char lexema;
		FST::FST graph;
	};

	IT::Entry* getEntry		// формирует и возвращает строку ТИ
	(
		LexAnalize::LEX& tables,						// ТЛ + ТИ
		char lex,								// лексема
		char* id,								// идентификатор
		char* idtype,							// предыдущая (тип)
		bool isParam,							// признак параметра функции
		bool isFunc,							// признак функции
		Log::LOG log,							// протокол
		int line,								// строка в исходном тексте
		bool& rc_err							// флаг ошибки(по ссылке)
	);

	struct ERROR_S									// тип исключения для throw ERROR_THROW | ERROR_THROW_IN
	{
		int id;
		char message[ERROR_MAXSIZE_MESSAGE];			// сообщение			
		struct
		{
			short line = -1;						//номер строки (0, 1, 2, ...)
			short col = -1;						//номер позиции в строке(0, 1, 2, ...)
		} position;
	};


	bool LexAnalize(LEX& tables, In::IN& in, Log::LOG& log, Parm::PARM& parm);
	int	getIndexInLT(LT::LexTable& lextable, int itTableIndex);					// индекс первой встречи в таблице лексем
};

