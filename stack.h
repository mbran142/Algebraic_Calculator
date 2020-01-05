#ifndef _STACK_H
#define _STACK_H

#include <iostream>
#include <stdlib.h>

template <class T>
struct Snode {
	T data;
	Snode<T>* next;
	Snode(const T& new_data, Snode<T>* next_node) : data(new_data), next(next_node) {};
};

template <class T>
class Stack {
public:
	Stack() : size(0), top(0) {};
	~Stack() { while (top != NULL) this->pop(); };
	void push(const T&);
	T pop();
	T& peek() const;
	bool isEmpty();
	int getSize() { return size; };
private:
	int size;
	Snode<T>* top;
};

template <class T>
void Stack<T>::push(const T& obj) {
	Snode<T>* p = new Snode<T>(obj, top);
	top = p;
	size++;
}

template <class T>
T Stack<T>::pop() {

	if (top == NULL) {
		std::cout << "Stack underflow. Program will exit." << std::endl;
		exit(1);
	}

	size--;
	Snode<T>* p = top;
	top = top->next;
	T data = p->data;
	delete p;
	return data;
}

template <class T>
T& Stack<T>::peek() const {
	
	if (top == NULL) {
		std::cout << "Stack underflow. Program will exit." << std::endl;
		exit(1);
	}

	return top->data;
}

template <class T>
bool Stack<T>::isEmpty() {
	return top == NULL;
}

template <class T>
class Queue {
public:
	Queue() : size(0), front(0), back(0) { };
	~Queue() { while (front != NULL) this->remove(); };
	void insert(const T&);
	T remove();
	T& query() const;
	bool isEmpty();
	int getSize() { return size; };
private:
	int size;
	Snode<T>* front;
	Snode<T>* back;
};

template <class T>
void Queue<T>::insert(const T& obj) {
	Snode<T>* p = new Snode<T>(obj, NULL);
	if (size) back->next = p;
	else front = p;
	back = p;
	size++;
}

template <class T>
T Queue<T>::remove() {

	if (front == NULL) {
		std::cout << "Queue underflow. Program will exit." << std::endl;
		exit(1);
	}

	size--;
	Snode<T>* p = front;
	front = front->next;
	if (!size) back = NULL;
	T data = p->data;
	delete p;
	return data;
}

template <class T>
T& Queue<T>::query() const {

	if (front == NULL) {
		std::cout << "Queue underflow. Program will exit." << std::endl;
		exit(1);
	}

	return front->data;
}

template <class T>
bool Queue<T>::isEmpty() {
	return !size;
}

#endif