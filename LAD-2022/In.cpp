#include "stdafx.h"
#define	LEX_MINUS		 '-'	// вычитание
#define	MAXSIZE_LT		 4096	    //максимальное количество строк в ТЛ

int In::inWords::size = NULL;


namespace In
{
	IN getin(wchar_t infile[], std::ostream* stream)
	{
		unsigned char* text = new unsigned char[IN_MAX_LEN_TEXT];
		std::ifstream instream(infile);
		if (!instream.is_open())
			throw ERROR_THROW(102);								//"Системная ошибка: Ошибка при открытии файла с исходным кодом(-in)"
		IN in;
		int pos = 0;
		bool isLiteral = false;
		while (true)
		{
			unsigned char uch = instream.get();
			if (instream.eof())
				break;
			if (in.code[uch] == IN::Q)   // ковычка 
				isLiteral = !isLiteral;
			switch (in.code[uch])
			{
			case IN::N: // новая строка 
				text[in.size++] = uch;
				in.lines++;
				pos = -1;
				break;
			case IN::T: // разрешённый символ 
			case IN::P: // пробел либо табуляция
			case IN::S: // символы 
			case IN::Q: // кавычки
				text[in.size++] = uch;
				break;
			case IN::F: // недопустимый символ в исходном файле
				Log::WriteError(stream, Error::geterrorin(200, in.lines, pos));
				in.ignor++;
				break;
			case IN::I:// игнорируемый символ 
				in.ignor++;
				break;
			default:
				text[in.size++] = in.code[uch];
			}
			pos++;
		}
		text[in.size] = '\0';
		in.text = text;
		instream.close(); // закрыть поток 
		return in;
	}

	void addWord(inWords* words, char* word, int line)
	{
		if (*word == '\0')
		{
			return;
		}
		words[inWords::size].line = line;
		strcpy_s(words[inWords::size].word, strlen(word) + 1, word);
		inWords::size++;
	}

	inWords* getWordsTable(std::ostream* stream, unsigned char* text, int* code, int textSize)
	{
		inWords* words = new inWords[MAXSIZE_LT];
		int bufpos = 0;				// позиция в буфере
		int line = 1;				// номер строки исходного кода
		bool words_good = false;
		bool mainBool = false;
		int name = 0;
		int countRightBrace = 0;
		char buffer[MAX_LEN_BUFFER] = "";		// буфер
		for (int i = 0; i < textSize; i++)			// заполнение
		{
			switch (code[text[i]])
			{
			case IN::S:
			{
				if (text[i] == LEX_LEFTBRACE)
				{
					name++;
				}
				if (text[i] != LEX_RIGHTHESIS && text[i] != LEX_LEFTHESIS && mainBool)
				{
					if (name == 0)
					{
						Log::WriteError(stream, Error::geterrorin(201, line, 0));
						break;
					}
				}
				if (text[i] == LEX_RIGHTBRACE)
				{
					countRightBrace++;
					words_good = true;
					name--;
				}
				if (text[i] == LEX_MINUS && isdigit(text[i + 1])) // числовой литерал с минусом
				{
					buffer[bufpos++] = text[i];
					buffer[bufpos] = IN_CODE_NULL;
					break;
				}
				char letter[] = { (char)text[i], IN_CODE_NULL };
				addWord(words, buffer, line);	// буфер перед односимвольной лексемой
				addWord(words, letter, line);	// сама односимвольная лексема
				*buffer = IN_CODE_NULL;
				bufpos = 0;
				break;
			}
			case IN::N:											//новая строка
			case IN::P:											// разделители и пробелы
				addWord(words, buffer, line);
				*buffer = IN_CODE_NULL;
				bufpos = 0;
				if (code[text[i]] == IN::N)
					line++;
				break;
			case IN::Q:		// строковый литерал
			{
				addWord(words, buffer, line);
				*buffer = IN_CODE_NULL;
				bufpos = 0;
				bool isltrlgood = true;
				bool isChar = false;
				// если литерал не закрыт - перевод строки будет раньше кавычки
				for (int j = i + 1; text[j] != IN_CODE_QUOTE; j++)
				{
					if (text[j] == IN_CODE_QUOTE2) {
						isChar = true;
						break;
					}
					if (code[text[j]] == IN::N)
					{
						Log::WriteError(stream, Error::geterrorin(311, line, 0));
						isltrlgood = false; // литерал не закрыт!
						break;
					}
				}
				if (isltrlgood && !isChar)
				{
					buffer[bufpos++] = IN_CODE_QUOTE;
					for (int j = 1; ; j++)
					{
						if (j == 256 || i + j == textSize)
						{
							Log::WriteError(stream, Error::geterrorin(312, line, 0));
							break; // превышен размер литерала(учтена откр кавычка)
						}
						buffer[bufpos++] = text[i + j];
						if (text[i + j] == IN_CODE_QUOTE) // закрывающая кавычка
						{
							buffer[bufpos] = IN_CODE_NULL;
							addWord(words, buffer, line);
							i = i + j;
							break;
						}
					}
				} // literal good
				else if (isChar) {
					buffer[bufpos++] = IN_CODE_QUOTE2;
					for (int j = 1; ; j++)
					{
						if (j == 256 || i + j == textSize)
						{
							Log::WriteError(stream, Error::geterrorin(312, line, 0));
							break; // превышен размер литерала(учтена откр кавычка)
						}
						buffer[bufpos++] = text[i + j];
						if (text[i + j] == IN_CODE_QUOTE2) // закрывающая кавычка
						{
							buffer[bufpos] = IN_CODE_NULL;
							addWord(words, buffer, line);
							i = i + j;
							break;
						}
					}
				}
				*buffer = IN_CODE_NULL;  bufpos = 0;
				break;
			}
			default:
				buffer[bufpos++] = text[i];
				buffer[bufpos] = IN_CODE_NULL;
				if (strcmp(buffer, "main") == 0)
					mainBool = true;
				if (mainBool)
					words_good = false;
				
			}
		}
		if (!words_good)
			Log::WriteError(stream, Error::geterrorin(321, line, 0));
		return words;
	}


	void printTable(inWords* table)
	{
		cout << " ------------------ ТАБЛИЦА СЛОВ: ------------------" << std::endl;
		for (int i = 0; i < table->size; i++)
		{
			std::cout << std::setw(2) << i << std::setw(3) << table[i].line << " |  " << table[i].word << std::endl;
		}
	}



};

