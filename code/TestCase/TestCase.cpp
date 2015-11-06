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
		printf("construct\n");
	}

	TestClass(const TestClass& other)
	{
		printf("copy construct\n");
	}

	~TestClass()
	{
		printf("distruct\n");
	}

private:
	int value;

};

#define VERIFY( v ) if (!(v)) {\
	\
	printf("\'%s\' failed! at in %s line %d\n", #v, __FILE__, __LINE__); \
}



int _tmain(int argc, _TCHAR* argv[])
{
	{
		RingBuffer<int> buffer(4);

		VERIFY(buffer.empty());
		buffer.push_back(1);
		buffer.push_back(2);
		buffer.push_back(3);
		buffer.push_back(4);
		VERIFY(buffer.full());

		buffer.pop_front();
		VERIFY(buffer.size() == 3);

		VERIFY(buffer.capacity() == 4);
		VERIFY(buffer.front() == 2);
		VERIFY(buffer.back() == 4);

		buffer.push_back(10);
		buffer.push_back(20);
		VERIFY(buffer.full());
		VERIFY(buffer.front() == 3);
		VERIFY(buffer.at(2) == 10);

		for (size_t i = 0; i < buffer.size(); i++) {
			printf("%d\n", buffer.at(i));
		}

		buffer.push_back(99);

		for (size_t i = 0; i < buffer.size(); i++) {
			printf("%d\n", buffer.at(i));
		}
	}

	{
		RingBuffer<TestClass> buffer;		
		buffer.reserve(10);

		VERIFY(buffer.capacity() == 10);

		buffer.push_back(TestClass());
		buffer.pop_front();
	}
	



	return 0;
}

