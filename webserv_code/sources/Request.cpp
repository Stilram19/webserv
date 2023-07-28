/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: obednaou <obednaou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/26 18:25:35 by obednaou          #+#    #+#             */
/*   Updated: 2023/07/28 20:11:02 by obednaou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "Request.hpp"

// **************** Constructor & Destructor ****************

Request::Request(int client_socket, std::string &body_file_name, const std::vector<VirtualServer *> &VServers) \
    : _keep_alive(false), _http_method(UNKNOWN), _handling_step(HEADER_READING), _status(WORKING), _client_socket(client_socket), _body_file_name(body_file_name), _VServers(VServers)
{
    // Mapping the Request handling states to their correspondant methods
    _handlers[HEADER_READING] = &Request::header_reader;
    _handlers[HEADER_PARSING] = &Request::header_parser;
    _handlers[BODY_READING] = &Request::body_reader;
}

Request::~Request()
{

}

// **************** HELPERS ****************

void    Client::random_file_name_generation(std::string &file_name)
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

int Request::skip_crlf(const char *temp)
{
    int i = 0;

    for (i = 0; temp[i]; i += 4)
    {
        if (strncmp(temp + i, "\r\n\r\n", 4))
            break ;
    }
    return (i);
}

int Request::request_line_parsing()
{
    int         index;
    std::string method;

    index = _header_buffer.find(' ');
    method = _header_buffer.substr(0, index);
    if (method != "GET" && method != "POST" && method != "DELETE")
        // error
    
}

// **************** REQUEST HANDLERS ****************

void    Request::header_reader()
{
    int     i = 0;
    char    temp[READ_BUFFER_SIZE + 1];
    int     read_bytes = read(_client_socket, temp, READ_BUFFER_SIZE);

    if (read_bytes <= 0)
    {
        // The Client Disconnected
        return ;
    }
    temp[read_bytes] = '\0';

    if (_header_buffer.empty())
        i = skip_crlf(temp);
    _header_buffer += temp + i;

    // checking if the header reader is done
    if (strstr(temp + i, "\r\n\r\n"))
        _handling_step = HEADER_PARSING;
}

void    Request::header_parser()
{
    int curr_index;

    curr_index = request_line_parsing();
    curr_index = header_parsing(curr_index);
}

void    Request::body_reader()
{

}

// **************** GETTERS ****************

bool Request::get_status() const
{
    return (_status);
}

bool    Request::get_keep_alive() const
{
    return (_keep_alive);
}

// **************** MAIN FUNCTION ****************

void    Request::request_parsing()
{
    // Calling the current request handler
    PtrToRequestHandler handler = _handlers[_handling_step];

    (this->*handler)();
}
