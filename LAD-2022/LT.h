#pragma once
#define	LEXEMA_FIXSIZE			 1	    
#define	LT_MAXSIZE				 4096	
#define	LT_TI_NULLIDX			 0xffffffff	
#define LEX_SEPARATORS			's'
#define LEX_TYPE				't'
#define LEX_LITERAL				'l'
#define LEX_ID					'i'
#define LEX_FUNCTION			'f'
#define LEX_MAINFUNC			'm'
#define LEX_RIGHT				']'
#define LEX_LEFT				'['
#define LEX_RETURN				'r'
#define LEX_PRINT				'p'
#define LEX_SEMICOLON			';'
#define LEX_COMMA				','
#define LEX_LEFTBRACE			'{'
#define LEX_RIGHTBRACE			'}'
#define LEX_LEFTHESIS			'('
#define LEX_RIGHTHESIS			')'
#define LEX_PLUS				'+'
#define LEX_MINUS				'-'
#define LEX_MULTIPLICATION		'*'
#define LEX_DIRSLASH			'/'
#define LEX_PERSENT				'%'
#define LEX_ASSIGN				'='
#define LEX_DECLARE				'n'
#define LEX_STATE				'~'
#define	LEX_LITERAL_16			'h'
#define LEX_ISTRUE				'w'
#define LEX_ISFALSE				'e'
#define LEX_NEWLINE				'^'
#define LEX_MORE				'>'
#define LEX_MORE_AND_EQAL		'#'
#define LEX_LESS_AND_EQAL		'@'
#define LEX_LESS				'<'
#define LEX_EQUAL				'&'
#define LEX_DONT_EQUAL			'!'
#define LEX_CYCLE				'c'
#define LEX_PROC				'u'
#define LEX_DIEZ				'?'




namespace LT
{
	struct  Entry
	{
		char lexema;
		int sn;
		int idxTI;

		Entry();
		Entry(char lexema, int snn, int idxti = LT_TI_NULLIDX);
	};

	struct LexTable
	{
		int maxsize;
		int size;
		Entry* table;
	};

	LexTable Create(int size);
	void Add(LexTable& lextable, Entry entry);
	void AddStruct(LexTable& lextable, Entry entry);	//экземпляр ТЛ, строка ТЛ
	Entry GetEntry(LexTable& lextable, int n);
	void Delete(LexTable& lextable);
	void writeLexTable(std::ostream* stream, LT::LexTable& lextable);		//вывести таблицу лексем
	void writeLexemsOnLines(std::ostream* stream, LT::LexTable& lextable);	//вывести лексемы по строкам

};