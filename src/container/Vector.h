/**
 * Programming II Project
 * SE106 - 2015 Spring
 *
 * =============================
 * container/Vector.h
 * ----------------------------
 * Vector implementation.
 * =============================
 */

// Header Protection
#ifndef CONTAINER_VECTOR_H
#define CONTAINER_VECTOR_H

// Header files
#include "../common.h"

/**
 * Template Class Vector
 *
 * Dynamic array. Code from Homework 2.
 */
template <typename T>
class Vector {
private:
	// elements array
	T* elements;

	// length of allocated array
	int capacity;

	// count of elements stored
	int count;

	// expand array length
	void expandCapacity();
public:
	// Constructor
	Vector();
	Vector(int size, const T& val = T());
	Vector(const Vector& x);

	// Destructor
	~Vector();

	// Copy assignment operator
	Vector<T>& operator=(const Vector<T>& x);

	// Get size
	int size() const;

	// Empty judgement
	bool empty() const;

	// Array access operator
	T& operator [](int pos);
	const T& operator [](int pos) const;

	// Resize the vector
	void resize(size_t n, T val = T());

	// Push back element
	void pushBack(const T& val);

	// Extra interface
	void sort();

	// Iterator implementation

	class iterator :
		public std::iterator<std::random_access_iterator_tag, T> {

	private:
		const Vector *vp;
		int index;

	public:

		iterator() {
			this->vp = NULL;
		}

		iterator(const iterator & it) {
			this->vp = it.vp;
			this->index = it.index;
		}

		iterator(const Vector *vp, int index) {
			this->vp = vp;
			this->index = index;
		}

		iterator & operator++() {
			index++;
			return *this;
		}

		iterator operator++(int) {
			// make a copy of the old iterator
			iterator old_itr(*this);
			index++;
			return old_itr;
		}

		iterator & operator--() {
			index--;
			return *this;
		}

		iterator operator--(int) {
			// make a copy of the old iterator
			iterator old_itr(*this);
			index--;
			return old_itr;
		}

		bool operator==(const iterator & rhs) {
			return (this->vp == rhs.vp && this->index == rhs.index);
		}

		bool operator!=(const iterator & rhs) {
			return !(*this == rhs);
		}

		bool operator<(const iterator & rhs) {
			if (this->vp != rhs.vp) {
				throw std::invalid_argument("Unable to compare between iteators of diffenent vectors!");
			}
			return (this->index < rhs.index);
		}

		bool operator<=(const iterator & rhs) {
			if (this->vp != rhs.vp) {
				throw std::invalid_argument("Unable to compare between iteators of diffenent vectors!");
			}
			return (*this == rhs || *this < rhs);
		}

		bool operator>(const iterator & rhs) {
			return !(*this <= rhs);
		}

		bool operator>=(const iterator & rhs) {
			return  !(*this < rhs);
		}

		iterator operator+(const int & rhs) {
			iterator added_itr(*this);
			added_itr.index += rhs;
			return added_itr;
		}

		iterator operator-(const int & rhs) {
			iterator added_itr(*this);
			added_itr.index -= rhs;
			return added_itr;
		}

		int operator-(const iterator & rhs) {
			if (this->vp != rhs.vp) {
				throw std::invalid_argument("Unable to compare between iteators of diffenent vectors!");
			}
			return (this->index - rhs.index);
		}

		T & operator*() {
			return vp->elements[index];
		}

		T & operator[](int k) {
			return vp->elements[index + k];
		}

	};

	// Begin iterator
	iterator begin() const {
		return iterator(this, 0);
	}

	// End iterator
	iterator end() const {
		return iterator(this, count);
	}

};


// (default constructor)
// Constructs an empty container, with no elements.
template <typename T>
Vector<T>::Vector() {
	// init capacity
	count = capacity = 0;
	// init array with an empty pointer
	elements = NULL;
}

