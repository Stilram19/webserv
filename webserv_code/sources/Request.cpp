/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: obednaou <obednaou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/26 18:25:35 by obednaou          #+#    #+#             */
/*   Updated: 2023/08/07 12:12:49 by obednaou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "Request.hpp"

// **************** Constructor & Destructor ****************

Request::Request(int client_socket, std::string &body_file_name, \
    const std::vector<VirtualServer *> &VServers) : _body_fd(-1), _body_file_name(body_file_name), _VServers(VServers), _VServer(NULL), _location("", NULL)
{
    // Default values
    _keep_alive = false;
    _handling_step = HEADER_READING;
    _status = WORKING;
    _error_type = -1;
    _client_socket = client_socket;
    header_read_bytes = 0;
    _raw_header_buffer = new RawDataBuffer();
    transfer_encoding_chunked = false;
    content_length = 0;
    body_read_bytes = 0;
    _consumed_body_bytes_size = 0;
    _consumed_body_bytes = NULL;
    te_chunked_middle_ware = NULL;

    // Mapping the Request handling states to their correspondant methods
    _handlers[HEADER_READING] = &Request::header_reader;
    _handlers[BODY_READING] = &Request::body_reader;
}

Request::~Request()
{
    delete _raw_header_buffer;
    delete _consumed_body_bytes;
}

// **************** HELPERS ****************

void    Request::random_file_name_generation(std::string &file_name)
{
    int j = 0, read_bytes = 0;
    int fd = open("/dev/random", O_RDONLY);
    char buffer[51];

    //file_name = "/tmp/";
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

    // creating a file with the generated name
    _body_fd = open(file_name.c_str(), O_CREAT, 0666);

    if (_body_fd == -1)
        throw INTERNAL_SERVER_ERROR;

    // Closing /dev/random fd
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

    // setting the config informations (the virtual server and the location)
    set_config_infos();
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
    size_t start = _raw_header_buffer->find("\r\n\r\n");

    start += 4;
    _consumed_body_bytes = new RawDataBuffer();
    // Extracting the body bytes consumed while reading the header
    _consumed_body_bytes_size = header_read_bytes - start;
    if (!_consumed_body_bytes_size)
        return ;
    *_consumed_body_bytes = _raw_header_buffer->substr(start, _consumed_body_bytes_size);
    // std::cout << "************************* HERE: " << _consumed_body_bytes->c_str() << std::endl;
}

void		Request::set_config_infos()
{
    // Setting the correspondant virtual server
    set_virtual_server();

    // setting the correspondant location
    set_location();
}

void		Request::set_virtual_server()
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

void    Request::set_location()
{
    _location = _VServer->get_correspondant_location(_resource_path);

    std::cout << "RESOURCE PATH: |" << _resource_path << "|" << std::endl;

    // checking if the request source path is not found
    if (_location == NULL)
        throw NOT_FOUND;

    // checking if the http method is allowed in the location
    if (!_location->is_http_method_allowed(_http_method))
        throw METHOD_NOT_ALLOWED;
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

            // Only chunked is implemented, as transfer-encoding
            if (!transfer_encoding_chunked)
                throw NOT_IMPLEMENTED;

            // Creating the middle ware object that will handle transfer-encoding chunked.
            te_chunked_middle_ware = new TEChunkedMiddleWare();
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
    std::cout << ANSI_COLOR_CYAN << "***************** HEADER PARSING INFOS *****************" << ANSI_COLOR_RESET << std::endl;
    std::cout << "Http Method: " << "'" << _http_method << "'" << std::endl;
    std::cout << "Request uri: " << "'" << _request_uri << "'" << std::endl;
    std::cout << "Resource path: " << "'" << _resource_path << "'" << std::endl;
    std::cout << "query string: " << "'" << _query_string << "'" << std::endl;
    std::cout << "fragment: " << "'" << _fragment << "'" << std::endl;
    std::cout << "==> EXTRACTED HEADERS: " << std::endl;

    std::cout << ANSI_COLOR_YELLOW;

    // !Debugging
    _VServer->display_server_informations();
    std::cout << ANSI_COLOR_RESET;
    std::cout << ANSI_COLOR_GREEN;
    _location.second->display_location_informations();

    for (std::map<std::string, std::vector<std::string> >::const_iterator it = _request_headers.begin(); it != _request_headers.end(); it++)
    {
        std::cout << ANSI_COLOR_BLUE << it->first << ": " << ANSI_COLOR_RESET;
        //std::cout << "Values: ";
        for (std::vector<std::string>::const_iterator it1 = it->second.begin(); it1 != it->second.end(); it1++)
        {
            std::cout << *it1;
            if (it1 + 1 != it->second.end())
                std::cout << ", ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

int Request::read_sent_packet(char *buffer)
{
    int     read_bytes = read(_client_socket, buffer, READ_BUFFER_SIZE);

    if (read_bytes <= 0)
        throw CLIENT_DISCONNECT;
    buffer[read_bytes] = '\0';
    return (read_bytes);
}

void    Request::extract_body_chunk(const char *body_packet, size_t read_bytes)
{
    if (!transfer_encoding_chunked)
    {
        // (*) Content length handler
        body_read_bytes += read_bytes;
        std::cout << "BODY_READ_BYTES: " << body_read_bytes << std::endl;
        if (body_read_bytes > content_length)
            throw REQUEST_ENTITY_TOO_LARGE;
        if (body_read_bytes == content_length)
            _status = NORMAL_TERM;
        append_chunk_to_body_file(body_packet, read_bytes);
        return ;
    }

    // (*) transfer-encoding chunked handler
    RawDataBuffer chunk = te_chunked_middle_ware->extract_body_chunk(body_packet, read_bytes);

    if (te_chunked_middle_ware->is_body_done())
    {
        delete te_chunked_middle_ware;
        _status = NORMAL_TERM;
    }
    append_chunk_to_body_file(chunk.c_str(), chunk.length());
}

void    Request::append_chunk_to_body_file(const char *body_chunk, size_t read_bytes)
{

    int written_bytes = write(_body_fd, body_chunk, read_bytes);

    if (written_bytes < 0 || (size_t)written_bytes < read_bytes)
        throw INTERNAL_SERVER_ERROR;
}

void    Request::close_body_file()
{
    if (_body_fd != -1)
        close(_body_fd);
}

// **************** REQUEST HANDLERS ****************

void    Request::header_reader()
{
    int     i = 0;
    size_t  read_bytes;
    char    buffer[READ_BUFFER_SIZE + 1];

    read_bytes = read_sent_packet(buffer);

    if (_raw_header_buffer->empty())
    {
        i = ParsingHelpers::skip_crlf(buffer);
        read_bytes -= i;
    }

    header_read_bytes += read_bytes;
    std::cout << "READ_BYTES: (HEADER READING) '" << header_read_bytes << "'" << std::endl;

    _raw_header_buffer->append(buffer + i, read_bytes);

    size_t end = _raw_header_buffer->find("\r\n\r\n");


    // checking if the header reader is done
    if (end != std::string::npos)
    {
        // Copying the header to the std::string buffer.
        end += 4;
        _header_buffer = _raw_header_buffer->substr(0, end).c_str();

        // parsing the read header.
        header_parser();
        //  debugging
        std::cout << ANSI_COLOR_GREEN << "======> HEADER: ||||||" << ANSI_COLOR_RESET << std::endl;
        std::cout << _header_buffer << "||||||" << std::endl;
        _handling_step = BODY_READING;
    }
}

void    Request::header_parser()
{
    int curr_index;

    curr_index = request_line_parsing();

    // if the method is post:
    // ==> Extracting the consumed body bytes (while reading the header), if any.
    // ==> Generating a random file name for the body file.
    if (_http_method == "POST")
    {
        random_file_name_generation(_body_file_name);
        extracting_body_consumed_bytes();
    }

    // destroying the object pointed to by _raw_header_buffer, and nullifying the pointer
    delete _raw_header_buffer;
    _raw_header_buffer = NULL;

    // Parsing the headers (key : value)
    headers_parsing(curr_index);

    // Extrating usefull headers values
    important_headers_extraction();

    // if the method is post
    // ==> the control of the request is transfered to the body reader
    // else
    // ==> the request is done
    display_request_header_infos();

    if (_http_method == "POST")
    {
        _handling_step = BODY_READING;

        // Checking if we've consumed a body chunk while reading the header
        std::cout << "CONSUMED BODY BYTES: " << _consumed_body_bytes_size <<  std::endl;
        if (_consumed_body_bytes_size)
            extract_body_chunk(_consumed_body_bytes->c_str(), _consumed_body_bytes->length());

        delete _consumed_body_bytes;
        _consumed_body_bytes = NULL;
        return ;
    }
    _status = NORMAL_TERM;
}

void    Request::body_reader()
{
    // Body reader
    std::cout << "BODY READER" << std::endl;
    char    buffer[READ_BUFFER_SIZE + 1];

    int read_bytes = read_sent_packet(buffer);

    // Extracting the body chunk from the sent packet
    extract_body_chunk(buffer, read_bytes);
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

bool    Request::is_connect_keep_alive() const
{
    return (_keep_alive);
}

VirtualServer   *Request::get_server() const
{
    return (_VServer);
}

Location    *Request::get_location() const
{
    return (_location);
}

const std::string 	&Request::get_uri_resource_path() const
{
    return (_resource_path);
}

const std::string	&Request::get_request_method() const
{
    return (_http_method);
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
    catch (e_status_code error_type)
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
