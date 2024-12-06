#pragma once

#include <iostream>
using namespace std;

class MyString {
private:
    char* str;
    size_t length;

public:
    // Default constructor
    MyString() : str(nullptr), length(0) {}

    // Constructor that takes a const char* argument
    MyString(const char* s) {
        if (s) {
            length = 0;
            while (s[length] != '\0') {
                ++length;
            }
            str = new char[length + 1];
            for (size_t i = 0; i < length; ++i) {
                str[i] = s[i];
            }
            str[length] = '\0';
        }
        else {
            str = nullptr;
            length = 0;
        }
    }

    // Copy constructor
    MyString(const MyString& other) {
        length = other.length;
        if (length > 0) {
            str = new char[length + 1];
            for (size_t i = 0; i < length; ++i) {
                str[i] = other.str[i];
            }
            str[length] = '\0';
        }
        else {
            str = nullptr;
        }
    }

    MyString(int num) {
        // Convert integer to string (simple approach, assuming non-negative integers)
        length = 0;
        int temp = num;
        while (temp > 0) {
            length++;
            temp /= 10;
        }
        if (length == 0) length = 1;  // For zero, it should have one digit

        str = new char[length + 1];
        str[length] = '\0';

        for (int i = length - 1; i >= 0; --i) {
            str[i] = '0' + (num % 10); // Get last digit and convert to char
            num /= 10;
        }
    }

    // Destructor
    ~MyString() {
        delete[] str;
    }

    size_t getlength()const{
        return this->length;
    }

    // Assignment operator
    MyString& operator=(const MyString& other) {
        if (this != &other) {
            delete[] str;

            length = other.length;
            if (length > 0) {
                str = new char[length + 1];
                for (size_t i = 0; i < length; ++i) {
                    str[i] = other.str[i];
                }
                str[length] = '\0';
            }
            else {
                str = nullptr;
            }
        }
        return *this;
    }

    // Concatenation operator
    MyString operator+(const MyString& other) const {
        size_t newLength = length + other.length;
        char* newStr = new char[newLength + 1];

        size_t i = 0;
        for (; i < length; ++i) {
            newStr[i] = str[i];
        }
        for (size_t j = 0; j < other.length; ++j, ++i) {
            newStr[i] = other.str[j];
        }
        newStr[newLength] = '\0';

        MyString result;
        result.str = newStr;
        result.length = newLength;
        return result;
    }

    // Less than operator
    bool operator<(const MyString& other) const {
        size_t i = 0;
        while (i < length && i < other.length) {
            if (str[i] < other.str[i]) return true;
            if (str[i] > other.str[i]) return false;
            ++i;
        }
        return length < other.length;
    }

    // Greater than operator
    bool operator>(const MyString& other) const {
        size_t i = 0;
        while (i < length && i < other.length) {
            if (str[i] > other.str[i]) return true;
            if (str[i] < other.str[i]) return false;
            ++i;
        }
        return length > other.length;
    }

    // Equality operator
    bool operator==(const MyString& other) const {
        if (length != other.length) return false;
        for (size_t i = 0; i < length; ++i) {
            if (str[i] != other.str[i]) return false;
        }
        return true;
    }

    char& operator[](size_t index) {
        if (index >= length) {
            std::cerr << "Index out of bounds!" << std::endl;
            exit(1); // Exit the program if the index is invalid
        }
        return str[index];
    }

    // Const subscript operator
    const char& operator[](size_t index) const {
        if (index >= length) {
            std::cerr << "Index out of bounds!" << std::endl;
            exit(1); // Exit the program if the index is invalid
        }
        return str[index];
    }

    // Overload << operator (friend function)
    friend std::ostream& operator<<(std::ostream& os, const MyString& obj) {
        for (size_t i = 0; i < obj.getlength(); ++i) {
            os << obj[i];  // Use the subscript operator to access the characters
        }
        return os;  // Return the ostream object to allow chaining (e.g., std::cout << obj1 << obj2;)
    }

    friend std::istream& operator>>(std::istream& is, MyString& myStr) {
        myStr.clear();  // Clear the string before reading
        char c;
        while (is.get(c)) {
            if (isspace(c)) { // Stop at whitespace
                break;
            }
            myStr.append(c); // Append character to MyString
        }
        return is;
    }

    const char* c_str() const {
        return str;
    }

    char* c_str_mutable() {
        return str;  // Return the mutable C-style string
    }

    size_t find(char c, size_t start = 0) const {
        if (start >= length) return -1;  // Return npos if start is beyond length

        for (size_t i = start; i < length; i++) {
            if (str[i] == c) {
                return i;
            }
        }
        return -1;  // Return npos if not found
    }

    // Find the first occurrence of a substring (like string.find())
    size_t find(const MyString& substr, size_t start = 0) const {
        if (start >= length || substr.getlength() > length) return -1;

        for (size_t i = start; i <= length - substr.getlength(); i++) {
            bool match = true;
            for (size_t j = 0; j < substr.getlength(); j++) {
                if (str[i + j] != substr[j]) {
                    match = false;
                    break;
                }
            }
            if (match) {
                return i;
            }
        }
        return -1;  // Return npos if not found
    }

    // Substring function to get a substring (like string.substr())
    MyString substr(size_t start, size_t length = -1) const {
        if (start >= this->length) return MyString("");  // Return empty if start is beyond length

        if (length == static_cast<size_t>(-1) || start + length > this->length) {
            length = this->length - start;
        }

        char* subStr = new char[length + 1];
        for (size_t i = 0; i < length; i++) {
            subStr[i] = str[start + i];
        }
        subStr[length] = '\0';

        MyString result(subStr);
        delete[] subStr; // Cleanup temporary substring buffer
        return result;
    }



    // Function to print the string
    void print() const {
        if (str) {
            std::cout << str;
        }
    }

    void clear() {
        delete[] str;
        str = nullptr;
        length = 0;
    }

    void append(char c) {
        char* newStr = new char[length + 2];
        for (size_t i = 0; i < length; i++) {
            newStr[i] = str[i];
        }
        newStr[length] = c;
        newStr[length + 1] = '\0';
        delete[] str;
        str = newStr;
        length++;
    }



    
};


std::istream& getLine(std::istream& is, MyString& myStr) {
    myStr.clear();  // Clear any previous content
    char c;
    while (is.get(c)) {
        if (c == '\n') {
            break;
        }
        myStr.append(c);
    }
    return is;
}

MyString to_string(int value) {
    char buffer[50];
    snprintf(buffer, sizeof(buffer), "%d", value);  // Convert int to string
    return MyString(buffer);
}



