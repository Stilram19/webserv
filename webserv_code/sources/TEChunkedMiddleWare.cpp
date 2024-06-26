/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TEChunkedMiddleWare.cpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: obednaou <obednaou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/01 11:22:19 by codespace         #+#    #+#             */
/*   Updated: 2023/08/05 18:50:18 by obednaou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "TEChunkedMiddleWare.hpp"

// Constructor & Destructor
TEChunkedMiddleWare::TEChunkedMiddleWare() : no_chunk_currently(true), end_of_body(false), curr_chunk_size(0) {}

TEChunkedMiddleWare::~TEChunkedMiddleWare() {}

// Methods
void    TEChunkedMiddleWare::extract_chunk_size()
{
    size_t end = iner_buffer.find("\r\n");
    RawDataBuffer chunk_size_str = iner_buffer.substr(0, end);

    curr_chunk_size = ParsingHelpers::hexa_to_decimal(chunk_size_str.c_str());
    std::cout << "CURRENT_CHUNK_SIZE: " << curr_chunk_size << std::endl;
    end_of_body = (curr_chunk_size == 0);
    no_chunk_currently = false;

    // skipping the size info.
    iner_buffer.right_shift(end + 2);
}

void    TEChunkedMiddleWare::extract_chunk()
{
    RawDataBuffer temp = iner_buffer.substr(0, curr_chunk_size);
    extracted_chunk.append(temp.c_str(), temp.length());
    // std::cout << "CURRENT CHUNK SIZE: " << curr_chunk_size << "| EXTRACTED CHUNK LENGTH: " << extracted_chunk.length() << std::endl;
    curr_chunk_size -= temp.length();

    iner_buffer.right_shift(temp.length());

    if (!curr_chunk_size)
    {
        // Marking the end of the chunk.
        no_chunk_currently = true;
    }
}

void TEChunkedMiddleWare::_extract_body_chunk(const char *body_packet, size_t len)
{
    iner_buffer.append(body_packet, len);
    if (iner_buffer.empty())
        return ;

    // Extracting the size of the next body chunk, if not already extracted.
    if (no_chunk_currently)
    {
        // skipping the "\r\n" that marks the end of the previous chunk, if any.
        if (iner_buffer.find("\r\n") == 0)
            iner_buffer.right_shift(2);

        // looking for "\r\n" that marks the end of the size of the incoming chunk
        if (iner_buffer.find("\r\n") == std::string::npos)
            return ;
        extract_chunk_size();
        // if 0 is read as chunk size, it means the end of the body
        if (end_of_body)
            return ;
    }
    extract_chunk();

    _extract_body_chunk("", 0);
}

// Main Method
RawDataBuffer TEChunkedMiddleWare::extract_body_chunk(const char *body_packet, size_t len)
{
    std::cout << "EXTRACTING...";
    _extract_body_chunk(body_packet, len);
    return (get_extracted_chunk());
}

// Getters
RawDataBuffer TEChunkedMiddleWare::get_extracted_chunk()
{
    RawDataBuffer temp = extracted_chunk;

    // Clearing the chunk buffer for next time.
    extracted_chunk.clear();
    return (temp);
}

bool    TEChunkedMiddleWare::is_body_done()
{
    return (end_of_body);
}
