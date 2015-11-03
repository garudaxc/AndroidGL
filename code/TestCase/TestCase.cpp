// TestCase.cpp : 定义控制台应用程序的入口点。
//

#include <stdio.h>
#include <tchar.h>
#include "../RingBuffer.h"


class TestClass
{
public:
	TestClass()
	{
	}

	~TestClass()
	{
	}

private:
	int value;

};



int _tmain(int argc, _TCHAR* argv[])
{
	RingBuffer<TestClass> buffer(10);

	RingBuffer<int> buffer2(10);

	buffer2.push_back(1);
	buffer2.push_back(2);
	buffer2.push_back(3);
	buffer2.push_back(4);


	printf("%d\n", buffer2.capacity());
	printf("%d\n", buffer2.size());
	printf("%d\n", buffer2.front());
	printf("%d\n", buffer2.back());

	return 0;
}

