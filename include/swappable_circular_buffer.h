#ifndef SWAPPABLE_CIRCULAR_BUFFER_H
#define SWAPPABLE_CIRCULAR_BUFFER_H

#include <iostream>
#include <vector>

template <typename T>
class swappable_circular_buffer
{
    typedef std::vector<T> buffer;

public:
    swappable_circular_buffer(size_t buffer_length, size_t unit_length)
        : size_(0), buffer_(buffer_length), head_(buffer_.begin()), tail_(buffer_.begin())
    {
        for(T& e: buffer_)
            e.resize(unit_length);
    }

    int size() const
    {
        return size_;
    }

    void swap_head(T& other)
    {
        if(size_ + 1 > buffer_.size())
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
        {
            for(unsigned char byte: buffer_[i])
                std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)byte;
            std::cout << std::endl;
        }
    }

private:
    void increase_iterator_(typename buffer::iterator& iter)
    {
        iter = (iter + 1 == buffer_.end()) ? buffer_.begin(): iter + 1;
    }


    buffer buffer_;
    size_t size_;
    typename buffer::iterator head_;
    typename buffer::iterator tail_;
};

#endif // SWAPPABLE_CIRCULAR_BUFFER_H
