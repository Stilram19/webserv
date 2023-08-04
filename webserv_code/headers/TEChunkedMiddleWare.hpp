/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TEChunkedMiddleWare.hpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/01 10:55:23 by codespace         #+#    #+#             */
/*   Updated: 2023/08/03 17:31:59 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef T_E_CHUNKED_MIDDLE_WARE_HPP
# define T_E_CHUNKED_MIDDLE_WARE_HPP

# include "include.hpp"
# include "RawDataBuffer.hpp"

class TEChunkedMiddleWare
{
    private:
        // Attributes
        bool            no_chunk_currently;
        bool            end_of_body;
        size_t          curr_chunk_size;
        RawDataBuffer   iner_buffer;
        RawDataBuffer   extracted_chunk;

        // Useless Copy Constructor & Assignment
        TEChunkedMiddleWare(const TEChunkedMiddleWare &);
        TEChunkedMiddleWare &operator=(const TEChunkedMiddleWare &);
    public:
        // Constructor & Destructor
        TEChunkedMiddleWare();
        ~TEChunkedMiddleWare();

        // methods
        RawDataBuffer   get_extracted_chunk();
        void            extract_chunk_size();
        void            extract_chunk();
        void            _extract_body_chunk(const char *body_chunk, size_t len);
        RawDataBuffer   extract_body_chunk(const char *read_bytes, size_t len);
};

#endif
