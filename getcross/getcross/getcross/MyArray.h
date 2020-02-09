#pragma once

template <class Type>

// 为了尽可能提升效率，建议每次函数调用时都引用，省去深拷贝的时间
class MyArray {
public:
	Type *pointer;
	int len;

	MyArray() {
		pointer = nullptr;
		len = 0;
	}

	MyArray(int arraySize)
	{
		pointer = new Type[arraySize];
		len = arraySize;
	}

	// 深拷贝，防止多次释放内存
	MyArray(const MyArray &s)
	{
		this->pointer = new Type[s.len];
		for (int i = 0; i < s.len; i++)
			this->pointer[i] = s.pointer[i];
	}

	void reset(int arraysize)
	{
		delete[] pointer;
		pointer = new Type[arraysize];
		len = arraysize;
	}

	~MyArray()
	{
		delete[] pointer;
	}
};