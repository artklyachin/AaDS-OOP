// unordered_map.h
//

#include <iostream>
#include <vector>
#include <algorithm>
#include <memory>
#include <iterator>
#include <limits>
#include <cassert>
#include <exception>
#include <cmath>

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
        {}
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

    explicit List(const Allocator& alloc = Allocator())
        : _head(nullptr), _size(0), _alloc(alloc)
    {
        allocate_head();
    }

    explicit List(size_t count, const T& value = T(), const Allocator& alloc = Allocator())
        : List(alloc)
    {
        for (int i = 0; i < int(count); ++i) {
            push_back(value);
        }
    }

    List(const List& other)
        : List(other._alloc)
    {
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
        {}

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
        {}
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

template <
    typename Key,
    typename Value,
    typename Hash = std::hash<Key>,
    typename Equal = std::equal_to<Key>,
    typename Alloc = std::allocator<std::pair<const Key, Value>>
>        
class UnorderedMap
{
public:
    using NodeType = std::pair<const Key, Value>;
    using AllocTraits = std::allocator_traits<Alloc>;
    using _Mutable_NodeType = std::pair<Key, Value>;
    using _MyNode = std::pair<NodeType, size_t>;
    using _MyNodeAlloc = typename std::allocator_traits<Alloc>::template rebind_alloc<_MyNode>;
    using _MyList = List<_MyNode, _MyNodeAlloc>;
    using _MyListIterator = typename _MyList::iterator;
    using _MyListConstIterator = typename _MyList::const_iterator;
    using _MyListNodePtr = typename _MyList::NodePtr;
    using _MyListNodeAlloc = typename _MyList::NodeAlloc;
    using _MyListNodeAllocTraits = typename _MyList::NodeAllocTraits;
    using _MyListIteratorAlloc = typename std::allocator_traits<Alloc>::template rebind_alloc<_MyListIterator>;
    using _MyListIteratorAllocTraits = std::allocator_traits<_MyListIteratorAlloc>;

public:
    UnorderedMap()
        : _list()
        , _buckets(_MinBuckets, _list.end())
        , _max_load_factor(1.0)
    {}

    UnorderedMap(const UnorderedMap& other)
        : _list(other._list)
        , _buckets(other._buckets.size(), _list.end())
        , _max_load_factor(other._max_load_factor)
    {
        _rebuild_buckets();
    }

    UnorderedMap(UnorderedMap&& other) noexcept
        : UnorderedMap()
    {
        _list.swap(other._list);
        _buckets.swap(other._buckets);
        std::swap(_max_load_factor, other._max_load_factor);
    }

    ~UnorderedMap()
    {}

    UnorderedMap& operator=(const UnorderedMap& other)
    {
        if (this != &other) {
            _list = other._list;
            _buckets.assign(other._buckets.size(), _list.end());
            _max_load_factor = other._max_load_factor;
            _rebuild_buckets();
        }
        return *this;
    }

    UnorderedMap& operator=(UnorderedMap&& other) noexcept
    {
        if (this != &other) {
            _list = std::move(other._list);
            _buckets = std::move(other._buckets);
            std::swap(_max_load_factor, other._max_load_factor);
        }
        return *this;
    }

    Value& operator[](const Key& key)
    {
        auto [it, hash] = _find(key);
        if (it != _list.end()) {
            return it->first.second;
        }
        size_t h = hash & _mask();
        _buckets[h] = _list.insert(_list.end(), std::make_pair(std::make_pair(key, Value{}), hash));
        return _buckets[h]->first.second;
    }

    Value& at(const Key& key)
    {
        auto [it, hash] = _find(key);
        if (it != _list.end()) {
            return it->first.second;
        }
        throw std::out_of_range("UnorderedMap::at(const Key& key)");
    }

    const Value& at(const Key& key) const
    {
        auto [it, hash] = _find(key);
        if (it != _list.end()) {
            return it->first.second;
        }
        throw std::out_of_range("UnorderedMap::at(const Key& key) const");
    }

    class ConstIterator
    {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = typename UnorderedMap::NodeType;
        using difference_type = std::ptrdiff_t;
        using pointer = const value_type*;
        using reference = const value_type&;

        ConstIterator()
        {}

        reference operator*() const
        {
            return _it->first;
        }

        pointer operator->() const
        {
            return &_it->first;
        }

        ConstIterator& operator++()
        {
            ++_it;
            return *this;
        }

        ConstIterator operator++(int)
        {
            ConstIterator temp = *this;
            ++_it;
            return temp;
        }

        ConstIterator& operator--()
        {
            --_it;
            return *this;
        }

        ConstIterator operator--(int)
        {
            ConstIterator temp = *this;
            --_it;
            return temp;
        }

        bool operator==(const ConstIterator& rhs) const
        {
            return _it == rhs._it;
        }

        bool operator!=(const ConstIterator& rhs) const
        {
            return !(*this == rhs);
        }

    private:
        friend class UnorderedMap;
        typename UnorderedMap::_MyList::const_iterator _it;

        ConstIterator(typename UnorderedMap::_MyList::const_iterator it)
            : _it(it)
        {}
    };

    class Iterator : public ConstIterator
    {
    public:
        using Base = ConstIterator;
        using pointer = typename Base::value_type*;
        using reference = typename Base::value_type&;

        using Base::Base;

        reference operator*() const noexcept
        {
            return const_cast<reference>(Base::operator*());
        }

        pointer operator->() const
        {
            return const_cast<pointer>(Base::operator->());
        }

        Iterator& operator++()
        {
            Base::operator++();
            return *this;
        }

        Iterator operator++(int)
        {
            Iterator temp = *this;
            Base::operator++();
            return temp;
        }

    private:
        friend class UnorderedMap;

        Iterator(typename UnorderedMap::_MyList::iterator it)
            : Base(it)
        {}
    };

    Iterator begin()
    {
        return Iterator(_list.begin());
    }

    Iterator end()
    {
        return Iterator(_list.end());
    }

    ConstIterator begin() const
    {
        return ConstIterator(_list.begin());
    }

    ConstIterator end() const
    {
        return ConstIterator(_list.end());
    }

    ConstIterator cbegin()
    {
        return ConstIterator(_list.begin());
    }

    ConstIterator cend()
    {
        return ConstIterator(_list.end());
    }

    template<typename... Args>
    std::pair<Iterator, bool> emplace(Args&& ...args)
    {
        _MyListNodePtr p = _list.allocate_free_node();
        Alloc alloc = static_cast<Alloc>(_list.get_allocator());
        AllocTraits::construct(alloc, &_list.nodeValue(p).first, std::forward<Args>(args)...);
        auto [it, hash] = _find(_list.nodeValue(p).first.first);
        if (it != _list.end()) {
            AllocTraits::destroy(alloc, &_list.nodeValue(p).first);
            _list.deallocate_free_node(p);
            return std::make_pair(Iterator(it), false);
        }
        _list.nodeValue(p).second = hash;
        reserve(size() + 1);
        size_t h = hash & _mask();
        _buckets[h] = _list.insert_node(_buckets[h], p);
        return std::make_pair(Iterator(_buckets[h]), true);
    }

    std::pair<Iterator, bool> insert(const NodeType& value)
    {
        return emplace(value);
    }

    std::pair<Iterator, bool> insert(NodeType&& value)
    {
        return emplace(reinterpret_cast<_Mutable_NodeType&&>(value));
    }

    template<class InputIt>
    void insert(InputIt first, InputIt last)
    {
        for (auto it = first; it != last; ++it) {
            insert(*it);
        }
    }

    Iterator erase(ConstIterator pos)
    {
        size_t h = pos._it->second & _mask();
        auto it = _list.erase(pos._it);
        if (_buckets[h] == pos._it) {
            if (it == _list.end() || h != (it->second & _mask())) {
                _buckets[h] = _list.end();
            } else {
                _buckets[h] = it;
            }
        }
        return Iterator(it);
    }

    Iterator erase(ConstIterator first, ConstIterator last)
    {
        auto it = first._it;
        while (it != last._it) {
            it = erase(ConstIterator(it))._it;
        }
        return Iterator(_list.remove_constness(it));
    }

    Iterator find(const Key& key)
    {
        auto [it, hash] = _find(key);
        if (it != _list.end()) {
            return Iterator(it);
        }
        return end();
    }

    ConstIterator find(const Key& key) const
    {
        auto [it, hash] = _find(key);
        if (it != _list.end()) {
            return ConstIterator(it);
        }
        return end();
    }

    bool empty() const
    {
        return size() == 0;
    }

    size_t size() const
    {
        return _list.size();
    }

    size_t max_size() const noexcept
    {
        return std::numeric_limits<std::ptrdiff_t>::max();
    }

    size_t bucket_count() const
    {
        return _buckets.size();
    }

    size_t max_bucket_count() const noexcept
    {
        return std::numeric_limits<std::ptrdiff_t>::max();
    }

    float load_factor() const
    {
        return static_cast<float>(size()) / bucket_count();
    }

    float max_load_factor() const
    {
        return _max_load_factor;
    }

    void max_load_factor(float ml)
    {
        assert(!std::isnan(ml) && ml > 0);
        _max_load_factor = ml;
        reserve(size());
    }

    void rehash(size_t count)
    {
        size_t newCount = bucket_count();
        while (newCount < count) {
            newCount *= 2;
        }
        if (newCount == bucket_count()) {
            return;
        }
        _buckets.assign(newCount, _list.end());
        auto sz = _list.size();
        for (size_t i = 0; i < sz; ++i) {
            size_t h = _list.begin()->second & _mask();
            _MyListNodePtr p = _list.detach_front();
            _buckets[h] = _list.insert_node(_buckets[h], p);
        }
    }

    void reserve(size_t size)
    {
        rehash(static_cast<size_t>(std::ceil(size / max_load_factor())));
    }

private:
    static constexpr size_t _MinBuckets = 8;

    _MyList _list;
    std::vector<_MyListIterator, _MyListIteratorAlloc> _buckets;
    float _max_load_factor;

    size_t _mask() const
    {
        return bucket_count() - 1;
    }

    std::pair<_MyListIterator, size_t> _find(const Key& key)
    {
        size_t hash = Hash{}(key);
        size_t h = hash & _mask();
        for (auto it = _buckets[h]; it != _list.end(); ++it) {
            if ((it->second & _mask()) != h) {
                break;
            }
            if (Equal{}(key, it->first.first)) {
                return std::make_pair(it, hash);
            }
        }
        return std::make_pair(_list.end(), hash);
    }

    std::pair<_MyListConstIterator, size_t> _find(const Key& key) const
    {
        size_t hash = Hash{}(key);
        size_t h = hash & _mask();
        for (auto it = _buckets[h]; it != _list.end(); ++it) {
            if ((it->second & _mask()) != h) {
                break;
            }
            if (Equal{}(key, it->first.first)) {
                return std::make_pair(it, hash);
            }
        }
        return std::make_pair(_list.end(), hash);
    }

    void _rebuild_buckets()
    {
        if (!empty()) {
            auto it = _list.begin();
            size_t h = it->second & _mask();
            _buckets[h] = it;
            for (; it != _list.end(); ++it) {
                if (h != (it->second & _mask())) {
                    h = it->second & _mask();
                    _buckets[h] = it;
                }
            }
        }
    }
};
