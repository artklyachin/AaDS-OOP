#pragma once

#include <iostream>
#include <vector>
#include <algorithm>
#include <tuple>


template<class T>
class Deque
{
public:
    friend class iterator;
    friend class const_iterator;

    Deque()
        : front_off(0), back_off(0)
    {
    }


    Deque(const Deque& rhs)
        : arr(rhs.arr.size()), front_off(rhs.front_off), back_off(rhs.back_off)
    {
        try {
            for (intptr_t off = front_off; off < back_off; ++off) {
                alloc_block(off);
                new(&elem(off)) T(rhs.elem(off));
            }
        } catch (...) {
            clear();
            throw;
        }
    }


    Deque(size_t n)
        : arr(std::max<size_t>((n + blocksize - 1) / blocksize, 0)), front_off(0), back_off(0)
    {
        try {
            for (; size() < n; ++back_off) {
                alloc_block(back_off);
                new(&elem(back_off)) T;
            }
        } catch (...) {
            clear();
            throw;
        }
    }


    Deque(size_t n, const T& value)
        : arr(std::max<size_t>((n + blocksize - 1) / blocksize, 0)), front_off(0), back_off(0)
    {
        try {
            for (; size() < n; ++back_off) {
                alloc_block(back_off);
                new(&elem(back_off)) T(value);
            }
        } catch (...) {
            clear();
            throw;
        }
    }


    ~Deque()
    {
        clear();
    }

    Deque& operator=(const Deque& rhs)
    {
        if (this != &rhs) {
            Deque copy(rhs);
            std::swap(arr, copy.arr);
            std::swap(front_off, copy.front_off);
            std::swap(back_off, copy.back_off);
        }
        return *this;
    }


    size_t size() const
    {
        return back_off - front_off;
    }


    T& operator[](intptr_t index)
    {
        return elem(front_off + index);
    }


    const T& operator[](intptr_t index) const
    {
        return elem(front_off + index);
    }


    T& at(intptr_t index)
    {
        if (index < 0 || index >= static_cast<intptr_t>(size())) throw std::out_of_range("Deque::at");
        return operator[](index);
    }


    const T& at(intptr_t index) const
    {
        if (index < 0 || index >= static_cast<intptr_t>(size())) throw std::out_of_range("Deque::at");
        return operator[](index);
    }


    void push_back(const T& value)
    {
        try {
            if (back_off >= capacity()) {
                resize();
            }
            alloc_block(back_off);
            new(&elem(back_off)) T(value);
            ++back_off;
        } catch (...) {
            if (back_off % blocksize == 0) {
                free_block(back_off);
            }
            throw;
        }
    }


    void pop_back()
    {
        elem(back_off - 1).~T();
        --back_off;
        if (back_off % blocksize == 0) {
            free_block(back_off + 1);
        }
    }


    void push_front(const T& value)
    {
        try {
            if (front_off == 0) {
                resize();
            }
            alloc_block(front_off - 1);
            new(&elem(front_off - 1)) T(value);
            --front_off;
        } catch (...) {
            if (front_off % blocksize == 0) {
                free_block(front_off - 1);
            }
            throw;
        }
    }


    void pop_front()
    {
        elem(front_off).~T();
        front_off += 1;
        if (front_off % blocksize == 0) {
            free_block(front_off - 1);
        }
    }


private:
    T*& block(intptr_t off)
    {
        return arr[off / blocksize];
    }

    const T& elem(intptr_t off) const
    {
        return arr[off / blocksize][off % blocksize];
    }

    T& elem(intptr_t off)
    {
        return arr[off / blocksize][off % blocksize];
    }



public:
    /*


    const_iterator


    */


    class const_iterator
    {
    protected:
        friend class iterator;
        const Deque<T>* deq = nullptr;
        intptr_t off = 0;
    public:
        using iterator_category = std::random_access_iterator_tag;
        using value_type = T;
        using difference_type = intptr_t;
        using pointer = T*;
        using reference = T&;

