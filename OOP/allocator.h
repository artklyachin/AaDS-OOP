#include <vector>
#include <algorithm>
#include <memory>
#include <iterator>
#include <limits>

template <size_t ChunkSize, typename = std::enable_if_t<ChunkSize >= sizeof(uint8_t*)>>
class FixedAllocator
{
public:
    FixedAllocator()
    { }

    ~FixedAllocator()
    {
        for (auto* p : blocks) {
            ::operator delete(p);
        }
    }

    void* allocate()
    {
        if (next_free == nullptr) {
            uint8_t* p = static_cast<uint8_t*>(::operator new(ChunkCount * ChunkSize));
            blocks.push_back(p);
            for (int i = ChunkCount - 1; i >= 0; --i) {
                add_free_chunk(p + i * ChunkSize);
            }
        }
        void* chunk = reinterpret_cast<void*>(next_free);
        next_free = *reinterpret_cast<uint8_t**>(next_free);
        return chunk;
    }

    void deallocate(void* p)
    {
        if (p != nullptr) {
            add_free_chunk(reinterpret_cast<uint8_t*>(p));
        }
    }

private:
    void add_free_chunk(uint8_t* p)
    {
        *reinterpret_cast<uint8_t**>(p) = next_free;
        next_free = p;
    }

    constexpr static size_t ChunkCount = 65536;
    std::vector<uint8_t*> blocks;
    uint8_t* next_free = nullptr;
};

template<typename T>
class FastAllocator
{
public:
    FastAllocator()
        : allocator(new FixedAllocator<AllocSize>)
    { }

    template <class U> friend class FastAllocator;

    template <class U>
    FastAllocator(const FastAllocator<U>& other)
        : allocator(other.allocator)
    { }

    ~FastAllocator()
    { }

    template <class U>
    FastAllocator& operator=(const FastAllocator<U>& other)
    {
        allocator = other.allocator;
        return *this;
    }

    T* allocate(size_t n, const void* = nullptr)
    {
        size_t sz = n * sizeof(T);
        if (sz <= AllocSize) {
            return reinterpret_cast<T*>(allocator->allocate());
        } else {
            return reinterpret_cast<T*>(::operator new(sz));
        }
    }

    void deallocate(T* p, size_t n)
    {
        size_t sz = n * sizeof(T);
        if (sz <= AllocSize) {
            allocator->deallocate(p);
        } else {
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
    constexpr static int AllocSize = 24;
    std::shared_ptr<FixedAllocator<AllocSize>> allocator;
};

template<
    typename T,
    typename Allocator = std::allocator<T>
>
class List
{
private:
    struct Node
    {
        friend class List;
        Node* _prev;
        Node* _next;
        T _value; // unused for head node

        Node(Node* prev, Node* next, const T& value)
            : _prev(prev), _next(next), _value(value)
        { }

        Node(Node* prev, Node* next)
            : _prev(prev), _next(next)
        { }
    };

public:
    using NodeAlloc = typename std::allocator_traits<Allocator>::template rebind_alloc<Node>;
    using NodeAllocTraits = std::allocator_traits<NodeAlloc>;
    using NodePtr = Node*;
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;

    Node* _head = nullptr;
    size_t _size = 0;
    NodeAlloc _alloc;

    List(const Allocator& alloc = Allocator())
        : _head(nullptr), _size(0), _alloc(alloc)
    {
        allocate_head();
    }

    List(size_t count, const T& value, const Allocator& alloc = Allocator())
        : List(alloc)
    {
        for (int i = 0; i < int(count); ++i) {
            push_back(value);
        }
    }

    List(size_t count, const Allocator& alloc = Allocator())
        : List(alloc)
    {
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

    List(List&& other) noexcept
        : List(other._alloc)
    {
        std::swap(_head, other._head);
        std::swap(_size, other._size);
    }

    ~List()
    {
        dispose();
    }

private:
    void allocate_head()
    {
        _head = allocate_free_node();
        _head->_prev = _head;
        _head->_next = _head;
    }

    void dispose()
    {
        Node* p = _head->_next;
        while (p != _head) {
            Node* next = p->_next;
            NodeAllocTraits::destroy(_alloc, p);
            NodeAllocTraits::deallocate(_alloc, p, 1);
            p = next;
        }
        deallocate_free_node(_head);
        _size = 0;
        _head = nullptr;
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
        return static_cast<Allocator>(_alloc);
    }

    List& operator=(const List& other)
    {
        if (this != &other) {
            dispose();
            if (NodeAllocTraits::propagate_on_container_copy_assignment::value) {
                _alloc = other._alloc;
            }
            allocate_head();
            append(other);
        }
        return *this;
    }

    List& operator=(List&& other) noexcept
    {
        if (this != &other) {
            std::swap(_head, other._head);
            std::swap(_size, other._size);
            if (NodeAllocTraits::propagate_on_container_move_assignment::value) {
                _alloc = std::move(other._alloc);
            }
        }
        return *this;
    }

    void swap(List& other)
    {
        std::swap(_head, other._head);
        std::swap(_size, other._size);
        if (NodeAllocTraits::propagate_on_container_swap::value) {
            std::swap(_alloc, other._alloc);
        }
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
        { }

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

    private:
        friend class List;
        Node* _node; // pointer to node

        const_iterator(List::Node* node)
            : _node(node)
        { }
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
            return const_cast<pointer>(Base::operator->());
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

    private:
        friend class List;

        iterator(List::Node* node)
            : const_iterator(node)
        {}
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

    template<typename... Args>
    iterator emplace(const_iterator next, Args&& ...args)
    {
        const_iterator prev = std::prev(next);
        Node* p = NodeAllocTraits::allocate(_alloc, 1);
        NodeAllocTraits::construct(_alloc, p, prev._node, next._node, std::forward<Args>(args)...);
        prev._node->_next = p;
        next._node->_prev = p;
        ++_size;
        return iterator(p);
    }

    iterator insert(const_iterator next, const T& value)
    {
        return emplace(next, value);
    }

    iterator insert(const_iterator next, T&& value)
    {
        return emplace(next, std::move(value));
    }

    iterator erase(const_iterator pos)
    {
        auto prev = std::prev(pos);
        auto next = std::next(pos);
        prev._node->_next = next._node;
        next._node->_prev = prev._node;
        NodeAllocTraits::destroy(_alloc, pos._node);
        NodeAllocTraits::deallocate(_alloc, pos._node, 1);
        --_size;
        return iterator(next._node);
    }


    Node* allocate_free_node()
    {
        return NodeAllocTraits::allocate(_alloc, 1);
    }

    void deallocate_free_node(Node* p)
    {
        NodeAllocTraits::deallocate(_alloc, p, 1);
    }

    static T& nodeValue(Node* p)
    {
        return p->_value;
    }

    static iterator remove_constness(const_iterator it)
    {
        return iterator(it._node);
    }

    iterator insert_node(const_iterator next, Node* p)
    {
        const_iterator prev = std::prev(next);
        p->_prev = prev._node;
        p->_next = next._node;
        prev._node->_next = p;
        next._node->_prev = p;
        ++_size;
        return iterator(p);
    }

    Node* detach_front()
    {
        if (size() == 0) {
            return nullptr;
        }
        Node* p = _head->_next;
        _head->_next = p->_next;
        _head->_next->_prev = _head;
        --_size;
        return p;
    }

};
