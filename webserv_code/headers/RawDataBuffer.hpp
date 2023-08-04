/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RawDataBuffer.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/03 13:01:30 by codespace         #+#    #+#             */
/*   Updated: 2023/08/03 20:07:03 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RAW_DATA_BUFFER_HPP
# define RAW_DATA_BUFFER_HPP

# include "include.hpp"

class RawDataBuffer
{
    private:
        // Attributes
        size_t  _len;
        char    *_data;


    public:
        // Constructors & Copy Assignment & Destructor
        RawDataBuffer();
        RawDataBuffer(const RawDataBuffer &);
        RawDataBuffer &operator=(const RawDataBuffer &);
        ~RawDataBuffer();

    public:
        // methods
        bool            empty() const;
        void            append(const char *data, size_t len);
        RawDataBuffer   substr(size_t pos, size_t len) const;
        size_t          find(const char *pattern, size_t pos = 0);
        size_t          length() const;
        char            *c_str() const;
        char          operator[](size_t pos) const;
        void            clear();
        void            right_shift(size_t new_start);
};

#endif
