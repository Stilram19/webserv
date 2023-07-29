/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: obednaou <obednaou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/26 18:25:35 by obednaou          #+#    #+#             */
/*   Updated: 2023/07/29 18:15:03 by obednaou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "Request.hpp"

// **************** Constructor & Destructor ****************

Request::Request(int client_socket, std::string &body_file_name, \
    const std::vector<VirtualServer *> &VServers, VirtualServer * &VServer) \
    : _keep_alive(false), _http_method(UNSUPPORTED_METHOD), _handling_step(HEADER_READING), _status(WORKING), \ 
    _error_type(0), _client_socket(client_socket), _body_file_name(body_file_name) \
    transfer_encoding_chuncked(false), content_length(0), _VServers(VServers), _VServer(VServer)
{
    // Mapping the Request handling states to their correspondant methods
    _handlers[HEADER_READING] = &Request::header_reader;
    _handlers[HEADER_PARSING] = &Request::header_parser;
    _handlers[BODY_READING] = &Request::body_reader;
}

Request::~Request() {}

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

int Request::request_line_parsing()
{
    int         start = 0;
    int         end;
    std::string method;

    end = _header_buffer.find(' ');
    method = _header_buffer.substr(start, end - start);

    // (*) Extracting The request method
    _http_method = get_http_method(method);

    // Checking if my webserv supports it.
    if (_http_method == UNSUPPORTED_METHOD)
        throw NOT_IMPLEMENTED;

    // (*) Extracting The request target
    start = ParsingHelpers::skip_blank(_header_buffer.c_str(), end);
    end = _header_buffer.find(' ', start);
    _request_uri = _header_buffer.substr(start, end - start);
    request_uri_parsing();

    // (*) Extracting The Protocol Version
    start = ParsingHelpers::skip_blank(_header_buffer.c_str(), end);
    end = _header_buffer.find("\r\n", end);
    std::string protocol_version = _header_buffer.substr(start, end - start);

    if (protocol_version != "HTTP/1.1")
        throw HTTP_VERSION_NOT_SUPPORTED;
    return (end + 2);
}

void    Request::headers_parsing(int start)
{
    int end;
    std::string key;
    std::string value;

    while (strncmp(_header_buffer.c_str() + start, "\r\n", 2))
    {
        // Extracting the key and the value
        end = _header_buffer.find(start, ':');
        key = _header_buffer.substr(start, end - start);
        start = ParsingHelpers::skip_blank(_header_buffer.c_str(), end + 1);
        end = _header_buffer.find("\r\n");
        value = _header_buffer.substr(start, end - start);
        start = end + 2;

        // Checking if the header is too long
        if (key.length() + value.length() >= REQUEST_HEADER_BUFFER_SIZE)
            throw REQUEST_HEADER_FIELDS_TOO_LARGE;

        // Adding the value to the key values
        try
        {
            std::vector<std::string> &values_ref = _request_headers.at(key);

            values_ref.push_back(value);
        }
        catch (std::out_of_range &e)
        {
            std::vector<std::string> new_values_v;

            new_values_v.push_back(value);
            _request_headers[key] = new_values_v;
        }
    }

    if (_VServer)
        set_the_virtual_server();
    check_body_headers();
}

int Request::get_http_method(const std::string &method)
{
    int         i = 0;
    const char  *supported_methods[3] = {"GET", "POST", "DELETE"};

    while (supported_methods[i])
    {
        if (method == supported_methods[i])
            break ;
    }
    return (i);
}

void    Request::request_uri_parsing()
{
    if (_request_uri.length() > REQUEST_HEADER_BUFFER_SIZE)
        throw REQUEST_URI_TOO_LONG;

    // Decoding the uri (replace %<hexadecimal_value> by its corresponding character)
    request_uri_decoding();

    // Extracting the uri components
    int start = _request_uri.find('#');
    int end = _request_uri.length();

    if (start != std::string::npos)
    {
        _fragment = _request_uri.substr(start, end - start);
        end = start;
    }
    start = _request_uri.find('?');
    if (start != std::string::npos)
    {
        _query_string = _request_uri.substr(start, end - start);
        end = start;
    }
    _resource_path = _request_uri.substr(0, end);
}

void    Request::request_uri_decoding()
{
    std::string new_request_uri;

    for (int i = 0; _request_uri[i]; i++)
    {
        char new_character = _request_uri[i];
        if (new_character == '\%')
        {
            new_character = \
            ParsingHelpers::decode_percent_encoded_character(_request_uri.c_str() + i + 1);
            i += 2;
        }
        new_request_uri += new_character;
    }
    _request_uri = new_request_uri;
}

void    Request::extracting_body_consumed_bytes()
{
    int start = _header_buffer.find("\r\n\r\n");

    start += 4;
    _body_consumed_bytes = _header_buffer.substr(start);
    _header_buffer[start] = '\0';
}

void		Request::set_the_virtual_server()
{
    std::string virtual_server_name;

    try
    {
        std::vector<VirtualServer *>::const_iterator it;

        virtual_server_name = _request_headers.at("Host").front();
        for (it = _VServers.begin(); it != _VServers.end(); it++)
        {
            if ((*it)->get_server_name() == virtual_server_name)
                break ;
        }
        if (it == _VServers.end())
            throw std::exception();
        _VServer = (*it);
    }
    catch (std::exception &e)
    {
        _VServer = _VServers.front();
    }
}

void    Request::check_body_headers()
{
    std::map<std::string, std::vector<std::string> >::iterator it1, it2;

    it1 = std::find(_request_headers.begin(), _request_headers.end(), "Content-Length");
    it2 = std::find(_request_headers.begin(), _request_headers.end(), "Transfer-Encoding");
    if (it1 == _request_headers.end() && it2 == _request_headers.end())
        throw BAD_REQUEST;
    if (it1 != _request_headers.end() && it2 != _request_headers.end())
        throw BAD_REQUEST;
    if ((it1 != _request_headers.end() || it2 != _request_headers.end())
        && _http_method != POST)
        throw BAD_REQUEST;

    // Extract the headers values
    if (it2 != _request_headers.end())
    {
        // Transfer-Encoding has at most one value
        if (it2->second.size() != 1)
            throw BAD_REQUEST;
        if (it2->second.front() != "chunked")
            throw BAD_REQUEST;
        transfer_encoding_chunked = true;
        return ;
    }
    // Content-Length has at most one value
    if (it1->second.size() != 1)
        throw BAD_REQUEST;
    content_length = ParsingHelpers::my_stoi(it1->second.front());
}

// **************** REQUEST HANDLERS ****************

void    Request::header_reader()
{
    int     i = 0;
    char    temp[READ_BUFFER_SIZE + 1];
    int     read_bytes = read(_client_socket, temp, READ_BUFFER_SIZE);

    if (read_bytes <= 0)
        throw CLIENT_DISCONNECT;
    temp[read_bytes] = '\0';

    if (_header_buffer.empty())
        i = ParsingHelpers::skip_crlf(temp);
    _header_buffer += temp + i;

    // checking if the header reader is done
    if (strstr(temp + i, "\r\n\r\n"))
        _handling_step = HEADER_PARSING;
}

void    Request::header_parser()
{
    int curr_index;

    curr_index = request_line_parsing();

    // if the method is post:
    // ==> Extracting the consumed body bytes (while reading the header), if any.
    // ==> Generating a random file name for the body file.
    if (_http_method == POST)
    {
        extracting_body_consumed_bytes();
        random_file_name_generation(_body_file_name);
    }

    // Parsing the headers (key : value)
    headers_parsing(curr_index);
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
    try
    {
        // Calling the current request handler
        PtrToRequestHandler handler = _handlers[_handling_step];

        (this->*handler)();
    }
    catch (int error_type)
    {
        _status = BAD_TERM;
        _error_type = error_type;
    }
}
