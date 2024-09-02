#pragma once

#include <functional>
#include <initializer_list>
#include <iostream>
#include <memory>
#include <stdexcept>

namespace usu
{
    template <typename T>
    class vector
    {
      public:
        // type aliases
        using size_type = std::size_t;
        using reference = T&;
        using pointer = std::shared_ptr<T[]>;
        using value_type = T;
        using resize_type = std::function<size_type(size_type)>;

        class iterator
        {
          public:
            using iterator_category = std::bidirectional_iterator_tag; // was std::forward_iterator_tag for deans demo

            iterator() :
                iterator(nullptr) // DefaultConstructable
            {
            }
            iterator(pointer ptr) :
                m_pos(0),
                m_data(ptr)

            {
            }
            iterator(size_type pos, pointer ptr) :
                m_pos(pos),
                m_data(ptr)
            {
            }

            iterator(const iterator& obj);     // CopyConstructable
            iterator(iterator&& obj) noexcept; // MoveConstructable

            iterator operator++();    // incrementable e.g., ++r
            iterator operator++(int); // incrementable e.g., r++

            iterator operator--();    // decrementable e.g., --r
            iterator operator--(int); // decrementable e.g., r--

            iterator& operator=(const iterator& rhs); // CopyAssignable
            iterator& operator=(iterator&& rhs);      // MoveAssignable

            reference operator*() // Derefrenceable
            {
                return this->m_data[this->m_pos];
            }

            T* operator->() // arrow
            {
                return &(this->m_data[this->m_pos]);
            }

            bool operator==(const iterator& rhs) { return this->m_pos == rhs.m_pos; }
            bool operator!=(const iterator& rhs) { return this->m_pos != rhs.m_pos; }

          private:
            size_type m_pos;
            pointer m_data;
        };

        // constructors
        vector() :
            vector(0, [](size_type currentCapacity)
                   {
                       return currentCapacity * 2;
                   })
        {
        }

        vector(size_type size) :
            vector(size, [](size_type currentCapacity)
                   {
                       return currentCapacity * 2;
                   })
        {
        }

        vector(resize_type resize) :
            vector(0, resize) {}

        // This is the real constructor
        vector(size_type size, resize_type resize);

        vector(std::initializer_list<T> list);

        vector(std::initializer_list<T> list, resize_type resize);

        // methods
        reference operator[](size_type index);

        void add(T value);

        void insert(size_type index, T value);

        void remove(size_type index);

        void clear() { m_size = 0; }

        size_type size() const { return m_size; }

        size_type capacity() const { return m_capacity; }

        iterator begin() { return iterator(m_data); }

        iterator end() { return iterator(m_size, m_data); }

      private:
        pointer m_data = std::make_shared<T[]>(10);
        size_type m_size = 0;
        size_type m_capacity = 10;

        resize_type capacityFunc = [](size_type currentCapacity)
        {
            return currentCapacity * 2;
        };

        void resize();
    };

    // Iterator methods
    template <typename T>
    vector<T>::iterator::iterator(const iterator& obj)
    {
        this->m_pos = obj.m_pos;
        this->m_data = obj.m_data;
    }

    template <typename T>
    vector<T>::iterator::iterator(iterator&& obj) noexcept
    {
        this->m_pos = obj.m_pos;
        this->m_data = obj.m_data;
        obj.m_pos = 0;
        obj.m_data = nullptr;
    }

    template <typename T>
    typename vector<T>::iterator vector<T>::iterator::operator++()
    {
        m_pos++;
        return *this;
    }

    template <typename T>
    typename vector<T>::iterator vector<T>::iterator::operator++(int)
    {
        iterator i = *this;
        m_pos++;
        return i;
    }

    template <typename T>
    typename vector<T>::iterator vector<T>::iterator::operator--()
    {
        m_pos--;
        return *this;
    }

    template <typename T>
    typename vector<T>::iterator vector<T>::iterator::operator--(int)
    {
        iterator i = *this;
        m_pos--;
        return i;
    }

    template <typename T>
    typename vector<T>::iterator& vector<T>::iterator::operator=(const iterator& rhs)
    {
        this->m_pos = rhs.m_pos;
        this->m_data = rhs.m_data;
        return *this;
    }

    template <typename T>
    typename vector<T>::iterator& vector<T>::iterator::operator=(iterator&& rhs)
    {
        if (this != &rhs)
        {
            std::swap(this->m_pos, rhs.m_pos);
            std::swap(this->m_data, rhs.m_data);
        }
        return *this;
    }

    // Vector constructors
    template <typename T>
    vector<T>::vector(size_type size, resize_type resize) :
        m_size(size), capacityFunc(resize)
    {
        if (m_size > 10)
        {
            m_capacity = capacityFunc(m_size);
        }
    }

    template <typename T>
    vector<T>::vector(std::initializer_list<T> list) :
        vector()
    {
        for (const auto& val : list)
        {
            add(val);
        }
    }

    template <typename T>
    vector<T>::vector(std::initializer_list<T> list, resize_type resize) :
        vector(resize)
    {
        for (const auto& val : list)
        {
            add(val);
        }
    }

    // Vector methods
    template <typename T>
    typename vector<T>::reference vector<T>::operator[](size_type index)
    {
        if (index >= m_size)
        {
            throw std::range_error("Index out of range");
        }
        return m_data[index];
    }

    template <typename T>
    void vector<T>::add(T value)
    {
        if (m_size == m_capacity)
        {
            resize();
        }
        m_data[m_size++] = value;
    }

    template <typename T>
    void vector<T>::insert(size_type index, T value)
    {
        if (index > m_size)
        {
            throw std::range_error("Index out of range");
        }

        if (m_size == m_capacity)
        {
            resize();
        }

        for (size_type i = m_size; i > index; --i) // sitch size and index?
        {
            m_data[i] = m_data[i - 1];
        }

        m_data[index] = value;
        ++m_size;
    }

    template <typename T>
    void vector<T>::remove(size_type index)
    {
        if (index >= m_size)
        {
            throw std::range_error("Index out of range");
        }

        for (size_type i = index; i < m_size - 1; ++i)
        {
            m_data[i] = m_data[i + 1];
        }

        --m_size;
    }

    template <typename T>
    void vector<T>::resize()
    {
        m_capacity = capacityFunc(m_capacity);
        auto newData = std::make_shared<T[]>(m_capacity);
        for (size_type i = 0; i < m_size; ++i)
        {
            newData[i] = m_data[i];
        }
        m_data = newData;
    }
} // namespace usu