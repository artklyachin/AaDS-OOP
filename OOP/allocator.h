#include <iostream>
#include <vector>
#include <algorithm>
#include <list>
#include <memory>
#include <iterator>
#include <cassert>
#include <string>

template <size_t chunkSize>
class FixedAllocator
{
public:
    FixedAllocator()
    {
    }

    ~FixedAllocator()
    {
        for (auto* p : blocks) {
            ::operator delete(p);
        }
    }

    void* allocate()
    {
        if (blocks.empty() || index == chunkCount) {
            blocks.push_back(static_cast<uint8_t*>(::operator new(chunkCount * chunkSize)));
            index = 0;
        }
        return blocks.back() + chunkSize * index++;
    }

    void deallocate(void* p)
    {
    }

private:
    const size_t chunkCount = 65536;
    std::vector<uint8_t*> blocks;
    size_t index = 0;
};

template<typename T>
class FastAllocator
{
public:
    FastAllocator()
        : allocator(new FixedAllocator<24>)
    {
    }

    template <class U> friend class FastAllocator;

    template <class U>
    FastAllocator(const FastAllocator<U>& other)
        : allocator(other.allocator)
    {
    }

    ~FastAllocator()
    {
    }

    template <class U>
    FastAllocator& operator=(const FastAllocator<U>& other)
    {
        allocator = other.allocator;
        return *this;
    }

    T* allocate(size_t n, const void* = nullptr)
    {
        size_t sz = n * sizeof(T);
        if (sz <= 24) {
            return reinterpret_cast<T*>(allocator->allocate());
        } else {
            return reinterpret_cast<T*>(::operator new(sz));
        }
    }

    void deallocate(T* p, size_t n)
    {
        size_t sz = n * sizeof(T);
        if (sz > 24) {
            ::operator delete(p);
        }
    }

    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;
    using value_type = T;
    template <typename U> struct rebind { using other = FastAllocator<U>; };

    auto select_on_container_copy_construction() const
    {
        return *this;
    }

    bool operator==(const FastAllocator& other) const
    {
        return allocator == other.allocator;
    }

    bool operator!=(const FastAllocator& other) const
    {
        return !operator==(other);
    }

private:
    std::shared_ptr<FixedAllocator<24>> allocator;
};

template<typename T, typename Allocator = std::allocator<T>>
class List
{
    struct Node
    {
        Node* _prev;
        Node* _next;
        T _value; 

        Node(Node* prev, Node* next, const T& value)
            : _prev(prev), _next(next), _value(value)
        {
        }

        Node(Node* prev, Node* next)
            : _prev(prev), _next(next)
        {
        }
    };

public:
    using RebindAlloc = typename std::allocator_traits<Allocator>::template rebind_alloc<Node>;
    using AllocTraits = std::allocator_traits<RebindAlloc>;
    using allocator_type = Allocator;
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;

    Node* _head = nullptr;
    size_t _size = 0;
    RebindAlloc _alloc;

    explicit List(const Allocator& _alloc = Allocator())
        : _head(nullptr), _size(0), _alloc(_alloc)
    {
        allocate_head();
    }

    explicit List(size_t count, const T& value, const Allocator& _alloc = Allocator())
        : _head(nullptr), _size(0), _alloc(_alloc)
    {
        allocate_head();
        for (int i = 0; i < int(count); ++i) {
            push_back(value);
        }
    }

    explicit List(size_t count, const Allocator& _alloc = Allocator())
        : _head(nullptr), _size(0), _alloc(_alloc)
    {
        allocate_head();
        for (int i = 0; i < int(count); ++i) {
            emplace(cend());
        }
    }

    List(const List& other)
        : _head(nullptr), _size(0), _alloc(std::allocator_traits<Allocator>::select_on_container_copy_construction(other.get_allocator()))
    {
        allocate_head();
        append(other);
    }

    ~List()
    {
        dispose();
    }

private:
    void allocate_head()
    {
        _head = AllocTraits::allocate(_alloc, 1);
        _head->_prev = _head;
        _head->_next = _head;
    }

    void dispose()
    {
        Node* p = _head->_next;
        while (p != _head) {
            Node* next = p->_next;
            AllocTraits::destroy(_alloc, p);
            AllocTraits::deallocate(_alloc, p, 1);
            p = next;
        }
        AllocTraits::deallocate(_alloc, _head, 1);
        _head = nullptr;
        _size = 0;
    }

    void append(const List& other)
    {
        for (const T& value : other) {
            push_back(value);
        }
    }

public:
    Allocator get_allocator() const noexcept
    {
        return Allocator(_alloc);
    }

    List& operator=(const List& other)
    {
        if (this != &other) {
            dispose();
            if (AllocTraits::propagate_on_container_copy_assignment::value) {
                _alloc = other._alloc;
            }
            allocate_head();
            append(other);
        }
        return *this;
    }

