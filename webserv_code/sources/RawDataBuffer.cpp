/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RawDataBuffer.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: obednaou <obednaou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/03 13:06:41 by codespace         #+#    #+#             */
/*   Updated: 2023/08/05 15:32:36 by obednaou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "RawDataBuffer.hpp"

// Constructors & Destructor

RawDataBuffer::RawDataBuffer() : _len(0), _data(NULL) {}

RawDataBuffer::RawDataBuffer(const RawDataBuffer &other) : _len(0), _data(NULL)
{
    *this = other;
}

RawDataBuffer &RawDataBuffer::operator=(const RawDataBuffer &other)
{
    // delete old data's memory
    delete[] _data;
    _data = NULL;
    _len = other._len;

    if (other.empty())
        return (*this);

    // Allocate memory for new data
    _data = new char[other._len + 1];

    // Copy new data
    for (size_t i = 0; i < other._len; i++)
        _data[i] = other._data[i];
    _data[other._len] = '\0';

    return (*this);
}

char RawDataBuffer::operator[](size_t pos) const
{
    return (_data[pos]);
}

RawDataBuffer::~RawDataBuffer()
{
    delete[] _data;
}

// Methods

void RawDataBuffer::append(const char *data, size_t len)
{
    if (!len)
        return ;

    char *new_data = new char[_len + len + 1];

    // Copying old data
    for (size_t i = 0; i < _len; i++)
        new_data[i] = _data[i];

    // Appending new data
    for (size_t i = 0; i < len; i++)
        new_data[i + _len] = data[i];

    new_data[_len + len] = '\0';

    // deleting old memory
    delete[] _data;

    // updating the attributes
    _len += len;
    _data = new_data;
}

RawDataBuffer RawDataBuffer::substr(size_t pos, size_t len) const
{
    RawDataBuffer ret;

    if (this->empty() || !len)
        return (ret);
    if (pos + len > _len)
        len = _len - pos;
    ret.append(_data + pos, len);
    return (ret);
}

size_t  RawDataBuffer::find(const char *pattern, size_t pos)
{
    bool    stop_flag = false;
    size_t  pattern_len = strlen(pattern);

    if (pos >= _len)
        return (std::string::npos);
    for (size_t i = pos; i < _len; i++)
    {
        for (size_t j = 0; j < pattern_len; j++)
        {
            if (_data[j + i] != pattern[j])
                break ;
            if (j == pattern_len - 1)
            {
                stop_flag = true;
                break ;
            }
        }
        if (stop_flag)
            return (i);
    }
    return (std::string::npos);
}

bool RawDataBuffer::empty() const
{
    return (!_len);
}

char *RawDataBuffer::c_str() const
{
    return (_data);
}

size_t RawDataBuffer::length() const
{
    return (_len);
}

void    RawDataBuffer::clear()
{
    _len = 0;
    delete _data;
    _data = NULL;
}

void    RawDataBuffer::right_shift(size_t new_start)
{
    // std::cout << "NEW LENGTH: " <<  _len - new_start << std::endl;
    // std::cout << "_LEN: " << _len << std::endl;
    // std::cout << "_NEW_START: " << new_start << std::endl;
    *this = substr(new_start, _len - new_start);
    // std::cout << "NO SEGFAULT HERE" << std::endl;
}
