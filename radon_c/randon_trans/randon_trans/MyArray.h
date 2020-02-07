#pragma once

template <class Type>

// Ϊ�˾���������Ч�ʣ�����ÿ�κ�������ʱ�����ã�ʡȥ�����ʱ��
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

	// �������ֹ����ͷ��ڴ�
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