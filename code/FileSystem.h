#pragma once
#include <stdio.h>
//#include <stdlib.h>

#define MAGIC(a, b, c, d) (a << 24 | b << 16 | c << 8 | d)
#define MAGIC_STR(str) (str[0] << 24 | str[1] << 16 | str[2] << 8 | str[3])

class File
{
public:
	File();
	~File();

	bool	Open(const char* fileName);
	void	Close();

	bool	Read(void* data, int size);

	template<class type>
	bool	Read(type &c) {
		bool r = Read(&c, sizeof(c));
		return r;
	}

	template<class type>
	bool	ReadArray(type* array, int count){
		bool r = Read(array, sizeof(type)* count);
		return r;
	}

private:
	FILE*	pf_;
};
