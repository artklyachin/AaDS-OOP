#include <iostream>
#include <cstring>
#include <assert.h>
#include <cctype>


class String {
public:
    String(const char* str);
    String(size_t n, char c);
    String();
    String(const String& Str);
    ~String();
    String& operator=(const String& Str);
    bool operator==(const String& Str) const;
    char operator[](size_t index) const;
    char& operator[](size_t index);
    size_t length() const;
    void push_back(char elem);
    char pop_back();
    char front() const;
    char back() const;
    char& front();
    char& back();
    String& operator+=(char elem);
    String& operator+=(const String& Str);
    size_t find(const String& Str) const;
    size_t rfind(const String& Str) const;
    String substr(size_t start, size_t count) const;
    bool empty() const;
    void clear();
    friend std::istream& operator>>(std::istream& in, String& Str);
    friend std::ostream& operator<<(std::ostream& out, const String& Str);

private:
    String(const char* str, size_t len);
    void increase_buffer(size_t delta);
    void reduce_buffer();

    size_t size;
    size_t capacity;
    char* string;

};


String::String(const char* str, size_t len)
{
    size = len;
    capacity = 2 * size;
    string = new char[capacity];
    memcpy(string, str, size);
}


String::String(const char* str)
    : String(str, strlen(str))
{
}


String::String(size_t len, char c)
{
    size = len;
    capacity = (size == 0) ? 2 : (2 * size);
    string = new char[capacity];
    memset(string, c, size);
}


String::String()
{
    size = 0;
    capacity = 2;
    string = new char[capacity];
}


String::String(const String& Str)
{
    size = Str.size;
    capacity = Str.capacity;
    string = new char[capacity];
    memcpy(string, Str.string, size);
}


String::~String()
{
    delete[] string;
}


String& String::operator=(const String& Str)
{
    if (*this == Str) return *this;
    delete[] string;
    size = Str.size;
    capacity = Str.capacity;
    string = new char[capacity];
    memcpy(string, Str.string, size);
    return *this;
}


bool String::operator==(const String& Str) const
{
    if (size != Str.size) return false;
    return memcmp(string, Str.string, size) == 0;
}


char String::operator[](size_t index) const
{
    return string[index];
}


char& String::operator[](size_t index)
{
    return string[index];
}


size_t String::length() const
{
    return size;
}


void String::push_back(char elem)
{
    increase_buffer(1);
    string[size++] = elem;
}


char String::pop_back()
{
    char delete_elem = string[--size];
    reduce_buffer();
    return delete_elem;
}


char String::front() const
{
    return string[0];
}


char String::back() const
{
    return string[size - 1];
}


char& String::front()
{
    return string[0];
}


char& String::back()
{
    return string[size - 1];
}


String& String::operator+=(char elem)
{
    push_back(elem);
    return *this;
}


String& String::operator+=(const String& Str)
{
    increase_buffer(Str.size);
    memcpy(string + size, Str.string, Str.size);
    size += Str.size;
    return *this;
}


String operator+(const String& Str, char elem)
{
    String copy = Str;
    copy += elem;
    return copy;
}


String operator+(char elem, const String& Str)
{
    String copy = String(1, elem);
    copy += Str;
    return copy;
}


String operator+(const String& Str, const String& added_Str)
{
    String copy = Str;
    copy += added_Str;
    return copy;
}


size_t String::find(const String& Str) const
{
    if (size < Str.size) {
        return size;
    }
    for (size_t i = 0; i < size - Str.size + 1; ++i) {
        bool flag = false;
        for (size_t j = 0; j < Str.size; ++j) {
            if (string[i + j] != Str[j]) {
                flag = true;
                break;
            }
        }
        if (!flag) {
            return i;
        }
    }
    return size;
}


size_t String::rfind(const String& Str) const
{
    if (size < Str.size) {
        return size;
    }
    size_t first = size - Str.size - 1;
    for (size_t i = 0; i < size - Str.size; ++i) {
        bool flag = false;
        for (size_t j = 0; j < Str.size; ++j) {
            if (string[(first - i) + j] != Str[j]) {
                flag = true;
                break;
            }
        }
        if (!flag) {
            return (first - i);
        }
    }
    return size;
}


String String::substr(size_t start, size_t count) const
{
    return String(string + start, count);
}


bool String::empty() const
{
    return size == 0;
}


void String::clear()
{
    delete[] string;
    size = 0;
    capacity = 2;
    string = new char[capacity];
}


std::istream& operator>>(std::istream& in, String& Str)
{
    char elem;
    while (in.get(elem) && !std::isspace(elem)) {
        Str.push_back(elem);
    }
    return in;
}


std::ostream& operator<<(std::ostream& out, const String& Str)
{
    for (size_t i = 0; i < Str.length(); ++i) {
        out << Str[i];
    }
    return out;
}


void String::increase_buffer(size_t delta)
{
    if (size + delta <= capacity) return;
    capacity = 2 * (size + delta);
    char* newstring = new char[capacity];
    memcpy(newstring, string, size);
    delete[] string;
    string = newstring;
}


void String::reduce_buffer()
{
    if (size >= capacity / 4) return;
    capacity /= 2;
    char* newstring = new char[capacity];
    memcpy(newstring, string, size);
    delete[] string;
    string = newstring;
}

