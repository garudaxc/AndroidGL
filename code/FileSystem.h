#pragma once
#include <stdio.h>
#include <string>

using namespace std;

#define MAGIC(a, b, c, d) (a << 24 | b << 16 | c << 8 | d)
#define MAGIC_STR(str) (str[0] << 24 | str[1] << 16 | str[2] << 8 | str[3])

class File
{
public:
	virtual size_t	Size() const = 0;
	virtual bool	Read(void* data, int size) = 0;
	virtual void	Seek(size_t pos) = 0;
	virtual void	Close() = 0;

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
};



class FileSystem
{
public:
	virtual void	SetRootPath(const string& path, bool addPak) = 0;
	virtual bool	AddPakFile(const string& pathname) = 0;

	virtual File*	OpenFile(const string& pathname) = 0;
	virtual bool	FileExist(const string& pathname) = 0;
};


extern FileSystem* GFileSys;