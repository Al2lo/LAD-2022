#pragma once
#define MAX_LEN_BUFFER  2048
#define IN_MAX_LEN_TEXT 1024*1024 //максимальный размер файла исходного кода = 1МВ
#define IN_CODE_ENDL '\n'//символ конца строки
#define IN_CODE_NULL  '\0'
#define IN_CODE_QUOTE '\"'
#define IN_CODE_QUOTE2 '\''


#define IN_CODE_TABLE	{\
		IN::F, IN::F, IN::F, IN::F,  IN::F, IN::F, IN::F, IN::F,   IN::F, IN::P, IN::N, IN::F,  IN::F, IN::F, IN::F, IN::F,\
		IN::F, IN::F, IN::F, IN::F,  IN::F, IN::F, IN::F, IN::F,   IN::F, IN::F, IN::F, IN::F,  IN::F, IN::F, IN::F, IN::F,\
		IN::P, IN::S, IN::Q, IN::S,  IN::T, IN::T, IN::S, IN::Q,   IN::S, IN::S, IN::S, IN::S,  IN::S, IN::S, IN::T, IN::S,\
		IN::T, IN::T, IN::T, IN::T,  IN::T, IN::T, IN::T, IN::T,   IN::T, IN::T, IN::T, IN::S,  IN::S, IN::S, IN::S, IN::S,\
		IN::T, IN::T, IN::T, IN::T,  IN::T, IN::T, IN::T, IN::T,   IN::T, IN::T, IN::T, IN::T,  IN::T, IN::T, IN::T, IN::T,\
		IN::T, IN::T, IN::T, IN::T,  IN::T, IN::T, IN::T, IN::T,   IN::T, IN::T, IN::T, IN::S,  IN::T, IN::S, IN::T, IN::T,\
		IN::F, IN::T, IN::T, IN::T,  IN::T, IN::T, IN::T, IN::T,   IN::T, IN::T, IN::T, IN::T,  IN::T, IN::T, IN::T, IN::T,\
		IN::T, IN::T, IN::T, IN::T,  IN::T, IN::T, IN::T, IN::T,   IN::T, IN::T, IN::T, IN::S,  IN::T, IN::S, IN::S, IN::T,\
																														   \
		IN::F, IN::F, IN::F, IN::F,  IN::F, IN::F, IN::F, IN::F,   IN::F, IN::F, IN::F, IN::F,  IN::F, IN::F, IN::F, IN::F,\
		IN::F, IN::F, IN::F, IN::F,  IN::F, IN::F, IN::F, IN::F,   IN::F, IN::F, IN::F, IN::F,  IN::F, IN::F, IN::F, IN::F,\
		IN::F, IN::F, IN::F, IN::F,  IN::F, IN::F, IN::F, IN::F,   IN::F, IN::F, IN::F, IN::F,  IN::F, IN::F, IN::F, IN::F,\
		IN::F, IN::F, IN::F, IN::F,  IN::F, IN::F, IN::F, IN::F,   IN::F, IN::F, IN::F, IN::F,  IN::F, IN::F, IN::F, IN::F,\
		IN::F, IN::F, IN::F, IN::F,  IN::F, IN::F, IN::F, IN::F,   IN::F, IN::F, IN::F, IN::F,  IN::F, IN::F, IN::F, IN::F,\
		IN::F, IN::F, IN::F, IN::F,  IN::F, IN::F, IN::F, IN::F,   IN::F, IN::F, IN::F, IN::F,  IN::F, IN::F, IN::F, IN::F,\
		IN::F, IN::F, IN::F, IN::F,  IN::F, IN::F, IN::F, IN::F,   IN::F, IN::F, IN::F, IN::F,  IN::F, IN::F, IN::F, IN::F,\
		IN::F, IN::F, IN::F, IN::F,  IN::F, IN::F, IN::F, IN::F,   IN::F, IN::F, IN::F, IN::F,  IN::F, IN::F, IN::T, IN::F \
}
namespace In
{
	struct inWords
	{
		char word[MAX_LEN_BUFFER];//слово
		static int size;//кол-во слов
		int line;//строка в исходном тексте
	};
	struct IN //исходные файлы
	{
		/*
		S - ( ) [ ] ? < > ! & * + - = , # /;
		P - пробел, табуляция+
		N (NEW)- новая строка+
		F (FALSE)- запрещённый
		T (TRUE)- разрешённый
		I (IGNORED)- игнорируемый
		Q - " '+
		*/
		enum //T - допустимый символ, F - недопустимый, I - игнорировать, иначе заменить
		{
			T = 1024,
			F = 2048,
			I = 4096,
			S,
			Q,
			P,
			N
		};
		int size = 0; //размер исходного файла
		int lines = 1; //количесвто строчек
		int ignor = 0; //количество проигнорированных символов
		unsigned char* text; //исходный код (Windows - 1251)
		int code[256] = IN_CODE_TABLE; //таблица проверки: T, F, I - новое значение
		inWords* words;
	};

	IN getin(wchar_t infile[], std::ostream* stream);


	void addWord(inWords* words,char* word,int line);
	inWords* getWordsTable(std::ostream* stream, unsigned char* text, int* code, int textSize);
	void printTable(inWords* worrdsTable);
};
