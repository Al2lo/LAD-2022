#pragma once
#define ID_MAXSIZE 8
#define TI_MAXSIZE 4096
#define SCOPED_ID_MAXSIZE   ID_MAXSIZE*2	//макс число символов идентификатор + область видимости
#define TI_INT_DEFAULT 0x00000000			//значение по умолчанию для num
#define TI_STR_DEFAULT 0x00					//значение по умолчанию для char
#define BOOL_DEFAULT	false				//значение по умолчанию для prime
#define TI_NULLIDX 0xffffffff				//нет элемента таблицы идентификаторов
#define TI_STR_MAXSIZE 255					//максимальная длина строкового литерала
#define INT_MAXSIZE    65535				//максимальное значение для типа int
#define INT_MINSIZE   -32768				//минимальное значение для типа int
#define MAX_PARAMS_COUNT 3					//максимальное количество параметров у функции
#define POW_PARAMS_CNT 2					//кол-во параметров у функции pow
#define PAUSE_PARAMS_CNT 1					//кол-во параметров у функции pause
#define LENGHT_PARAMS_SQRT 1				//кол-во параметров у функции sqrt
#define CONCAT_PARAMS_CNT 2					//кол-во параметров у функции concat
#define CONCAT_TYPE IT::IDDATATYPE::STR
#define POW_TYPE_INT IT::IDDATATYPE::INT
#define PAUSE_TYPE_INT IT::IDDATATYPE::INT
#define SQRT_TYPE_INT IT::IDDATATYPE::INT
#define SQRT_TYPE_UINT IT::IDDATATYPE::UINT
#define POW_TYPE_UINT IT::IDDATATYPE::UINT
#define PAUSE_TYPE_UINT IT::IDDATATYPE::UINT



namespace IT
{
	enum IDDATATYPE {INT = 1,STR = 2,PROC = 3,CHAR =4, UINT = 5,UNDEF = 7};//типы данных идентификаторов
	enum IDTYPE {V = 1, F = 2, P = 3, L = 4, S = 5};//типы идентификаторов: переменная, функция, параметр, литерал, стандартная функция
	enum STD_FUNC {F_POW,F_SQRT, F_CONCAT, F_PAUSE,F_NOT_STD};
	static const IDDATATYPE CONCAT_PARAMS[] = { IT::IDDATATYPE::STR, IT::IDDATATYPE::STR };//параметры функции  concatstr
	static const IDDATATYPE POW_PARAMS_INT[] = { IT::IDDATATYPE::INT, IT::IDDATATYPE::INT };//параметры ф-ции pow
	static const IDDATATYPE PAUSE_PARAMS_INT[] = { IT::IDDATATYPE::INT };//параметры ф-ции pause
	static const IDDATATYPE SQRT_PARAMS_INT[] = { IT::IDDATATYPE::INT };//параметры ф-ции sqrt
	static const IDDATATYPE POW_PARAMS_UINT[] = { IT::IDDATATYPE::UINT, IT::IDDATATYPE::INT };//параметры ф-ции pow
	static const IDDATATYPE PAUSE_PARAMS_UINT[] = { IT::IDDATATYPE::UINT };//параметры ф-ции pause
	static const IDDATATYPE SQRT_PARAMS_UINT[] = { IT::IDDATATYPE::UINT };//параметры ф-ции sqrt

	struct  Entry
	{
		int idxfirstLE;
		char id[SCOPED_ID_MAXSIZE];
		IDDATATYPE iddatatype;
		IDTYPE idtype;
		union 
		{
			bool vbool;
			int vint;
			struct 
			{
				char len;
				char str[TI_STR_MAXSIZE - 1];
			}vstr;
			struct
			{
				int count;
				IDDATATYPE* types;
			}params;
		}value;
		Entry()							//конструктор без параметров
		{
			this->value.vint = TI_INT_DEFAULT;
			this->value.vstr.len = NULL;
			this->value.params.count = NULL;
		};
		Entry(char* id, int idxLT, IDDATATYPE datatype, IDTYPE idtype) //конструктор с параметрами
		{
			strncpy_s(this->id, id, SCOPED_ID_MAXSIZE - 1);
			this->idxfirstLE = idxLT;
			this->iddatatype = datatype;
			this->idtype = idtype;
		};
	};

	struct IdTable
	{
		int maxsize;
		int size;
		Entry* table;
	};

	IdTable Create(int size);

	void Add(IdTable& idtable, Entry entry);

	int IsId(IdTable& idtable, char id[SCOPED_ID_MAXSIZE]);
	bool SetValue(IT::Entry* entry, char* value);	//задать значение идентификатора
	bool SetValue(IT::IdTable& idtable, int index, char* value);
	void writeIdTable(std::ostream* stream, IT::IdTable& idtable); //вывести таблицу идентификаторов
	void DELETE(IdTable& idtable);

};