    size_t size() const
    {
        return _size;
    }

    void push_back(const T& value)
    {
        insert(cend(), value);
    }

    void push_front(const T& value)
    {
        insert(cbegin(), value);
    }

    void pop_back()
    {
        erase(std::prev(cend()));
    }

    void pop_front()
    {
        erase(cbegin());
    }

    class const_iterator
    {
    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = typename List::value_type;
        using difference_type = std::ptrdiff_t;
        using pointer = typename List::const_pointer;
        using reference = const value_type&;

        const_iterator()
            : _node(nullptr)
        {
        }

        const_iterator(List::Node* node)
            : _node(node)
        {
        }

        reference operator*() const
        {
            return _node->_value;
        }

        pointer operator->() const
        {
            return &_node->_value;
        }

        const_iterator& operator++()
        {
            _node = _node->_next;
            return *this;
        }

        const_iterator operator++(int)
        {
            const_iterator temp = *this;
            _node = _node->_next;
            return temp;
        }

        const_iterator& operator--()
        {
            _node = _node->_prev;
            return *this;
        }

        const_iterator operator--(int)
        {
            const_iterator temp = *this;
            _node = _node->_prev;
            return temp;
        }

        bool operator==(const const_iterator& rhs) const
        {
            return _node == rhs._node;
        }

        bool operator!=(const const_iterator& rhs) const
        {
            return !(*this == rhs);
        }

        Node* _node; 
    };

    class iterator : public const_iterator
    {
    public:
        using Base = const_iterator;
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = typename List::value_type;
        using difference_type = std::ptrdiff_t;
        using pointer = typename List::pointer;
        using reference = value_type&;

        using Base::Base;

        reference operator*() const noexcept
        {
            return const_cast<reference>(Base::operator*());
        }

        pointer operator->() const
        {
            return const_cast<pointer>(&Base::_node->_value);
        }

        iterator& operator++()
        {
            Base::operator++();
            return *this;
        }

        iterator operator++(int)
        {
            iterator temp = *this;
            Base::operator++();
            return temp;
        }

        iterator& operator--()
        {
            Base::operator--();
            return *this;
        }

        iterator operator--(int)
        {
            iterator temp = *this;
            Base::operator--();
            return temp;
        }
    };

    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    iterator begin()
    {
        return iterator(_head->_next);
    }

    iterator end()
    {
        return iterator(_head);
    }

    const_iterator begin() const
    {
        return const_iterator(_head->_next);
    }

    const_iterator end() const
    {
        return const_iterator(_head);
    }

    const_iterator cbegin()
    {
        return const_iterator(_head->_next);
    }

    const_iterator cend()
    {
        return const_iterator(_head);
    }

    reverse_iterator rbegin()
    {
        return reverse_iterator(end());
    }

    reverse_iterator rend()
    {
        return reverse_iterator(begin());
    }

    const_reverse_iterator rbegin() const
    {
        return const_reverse_iterator(end());
    }

    const_reverse_iterator rend() const
    {
        return const_reverse_iterator(begin());
    }

    const_reverse_iterator crbegin()
    {
        return const_reverse_iterator(end());
    }

    const_reverse_iterator crend()
    {
        return const_reverse_iterator(begin());
    }

    iterator insert(const_iterator next, const T& value)
    {
        const_iterator prev = std::prev(next);
        Node* p = AllocTraits::allocate(_alloc, 1);
        AllocTraits::construct(_alloc, p, prev._node, next._node, value);
        prev._node->_next = p;
        next._node->_prev = p;
        ++_size;
        return iterator(p);
    }

    iterator emplace(const_iterator next)
    {
        const_iterator prev = std::prev(next);
        Node* p = AllocTraits::allocate(_alloc, 1);
        AllocTraits::construct(_alloc, p, prev._node, next._node);
        prev._node->_next = p;
        next._node->_prev = p;
        ++_size;
        return iterator(p);
    }

    iterator erase(const_iterator pos)
    {
        auto prev = std::prev(pos);
        auto next = std::next(pos);
        prev._node->_next = next._node;
        next._node->_prev = prev._node;
        AllocTraits::destroy(_alloc, pos._node);
        AllocTraits::deallocate(_alloc, pos._node, 1);
        --_size;
        return iterator(next._node);
    }
};

using namespace std;

template<typename T>
void print_list(T& container)
{
    for (auto& v : container) {
        cout << v << " ";
    }
    cout << endl;
}

template<typename T>
void print_reversed_list(T& container)
{
    for (auto it = container.rbegin(); it != container.rend(); ++it) {
        cout << *it << " ";
    }
    cout << endl;
}
