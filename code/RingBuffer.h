#pragma once




template<typename T>
class RingBuffer
{
public:

	typedef typename T value_type;
	typedef typename size_t size_type;
	typedef typename T* pointer;
	typedef typename const T* const_pointer;
	typedef typename T& reference;
	typedef typename const T& const_reference;
	

	RingBuffer(size_type capacity);
	~RingBuffer();

	
	size_type size() const {
		return 0;
	}

	size_type capacity() const{
		return 0;
	}

	bool empty() const {
		return true;
	}

	
	void clear() {

	}

	const_reference front() const {

	}
	
	const_reference back() const {

	}

	void push_back(const_reference v) {

	}

	void pop_front() {

	}

	const_reference at(size_t index) {

	}
	
private:

	size_type	capacity_;
	pointer		data_;
	size_type	first_;
	size_type	last_;
};

RingBuffer::RingBuffer()
{
}

RingBuffer::~RingBuffer()
{
}