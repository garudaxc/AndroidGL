#pragma once
#include <assert.h>
#include <malloc.h>

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
	
	RingBuffer() :capacity_(0), size_(0), first_(0), data_(NULL)
	{

	}

	RingBuffer(size_type capacity) :capacity_(0), size_(0), first_(0), data_(NULL)
	{
		Alloc(capacity);
	}

	~RingBuffer()
	{

	}

	
	size_type size() const {
		return size_;
	}

	size_type capacity() const{
		return capacity_;
	}

	bool empty() const {
		return (size_ == 0);
	}

	bool full() const {
		return (size_ == capacity_);
	}

	
	void clear() {

	}

	const_reference front() const {
		assert(!empty());

		return data_[first_];
	}
	
	const_reference back() const {
		assert(!empty());

		size_type i = (first_ + size_ -1) % capacity_;
		return data_[i];
	}

	void push_back(const_reference v) {
		if (full()) {
			pop_front();
		}

		size_type i = (first_ + size_) % capacity_;
		new(data_ + i)(v);
		size_++;
	}

	void pop_front() {
		assert(!empty());
		
	}

	const_reference at(size_t index) {
		assert(!empty() && index < size());

		size_type i = (first_ + index) % capacity_;
		return data_[i];
	}
	
private:

	template<typename T>
	RingBuffer(const RingBuffer<T>& other)	{}

	template<typename T>
	const RingBuffer<T>& operator = (const RingBuffer<T>& other)	{ return *this; }

	void Alloc(size_type count)
	{
		assert(data_ == NULL);
		
		data_ = (pointer)malloc(sizeof(value_type) * count);
		first_	= 0;
		size_	= 0;
		capacity_ = count;
	}

	size_type	capacity_;
	size_type	first_;
	size_type	size_;
	pointer		data_;
};
