#pragma once
#include <assert.h>
#include <malloc.h>
#include <new>

template<typename T>
class RingBuffer
{
public:

	typedef T value_type;
	typedef size_t size_type;
	typedef T* pointer;
	typedef const T* const_pointer;
	typedef T& reference;
	typedef const T& const_reference;
	
	RingBuffer() :capacity_(0), size_(0), first_(0), data_(NULL)
	{
	}

	RingBuffer(size_type capacity) :capacity_(0), size_(0), first_(0), data_(NULL)
	{
		Realloc(capacity);
	}

	~RingBuffer() {
		clear();
		Realloc(0);
	}

	void reserve(size_type capacity) {
		clear();
		Realloc(capacity);
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
		while (!empty()) {
			pop_front();
		}
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
		::new(data_ + i) value_type(v);
		size_++;
	}

	void pop_front() {
		assert(!empty());
		
		data_[first_].~value_type();
		first_ = ++first_ % capacity_;
		size_--;
	}

	const_reference at(size_t index) const {
		assert(!empty() && index < size());

		size_type i = (first_ + index) % capacity_;
		return data_[i];
	}
	
private:

	// ring buffer should not copy to each other
	RingBuffer(const RingBuffer<T>& other)	{}
	const RingBuffer<T>& operator = (const RingBuffer<T>& other)	{ return *this; }

	void Realloc(size_type count)
	{
		if (data_ != NULL) {
			::free(data_);
			data_ = NULL;
		}

		if (count > 0) {
			data_ = (pointer)malloc(sizeof(value_type)* count);
		}
		
		first_	= 0;
		size_	= 0;
		capacity_ = count;
	}

	size_type	capacity_;
	size_type	first_;
	size_type	size_;
	pointer		data_;
};
