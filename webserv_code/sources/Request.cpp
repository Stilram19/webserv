/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: obednaou <obednaou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/26 18:25:35 by obednaou          #+#    #+#             */
/*   Updated: 2023/07/27 18:36:33 by obednaou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "Request.hpp"

// **************** Constructor & Destructor ****************

Request::Request(int client_socket) : _request_handling_state(HEADER_READING), _client_socket(client_socket)
{
    // generating a random file name for the body of the request, if any
    random_file_name_generation(body_file_name);

    // Mapping the Request handling states to their correspondant methods
    _request_handlers[HEADER_READING] = &Request::header_reader;
    _request_handlers[HEADER_PARSING] = &Request::header_parser;
    _request_handlers[BODY_READING] = &Request::body_reader;
}

Request::~Request()
{

}

// **************** HELPERS ****************

void    Request::random_file_name_generation(std::string &file_name)
{
    int j = 0, read_bytes = 0;
    int fd = open("/dev/random", O_RDONLY);
    char buffer[51];

    file_name = "/tmp/";
    for (int i = 0; i < 14; i++)
    {
        read_bytes = read(fd, buffer, 50);
        for (j = 0; j < read_bytes; j++)
        {
            if (isalnum(buffer[j]))
            {
                file_name += buffer[j];
                break ;
            }
        }
        if (j == read_bytes)
            i--;
    }
    close(fd);
}

// **************** REQUEST HANDLERS ****************

void    Request::header_reader()
{
    char    temp[READ_BUFFER_SIZE + 1];
    int     read_bytes = read(_client_socket, temp, READ_BUFFER_SIZE);

    if (read_bytes <= 0)
    {
        // The Client Disconnected
        return ;
    }
    temp[read_bytes] = '\0';
    header_buffer += temp;

    // checking if the header reader is done
    if (strstr(temp, "\r\n\r\n"))
    {
        _request_handling_state = HEADER_PARSING;
        return ;
    }
}

void    Request::header_parser()
{
    
}

void    Request::body_reader()
{

}

// **************** MAIN FUNCTION ****************

void    Request::request_parsing()
{
    // Calling the current request handler
    PtrToRequestHandler handler = _request_handlers[_request_handling_state];

    (this->*handler)();
}