        const_iterator() : deq(nullptr), off(0) {}
        const_iterator(const Deque<T>* deq, intptr_t off) : deq(deq), off(off) {}
        const_iterator(const const_iterator& it) : deq(it.deq), off(it.off) {}

        const T& operator*() const
        {
            return deq->elem(off);
        }

        const T* operator->() const
        {
            return &deq->elem(off);
        }

        const_iterator& operator=(const const_iterator& rhs)
        {
            if (this != &rhs) {
                deq = rhs.deq;
                off = rhs.off;
            }
            return *this;
        }


        bool operator==(const const_iterator& rhs) const
        {
            return std::tie(deq, off) == std::tie(rhs.deq, rhs.off);
        }

        bool operator!=(const const_iterator& rhs) const
        {
            return !operator==(rhs);
        }

        bool operator<(const const_iterator& rhs) const
        {
            return std::tie(deq, off) < std::tie(rhs.deq, rhs.off);
        }

        bool operator>(const const_iterator& rhs) const
        {
            return rhs.operator<(*this);
        }

        bool operator<=(const const_iterator& rhs) const
        {
            return !operator>(rhs);
        }

        bool operator>=(const const_iterator& rhs) const
        {
            return !operator<(rhs);
        }

        const_iterator& operator+=(intptr_t rhs)
        {
            off += rhs;
            return *this;
        }


        const_iterator& operator-=(intptr_t rhs)
        {
            return operator+=(-rhs);
        }


        const_iterator operator++()
        {
            return operator+=(1);
        }

        const_iterator operator++(int)
        {
            const_iterator result = *this;
            operator++();
            return result;
        }


        const_iterator operator--()
        {
            return operator-=(1);
        }


        const_iterator operator--(int)
        {
            const_iterator result = *this;
            operator--();
            return result;
        }

        const_iterator operator+(intptr_t rhs) const
        {
            const_iterator result = *this;
            return result.operator+=(rhs);
        }

        const_iterator operator-(intptr_t rhs) const
        {
            const_iterator result = *this;
            return result.operator-=(rhs);
        }

        intptr_t operator-(const const_iterator& rhs) const
        {
            return off - rhs.off;
        }
    };

    /*
iterator
iterator
iterator
iterator
 */

    class iterator : public const_iterator
    {
    private:
        using super = const_iterator;
        using super::deq;
        using super::off;

    public:
        iterator() : super() {}
        iterator(Deque<T>* deq, intptr_t off) : super(deq, off) {}
        iterator(const iterator& it) : super(it) {}

        T& operator*() const
        {
            return const_cast<Deque<T>*>(deq)->elem(off);
        }

        T* operator->() const
        {
            return &const_cast<Deque<T>*>(deq)->elem(off);
        }

        iterator& operator=(const iterator& rhs)
        {
            super::operator=(rhs);
            return *this;
        }


        iterator& operator+=(intptr_t rhs)
        {
            super::operator+=(rhs);
            return *this;
        }


        iterator& operator-=(intptr_t rhs)
        {
            super::operator-=(rhs);
            return *this;
        }


        iterator operator++()
        {
            super::operator++();
            return *this;
        }

        iterator operator++(int)
        {
            iterator result = *this;
            operator++();
            return result;
        }


        iterator operator--()
        {
            super::operator--();
            return *this;
        }


        iterator operator--(int)
        {
            iterator result = *this;
            operator--();
            return result;
        }

        iterator operator+(intptr_t rhs) const
        {
            iterator result = *this;
            result += rhs;
            return result;
        }

        iterator operator-(intptr_t rhs) const
        {
            iterator result = *this;
            result -= rhs;
            return result;
        }

        intptr_t operator-(const iterator& rhs) const
        {
            return this->off - rhs.off;
        }
    };

    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    using reverse_iterator = std::reverse_iterator<iterator>;


    iterator begin()
    {
        return iterator(this, front_off);
    }

    iterator end()
    {
        return iterator(this, back_off);
    }

    const_iterator begin() const
    {
        return cbegin();
    }

