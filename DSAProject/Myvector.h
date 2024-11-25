#include <iostream>
using namespace std;

template <typename T>
class MyVector {
private:

    T* data;      
    int size;
    int length;  

    void resize(int newSize) {
        T* newData = new T[newSize];
        for (int i = 0; i < length; ++i) {
            newData[i] = data[i];
        }
        delete[] data;
        data = newData;
        size = newSize;
    }

public:
    MyVector() : data(nullptr), size(0), length(0) {}

    ~MyVector() {
        delete[] data;
    }

    void push_back(const T& value) {
        if (length == size) 
        {
            if (size == 0)
            {
                resize(1);
            }
            else
            {
                resize(size * 2);
            }
        }
        data[length++] = value;
    }

    T& operator[](int index) 
    {
        if (index >= length) {
            cout<<"Index out of range"<<endl;
        }
        return data[index];
    }

    const T* begin() const {
        return data;
    }

    const T& operator[](int index) const {
        if (index >= length) {
            cout << "Index out of range" << endl;
        }
        return data[index];
    }

    int getSize() const {
        return length;
    }

    int getCapacity() const {
        return size;
    }

    T* begin() {
        return data; 
    }

    bool empty() const {
        return length == 0;
    }


    T* end() {
        return data + length; 
    }

    const T* end() const {
        return data + length;
    }

    void clear() {
        delete[] data;
        data = nullptr;
        size = 0;
        length = 0;
    }
};