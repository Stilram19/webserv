/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: obednaou <obednaou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/26 18:25:35 by obednaou          #+#    #+#             */
/*   Updated: 2023/07/30 20:45:33 by obednaou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "Request.hpp"

// **************** Constructor & Destructor ****************

Request::Request(int client_socket, std::string &body_file_name, \
    const std::vector<VirtualServer *> &VServers, VirtualServer * &VServer) : _body_file_name(body_file_name), _VServers(VServers), _VServer(VServer)
{
    // Default values
    _keep_alive = false;
    _handling_step = HEADER_READING;
    _status = WORKING;
    _error_type = -1;
    _client_socket = client_socket;
    transfer_encoding_chunked = false;
    content_length = -1;

    // Mapping the Request handling states to their correspondant methods
    _handlers[HEADER_READING] = &Request::header_reader;
    _handlers[HEADER_PARSING] = &Request::header_parser;
    _handlers[BODY_READING] = &Request::body_reader;
}

Request::~Request() {}

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

int Request::request_line_parsing()
{
    int         start = 0;
    int         end;

    end = _header_buffer.find(' ');
    _http_method = _header_buffer.substr(start, end - start);

    // (*) Extracting the Request method

    if (!ParsingHelpers::is_http_method(_http_method))
        throw NOT_IMPLEMENTED;

    // (*) Extracting The request target
    start = ParsingHelpers::skip_blank(_header_buffer.c_str(), end);
    end = _header_buffer.find(' ', start);
    _request_uri = _header_buffer.substr(start, end - start);
    request_uri_parsing();

    // (*) Extracting The Protocol Version
    start = ParsingHelpers::skip_blank(_header_buffer.c_str(), end);
    end = _header_buffer.find("\r\n", start);
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
        end = _header_buffer.find(':', start);
        key = _header_buffer.substr(start, end - start);
        start = ParsingHelpers::skip_blank(_header_buffer.c_str(), end + 1);
        end = _header_buffer.find("\r\n", start);
        value = _header_buffer.substr(start, end - start);
        start = end + 2;

        std::cout << "KEY: <<<<" << key << ">>>>" << std::endl;
        std::cout << "VALUE: <<<<" << value << ">>>>" << std::endl;

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

    // setting the virtual server, if not already set.
    if (!_VServer)
        set_the_virtual_server();
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
    size_t start = _request_uri.find('#');
    size_t end = _request_uri.length();

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

void    Request::extracting_body_headers()
{
    std::map<std::string, std::vector<std::string> >::iterator it1, it2;

    it1 = _request_headers.find("Content-Length");
    it2 = _request_headers.find("Transfer-Encoding");

    if (_http_method == "POST")
    {
        // Exactly one of the "Content-Length, Transfer-Encoding" headers must be there.
        if (it1 == _request_headers.end() && it2 == _request_headers.end())
            throw BAD_REQUEST;
        if (it1 != _request_headers.end() && it2 != _request_headers.end())
            throw BAD_REQUEST;

        // Extracting the values
        if (it2 != _request_headers.end())
        {
            transfer_encoding_chunked = (it2->second.back() == "chunked");
            if (!transfer_encoding_chunked)
                throw NOT_IMPLEMENTED;
            return ;
        }
        content_length = ParsingHelpers::my_stoi(it1->second.back());

        if (content_length > _VServer->get_max_client_body_size())
            throw REQUEST_ENTITY_TOO_LARGE;
        return ;
    }

    // Checking if body related headers exit in the context of a GET or DELETE method.
    if (it1 != _request_headers.end() || it2 != _request_headers.end())
        throw BAD_REQUEST;
}

void    Request::important_headers_extraction()
{
    // Cheking if the http method is allowed
    if (!_VServer->is_http_method_allowed(_http_method))
        throw METHOD_NOT_ALLOWED;

    // Extracting the connection type
    extracting_connection_type();

    // Checking and extracting body related headers.
    extracting_body_headers();
}

void    Request::extracting_connection_type()
{
    std::map<std::string, std::vector<std::string> >::iterator it;
    std::string key("Connection");

    it = _request_headers.find(key);

    // return If no connection header found (connection is set to close by default)
    if (it == _request_headers.end())
        return ;

    // if more than one header found with the "connection" key ==> BAD_REQUEST
    if (it->second.size() != 1)
        throw BAD_REQUEST;

    _keep_alive = (it->second.front() == "keep-alive");
}

void    Request::display_request_header_infos()
{
    std::cout << "***************** HEADER PARSING INFOS *****************" << std::endl;
    std::cout << "Http Method: " << "'" << _http_method << "'" << std::endl;
    std::cout << "Request uri: " << "'" << _request_uri << "'" << std::endl;
    std::cout << "Resource path: " << "'" << _resource_path << "'" << std::endl;
    std::cout << "query string: " << "'" << _query_string << "'" << std::endl;
    std::cout << "fragment: " << "'" << _fragment << "'" << std::endl;
}

// **************** REQUEST HANDLERS ****************

void    Request::header_reader()
{
    std::cout << "#HEADER READER CALLED#" << std::endl;
    int     i = 0;
    char    temp[READ_BUFFER_SIZE + 1];
    int     read_bytes = read(_client_socket, temp, READ_BUFFER_SIZE);

    if (read_bytes <= 0)
        throw CLIENT_DISCONNECT;
    temp[read_bytes] = '\0';

    //std::cout << "SENT_PACKET: <<<<" << temp << ">>>>" << std::endl;

    if (_header_buffer.empty())
        i = ParsingHelpers::skip_crlf(temp);
    _header_buffer += temp + i;

    // checking if the header reader is done
    if (_header_buffer.find("\r\n\r\n") != std::string::npos)
    {
        // parsing the read header.
        header_parser();
        //  debugging
        std::cout << "======> HEADER: " << std::endl;
        std::cout << _header_buffer << std::endl;
        _handling_step = NORMAL_TERM;
    }
}

void    Request::header_parser()
{
    std::cout << "#HEADER PARSER CALLED# " << std::endl;
    int curr_index;

    curr_index = request_line_parsing();

    // if the method is post:
    // ==> Extracting the consumed body bytes (while reading the header), if any.
    // ==> Generating a random file name for the body file.
    if (_http_method == "POST")
    {
        extracting_body_consumed_bytes();
        random_file_name_generation(_body_file_name);
    }

    // Parsing the headers (key : value)
    headers_parsing(curr_index);



    // Extrating usefull headers values
    important_headers_extraction();

    // if the method is post
    // ==> the control of the request is transfered to the body reader
    // else
    // ==> the request is done
    if (_http_method == "POST")
    {
        _handling_step = BODY_READING;
        return ;
    }
    display_request_header_infos();
    _status = NORMAL_TERM;
}

void    Request::body_reader()
{
    // Body reader

    std::cout << "#BODY READER CALLED#" << std::endl;
    _status = NORMAL_TERM;
}

// **************** GETTERS ****************

bool Request::get_status() const
{
    return (_status);
}

int Request::get_error_type() const
{
    return (_error_type);
}

bool    Request::is_connection_to_be_closed() const
{
    return (!_keep_alive);
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
    catch (e_request_errors error_type)
    {
        _status = BAD_TERM;
        _error_type = error_type;
        if (_error_type == CLIENT_DISCONNECT)
        {
            std::cout << "Client disconnected!" << std::endl;
            return ;
        }
        std::cout << error_type << std::endl;
    }
}