    const_iterator end() const
    {
        return cend();
    }

    const_iterator cbegin() const
    {
        return const_iterator(this, front_off);
    }

    const_iterator cend() const
    {
        return const_iterator(this, back_off);
    }

    reverse_iterator rbegin()
    {
        return std::make_reverse_iterator(end());
    }

    reverse_iterator rend()
    {
        return std::make_reverse_iterator(begin());
    }

    const_reverse_iterator rbegin() const
    {
        return crbegin();
    }

    const_reverse_iterator rend() const
    {
        return crend();
    }

    const_reverse_iterator crbegin() const
    {
        return std::make_reverse_iterator(cend());
    }

    const_reverse_iterator crend() const
    {
        return std::make_reverse_iterator(cbegin());
    }


    void insert(iterator it, const T& elem)
    {
        if (it == end()) {
            push_back(elem);
        } else {
            iterator it2 = end() - 1;
            push_back(*it2);
            while (it2 > it) {
                *it2 = *(it2 - 1);
                --it2;
            }
            *it = elem;
        }
    }


    void erase(iterator it)
    {
        iterator it2 = it;
        while (it2 < end() - 1) {
            *it2 = *(it2 + 1);
            ++it2;
        }
        pop_back();
    }


    void print_deque()
    {
        std::cout << "front_off = " << front_off << " back_off = " << back_off << " sz = " << size() << " capacity = " << capacity() << std::endl;
        std::cout << "end - begin = " << end() - begin();
        std::cout << " cend - cbegin = " << cend() - cbegin();
        std::cout << " rend - rbegin = " << rend() - rbegin();
        std::cout << " crend - crbegin = " << crend() - crbegin() << std::endl;

        for (intptr_t off = front_off; off < back_off; ++off) {
            std::cout << "\"" << elem(off) << "\" ";
            if (off % blocksize == blocksize - 1) {
                std::cout << "-> " << std::endl;
            }
        }
        std::cout << "<- " << std::endl;
    }

private:
    static const intptr_t blocksize = 5;

    std::vector<T*> arr;
    intptr_t front_off = 0, back_off = 0;


    void clear()
    {
        if (front_off != back_off) {
            for (; back_off > front_off; --back_off) {
                elem(back_off - 1).~T();
                if ((back_off - 1) % blocksize == 0) {
                    free_block(back_off - 1);
                }
            }
            free_block(front_off);
        }
    }


    void alloc_block(intptr_t off)
    {
        if (block(off) == nullptr) {
            block(off) = reinterpret_cast<T*>(new int8_t[blocksize * sizeof(T)]);
        }
    }

    void free_block(intptr_t off)
    {
        delete[] reinterpret_cast<int*>(block(off));
        block(off) = nullptr;
    }

    intptr_t capacity() const
    {
        return static_cast<intptr_t>(arr.size()) * blocksize;
    }


    void resize()
    {
        intptr_t frontblock = front_off / blocksize, lastblock = (back_off + blocksize - 1) / blocksize - 1, blocks = lastblock - frontblock + 1;
        intptr_t vecsize = static_cast<intptr_t>(arr.size());
        intptr_t newvecsize = std::max<intptr_t>(2 * vecsize, 4);
        std::vector<T*> newarr(newvecsize);
        intptr_t newfrontblock = newvecsize / 2 - blocks / 2;
        for (intptr_t i = 0; i < blocks; ++i) {
            std::swap(newarr[newfrontblock + i], arr[frontblock + i]);
        }
        std::swap(arr, newarr);
        uintptr_t shift = (newfrontblock - frontblock) * blocksize;
        front_off += shift;
        back_off += shift;
    }


};



template<typename T>
typename Deque<T>::iterator operator+(intptr_t lhs, const typename Deque<T>::iterator& rhs)
{
    return rhs + lhs;
}

template<typename T>
typename Deque<T>::const_iterator operator+(intptr_t lhs, const typename Deque<T>::const_iterator& rhs)
{
    return rhs + lhs;
}
