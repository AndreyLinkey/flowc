#ifndef SWAPPABLE_CIRCULAR_BUFFER_H
#define SWAPPABLE_CIRCULAR_BUFFER_H

#include <iomanip>
#include <iostream>
#include <vector>
#include <shared_mutex>

template <typename T>
class swappable_circular_buffer
{
    typedef std::vector<T> buffer;

public:
    swappable_circular_buffer(size_t buffer_length, size_t unit_length = 0)
        : size_(0), buffer_(buffer_length), head_(buffer_.begin()), tail_(buffer_.begin())
    {
        if(unit_length > 0)
        {
            for(T& e: buffer_)
                e.resize(unit_length);
        }
    }

    size_t size() const
    {
        //std::shared_lock<std::shared_mutex> lock(buffer_mutex_);
        return size_;
    }

    void swap_head(T& other)
    {
        //std::lock_guard<std::shared_mutex> lock(buffer_mutex_);
        if(size_ + 1 > buffer_.size())
        {
            throw std::length_error("overflow, max buffer_size is " + std::to_string(buffer_.size()));
        }

        std::swap(*head_, other);
        increase_iterator_(head_);
        size_ += 1;
    }

    void swap_tail(T& other)
    {
        //std::lock_guard<std::shared_mutex> lock(buffer_mutex_);
        if(size_ == 0)
        {
            throw std::out_of_range("empty");
        }

        std::swap(*tail_, other);
        increase_iterator_(tail_);
        size_ -= 1;
    }

    size_t unit_length() const
    {
        //std::shared_lock<std::shared_mutex> lock(buffer_mutex_);
        return head_->size();
    }

    void print() const
    {
        //std::shared_lock<std::shared_mutex> lock(buffer_mutex_);
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
    mutable std::shared_mutex buffer_mutex_;
    typename buffer::iterator head_;
    typename buffer::iterator tail_;
};

#endif // SWAPPABLE_CIRCULAR_BUFFER_H
