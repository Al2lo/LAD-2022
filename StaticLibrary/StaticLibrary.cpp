#include "pch.h"
#include <iostream>
#include <fstream>
#include "Math.h"
#include <time.h>

extern "C"
{


	char* __stdcall rowmerg(char* buffer, char* str1, char* str2)
	{
		int i = NULL, len1 = NULL, len2 = NULL;
		for (int j = 0; str1[j] != '\0'; j++)
		{
			if (i == 255)
				break;
			buffer[i++] = str1[j];
		}
		for (int j = 0; str2[j] != '\0'; j++)
		{
			if (i == 255)
				break;
			buffer[i++] = str2[j];
		}
		buffer[i] = '\0';
		return buffer;
	}

	int __stdcall powNumb(char* ptr, int num, int steprn)
	{
		int res = pow(num, steprn);
			if (res > 4294967295)
			{
				std::cout << "Overflow in pow()" << std::endl;
				throw - 1;
			}
		return res;
	}

	int __stdcall sqrtNumb(char* ptr, int a)
	{
		return 	sqrt(a);
	}

	int __stdcall paused(int z, char* ptr) {
		std::cout << std::endl;
		system("pause");
		return 0;
	}

	void __stdcall printints(int value)
	{
		std::cout << value;
		std::cout << "\n";
		return ;
	}

	void __stdcall printstr(char* ptr)
	{
		if (ptr == nullptr)
		{
			std::cout << std::endl;
		}
		for (int i = 0; ptr[i] != '\0'; i++)
			std::cout << ptr[i];
		std::cout << "\n";
		return ;
	}

	int __stdcall proverkaUint(int number)
	{
		setlocale(LC_ALL, "Rus");
		if (number < 0)
		{
			std::cout << "Ошибка 320: Семантическая ошибка: Присвоение значения не входящего в диапозон данного типа" << "\n";
			return 0;
		}
		return 1;
	}

	int __stdcall proverkaDel(int number)
	{
		setlocale(LC_ALL, "Rus");
		if (number == 0)
		{
			std::cout << "Ошибка 319: Семантическая ошибка: Деление на ноль" << "\n";
			return 0;
		}
		return 1;
	}
}