// (fill constructor)
// Constructs a container with n elements.
// Each element is a copy of val.
template <typename T>
Vector<T>::Vector(int size, const T& val) {
	// check size
	if (size < 0)
		throw std::invalid_argument("Size must be zero or positive!");

	// init capacity
	count = capacity = size;
	// init array with an empty pointer
	elements = (size == 0) ? NULL : new T[size];

	// fill values
	for (int i = 0; i < count; ++i) {
		elements[i] = val;
	}
}

// (copy constructor)
// Constructs a container with a copy of each of the
// elements in x, in the same order.
template <typename T>
Vector<T>::Vector(const Vector& x) {
	// init capacity
	count = capacity = x.count;
	// init array with an empty pointer
	elements = (count == 0) ? NULL : new T[count];

	// fill values
	for (int i = 0; i < count; ++i) {
		elements[i] = x.elements[i];
	}
}

// (destructor)
// Destroys the container object.
template <typename T>
Vector<T>::~Vector() {
	if (elements != NULL) {
		delete[] elements;
	}
}

// operator=
// Assigns new contents to the container, replacing its
// current contents, and modifying its size accordingly.
template <typename T>
Vector<T>& Vector<T>::operator=(const Vector<T>& x) {
	// avoid repeated copy
	if (this != &x) {
		// copy the vector first in case allocation fails
		Vector tmp(x);
		// then deallocate previous array
		if (elements != NULL)
			delete[] elements;

		// move tmp data to this (like std::move() in C++ 11)
		count = tmp.count;
		capacity = tmp.capacity;
		elements = tmp.elements;

		// clear tmp vector
		tmp.count = tmp.capacity = 0;
		tmp.elements = NULL;
	}
	return *this;
}

// size
// Returns the number of elements in the vector.
// This is the number of actual objects held in the
// vector, which is not necessarily equal to its
// storage capacity.
template <typename T>
int Vector<T>::size() const {
	return count;
}

// empty
// Returns whether the vector is empty
template <typename T>
bool Vector<T>::empty() const {
	return (count == 0);
}

// operator[]
// Returns a reference to the element at position n
// in the vector container.
// if the requested position is out of range, the
// behavior is undefined
template <typename T>
T& Vector<T>::operator[](int pos) {
	return elements[pos];
}

template <typename T>
const T& Vector<T>::operator[](int pos) const {
	return elements[pos];
}

// resize
// Resizes the container so that it contains n elements.
// If n is smaller than the current container size, the 
// content is reduced to its first n elements, removing 
// those beyond (and destroying them).
// If n is greater than the current container size, the 
// content is expanded by inserting at the end as many 
// elements as needed to reach a size of n. If val is 
// specified, the new elements are initialized as copies 
// of val, otherwise, they are value-initialized.
template <typename T>
void Vector<T>::resize(size_t n, T val) {
	if (n <= static_cast<unsigned int>(count)) {			// smaller than size
		count = n;
	}
	else {					// greater than size
		while (count != n) {
			pushBack(val);
		}
	}
}

// pushBack
// Adds a new element at the end of the vector, after its
// current last element. The content of val is copied (or
// moved) to the new element.
template <typename T>
void Vector<T>::pushBack(const T& val) {
	if (count == capacity) {
		expandCapacity();
	}

	// push back new value
	elements[count++] = val;
}

// sort
// Sort the vector.
template <typename T>
void Vector<T>::sort() {
	std::sort(this->begin(), this->end());
}

template <typename T>
void Vector<T>::expandCapacity() {
	// try allocating
	int new_capacity = std::max(1, capacity * 2);
	T* new_elements = new T[new_capacity];

	// copy values
	for (int i = 0; i < count; ++i) {
		new_elements[i] = elements[i];
	}

	// deallocate old array
	if (elements != NULL)
		delete[] elements;

	// update self
	elements = new_elements;
	capacity = new_capacity;
}
// End of Class Vector

#endif
// End of Header Protection
