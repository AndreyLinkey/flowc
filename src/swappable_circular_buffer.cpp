#include "../include/swappable_circular_buffer.h"

template <typename T, size_t buffer_length>
swappable_circular_buffer::swappable_circular_buffer()
    : size_(0), head_(buffer_.begin()), tail_(buffer_.begin())
{

}

template <typename T, size_t buffer_length>
void swappable_circular_buffer::swap_head(T& other)
{
    if(size_ + 1 > buffer_length)
        throw std::length_error("overflow");

    std::swap(*head_, other);
    increase_iterator_(head_);
    size_ += 1;
}

template <typename T, size_t buffer_length>
void swappable_circular_buffer::swap_tail(T& other)
{
    if(size_ - 1 < 0)
        throw std::out_of_range("empty");

    std::swap(*tail_, other);
    increase_iterator_(tail_);
    size_ -= 1;
}

template <typename T, size_t buffer_length>
void swappable_circular_buffer::print()
{
    for(int i = 0; i < buffer_.size(); ++i)
        std::cout << ' ' << buffer_[i];
    std::cout << std::endl;
}

template <typename T, size_t buffer_length>
void swappable_circular_buffer::increase_iterator_(typename swappable_circular_buffer::buffer::iterator& iter)
{
    iter = (iter + 1 == buffer_.end()) ? buffer_.begin(): iter + 1;
}
