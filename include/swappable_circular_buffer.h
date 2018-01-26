#ifndef SWAPPABLE_CIRCULAR_BUFFER_H
#define SWAPPABLE_CIRCULAR_BUFFER_H

#include <array>
#include <iostream>

template <typename T, size_t buffer_length>
class swappable_circular_buffer
{
    typedef std::array<T, buffer_length> buffer;

public:
    swappable_circular_buffer()
        : size_(0), head_(buffer_.begin()), tail_(buffer_.begin())
    {

    }

    swappable_circular_buffer(const T& value)
        : swappable_circular_buffer()
    {
        buffer_.fill(value);
    }

    int size() const
    {
        return size_;
    }

    void swap_head(T& other)
    {
        if(size_ + 1 > buffer_length)
            throw std::length_error("overflow");

        std::swap(*head_, other);
        increase_iterator_(head_);
        size_ += 1;
    }

    void swap_tail(T& other)
    {
        if(size_ - 1 < 0)
            throw std::out_of_range("empty");

        std::swap(*tail_, other);
        increase_iterator_(tail_);
        size_ -= 1;
    }

    void print() const
    {
        for(int i = 0; i < buffer_.size(); ++i)
            std::cout << ' ' << buffer_[i];
        std::cout << std::endl;
    }

private:
    void increase_iterator_(typename buffer::iterator& iter)
    {
        iter = (iter + 1 == buffer_.end()) ? buffer_.begin(): iter + 1;
    }


    buffer buffer_;
    int size_;
    typename buffer::iterator head_;
    typename buffer::iterator tail_;
};

/*
int main()
{
//    std::vector<char> v1({'f', 'o', 'o'});
//    std::vector<char> v2({'b', 'a', 'r'});
//    std::vector<char> v3({'x', 'y', 'z'});
//    std::vector<char> v4;
//    std::vector<char> v5;

    int i1 = 10;
    int i2 = 42;
    int i3 = 75;
    int i4 = 15;
    int i5 = 32;

    swappable_circular_buffer<int, 0> scb;
    scb.swap_head(i1);
    scb.swap_head(i2);
    scb.swap_head(i3);
    scb.print();

    int temp1 = 1;
    scb.swap_tail(temp1);
    scb.print();
    std::cout << temp1 << std::endl;

    scb.swap_head(i4);
    scb.print();

    int temp2 = 22;
    scb.swap_tail(temp2);
    scb.print();
    std::cout << temp2 << std::endl;

    //scb.swap_head(i5);
} */


#endif // SWAPPABLE_CIRCULAR_BUFFER_H
