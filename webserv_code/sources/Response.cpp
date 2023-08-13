/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: obednaou <obednaou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/05 19:22:49 by obednaou          #+#    #+#             */
/*   Updated: 2023/08/13 12:15:05 by obednaou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "Response.hpp"

// Constructor & Destructor
Response::Response(int client_socket, Request *request) : _is_there_index(false),  _cgi_flag(false), _is_response_header_sent(false), _status(WORKING), \
    _temporary_storage_type(FILE_BUFFER), _handling_station(MAIN_PROCESSING), _cgi_pid(0), _body_fd(-1), _status_code(OK), _cgi_start_time(0), _request(request)
{
    _client_socket = client_socket;
    _VServer = request->get_server();
    _location = request->get_location();
    _request_method = request->get_request_method();
    _resource_physical_path = request->get_physical_resource_path();
    _resource_logical_path = request->get_logical_resource_path();
    std::cout << "Resource Physical Path: " << _resource_physical_path << std::endl;
    std::cout << "Resource Logical Path: " << _resource_logical_path << std::endl;
    _request_body_file_path = request->get_body_file_path();

    // Mapping the response handling stations to their corresponding methods
    _stations[MAIN_PROCESSING] = &Response::main_processing;
    _stations[WAITING_FOR_CGI] = &Response::wait_for_cgi;
    _stations[RESPONSE_SENDING] = &Response::response_sending;

    // Mapping http methods to their corresponding handlers
    _methods_handlers["GET"] = &Response::get_handler;
    _methods_handlers["POST"] = &Response::post_handler;
    _methods_handlers["DELETE"] = &Response::delete_handler;

    // Mapping each status code to its default html page
    _status_code_pages[OK] = "<html><head><title>200</title></head><body><h1>200 OK</h1></body></html>";
    _status_code_pages[CREATED] = "<html><head><title>201</title></head><body><h1>201 Created</h1></body></html>";
    _status_code_pages[MOVED_PERMANENTLY] = "<html><head><title>301</title></head><body><h1>301 Moved Permanently</h1></body></html>";
    _status_code_pages[BAD_REQUEST] = "<html><head><title>400</title></head><body><h1>400 Bad Request</h1></body></html>";
    _status_code_pages[NOT_IMPLEMENTED] = "<html><head><title>501</title></head><body><h1>501 Not Implemented</h1></body></html>";
    _status_code_pages[HTTP_VERSION_NOT_SUPPORTED] = "<html><head><title>505</title></head><body><h1>505 HTTP Version Not Supported</h1></body></html>";
    _status_code_pages[REQUEST_HEADER_FIELDS_TOO_LARGE] = "<html><head><title>431</title></head><body><h1>431 Request Header Fields Too Large</h1></body></html>";
    _status_code_pages[METHOD_NOT_ALLOWED] = "<html><head><title>405</title></head><body><h1>405 Method Not Allowed</h1></body></html>";
    _status_code_pages[REQUEST_ENTITY_TOO_LARGE] = "<html><head><title>413</title></head><body><h1>413 Request Entity Too Large</h1></body></html>";
    _status_code_pages[REQUEST_URI_TOO_LONG] = "<html><head><title>414</title></head><body><h1>414 Request-URI Too Long</h1></body></html>";
    _status_code_pages[INTERNAL_SERVER_ERROR] = "<html><head><title>500</title></head><body><h1>500 Internal Server Error</h1></body></html>";
    _status_code_pages[NOT_FOUND] = "<html><head><title>404</title></head><body><h1>404 Not Found</h1></body></html>";
    _status_code_pages[FORBIDDEN] = "<html><head><title>403</title></head><body><h1>403 Forbidden</h1></body></html>";
    _status_code_pages[CONFLICT] = "<html><head><title>409</title></head><body><h1>409 Conflict</h1></body></html>";
    _status_code_pages[NO_CONTENT] = "<html><head><title>204</title></head><body><h1>204 No Content</h1></body></html>";
    _status_code_pages[GATEWAY_TIMEOUT] = "<html><head><title>504</title></head><body><h1>504 Gateway Timeout</h1></body></html>";
    _status_code_pages[BAD_GATEWAY] = "<html><head><title>502</title></head><body><h1>502 Bad Gateway</h1></body></html>";

    // Mapping each status code to the correspondant message
    _status_code_messages[OK] = "200 OK";
    _status_code_messages[CREATED] = "201 Created";
    _status_code_messages[MOVED_PERMANENTLY] = "301 Moved Permanently";
    _status_code_messages[BAD_REQUEST] = "400 Bad Request";
    _status_code_messages[NOT_IMPLEMENTED] = "501 Not Implemented";
    _status_code_messages[HTTP_VERSION_NOT_SUPPORTED] = "505 HTTP Version Not Supported";
    _status_code_messages[REQUEST_HEADER_FIELDS_TOO_LARGE] = "431 Request Header Fields Too Large";
    _status_code_messages[METHOD_NOT_ALLOWED] = "405 Method Not Allowed";
    _status_code_messages[REQUEST_ENTITY_TOO_LARGE] = "413 Request Entity Too Large";
    _status_code_messages[REQUEST_URI_TOO_LONG] = "414 Request-URI Too Long";
    _status_code_messages[INTERNAL_SERVER_ERROR] = "500 Internal Server Error";
    _status_code_messages[NOT_FOUND] = "404 Not Found";
    _status_code_messages[FORBIDDEN] = "403 Forbidden";
    _status_code_messages[CONFLICT] = "409 Conflict";
    _status_code_messages[NO_CONTENT] = "204 No Content";
    _status_code_messages[GATEWAY_TIMEOUT] = "504 Gateway Timeout";
    _status_code_messages[BAD_GATEWAY] = "502 Bad Gateway";

    // Mapping each file extension to the correspondant content type
    _content_types[".txt"] = "text/plain";
    _content_types[".html"] = "text/html";
    _content_types[".htm"] = "text/html";
    _content_types[".json"] = "application/json";
    _content_types[".xml"] = "application/xml";
    _content_types[".jpg"] = "image/jpeg";
    _content_types[".jpeg"] = "image/jpeg";
    _content_types[".png"] = "image/png";
    _content_types[".pdf"] = "application/pdf";
    _content_types[".doc"] = "application/msword";
    _content_types[".xls"] = "application/vnd.ms-excel";
    _content_types[".mp3"] = "audio/mpeg";
    _content_types[".mp4"] = "video/mp4";
    _content_types[".zip"] = "application/zip";
    _content_types[".css"] = "text/css";
    _content_types[".js"] = "application/javascript";
    _content_types[".mpeg"] = "video/mpeg";
    _content_types[".mpg"] = "video/mpeg";
    _content_types[".xhtml"] = "application/xhtml+xml";
    _content_types[".ogg"] = "audio/ogg";
    _content_types[".webm"] = "video/webm";
    _content_types[".dtd"] = "application/xml-dtd";
    _content_types[".svg"] = "image/svg+xml";
    _content_types[".gz"] = "application/x-gzip";
    _content_types[".wav"] = "audio/wav";
    _content_types[".gif"] = "image/gif";
}

Response::~Response()
{
    if (_body_fd != -1)
        close(_body_fd);

    // in case of cgi, removing the response body file name (randomly generated in /tmp)
    if (_cgi_flag)
        unlink(_response_body_file_name.c_str());
}

// ******************* HELPERS *******************

bool    Response::is_there_index_file() const
{
    return (_is_there_index);
}

bool    Response::is_directory_listing_on() const
{
    return (_location->get_directory_listing());
}

void    Response::extracting_index_file()
{
    std::string _index_file_name = _location->get_index_file(_resource_physical_path);

    // return if no index file was found
    if (_index_file_name.empty())
        return ;

    // setting the index physical path.
    _index_physical_path = _resource_physical_path + _index_file_name;
    _index_logical_path = _resource_logical_path + _index_file_name;

    // std::cout << "INDEX PH" << _index_physical_path << std::endl;
    // std::cout << "INDEX LO" << _index_logical_path << std::endl;

    _is_there_index = true;
}

void    Response::redirect_cgi_input() const
{
    int fd = open(_request_body_file_path.c_str(), O_RDONLY);

    if (fd == -1)
        throw INTERNAL_SERVER_ERROR;

    // redirecting the input to the request body file.
    if (dup2(fd, 0) == -1)
        throw INTERNAL_SERVER_ERROR;
}

void    Response::redirect_cgi_output()
{
    int fd = open(_response_body_file_name.c_str(), O_CREAT | O_WRONLY, 0666);

    if (fd == -1)
        throw INTERNAL_SERVER_ERROR;

    // redirecting the output to the randomly generated file.
    if (dup2(fd, 1) == -1)
        throw INTERNAL_SERVER_ERROR;
}

const std::string   &Response::get_script_name() const
{
    if (is_there_index_file())
        return (_index_logical_path);
    return (_request->get_logical_resource_path());
}

const std::string   &Response::get_path_info() const
{
    if (is_there_index_file())
        return (_index_logical_path);
    return (_request->get_logical_resource_path());
}

const std::string   &Response::get_script_filename() const
{
    if (is_there_index_file())
        return (_index_physical_path);
    return (_request->get_physical_resource_path());
}

void    Response::cgi_environment_setup()
{
    // manual variable setting
    _cgi_env_vector.push_back("REQUEST_METHOD=" + _request_method);
    _cgi_env_vector.push_back("SERVER_PROTOCOL=" + _request->get_protocol_version());
    _cgi_env_vector.push_back("QUERY_STRING=" + _request->get_query_string());
    _cgi_env_vector.push_back("SCRIPT_NAME=" + get_script_name());
    _cgi_env_vector.push_back("PATH_INFO=" + get_path_info());
    _cgi_env_vector.push_back("SCRIPT_FILENAME=" + get_script_filename());
    _cgi_env_vector.push_back("REDIRECT_STATUS=200");
    _cgi_env_vector.push_back("REQUEST_URI=" + _request->get_request_uri());

    // Setting the request headers in the environment
    const std::map<std::string, std::vector<std::string> >    &_request_headers = _request->get_headers();

    for (std::map<std::string, std::vector<std::string> >::const_iterator it = _request_headers.begin(); it != _request_headers.end(); it++)
    {
        std::string header_key = it->first;
        std::string header_value;

        // converting the header to uppercase & replacing '-' with '_'
        ParsingHelpers::str_to_upper(header_key);
        ParsingHelpers::str_tr(header_key, '-', '_');
        header_key += '=';

        if (it->first == "Content-Length" || it->first == "Content-Type")
        {
            _cgi_env_vector.push_back(header_key + it->second.back());
            continue ;
        }

        // adding the HTTP_ prefix
        header_key = "HTTP_" + header_key;

        std::cout << "KEYYYYYY: " << header_key << std::endl;

        // extracting the value
        for (std::vector<std::string>::const_iterator it1 = it->second.begin(); it1 != it->second.end(); it1++)
        {
            header_value += (*it1);
            if (it1 + 1 != it->second.end())
                header_value += ',';
        }

        // setting the variable into the map
        _cgi_env_vector.push_back(header_key + header_value);
    }
}

void    Response::regular_file_handler(const std::string &regular_file)
{
    // Checking if the regular file has read permission (GET, POST)

    if (access(regular_file.c_str(), F_OK | R_OK))
        throw FORBIDDEN;

    // Checking if there is a cgi handler for the regular file

    std::string file_extension = ParsingHelpers::get_file_extension(regular_file);
    std::string cgi_handler = _location->get_cgi_handler(file_extension);

    if (!cgi_handler.empty())
    {
        _cgi_flag = true;
        cgi(regular_file, cgi_handler);
        return ;
    }

    // 201 created for POST
    if (_request_method == "POST")
    {
        _status_code = CREATED;
        produce_html_for_status_code();
        return ;
    }

    // 200 OK for GET
    _status_code = OK;
    _response_body_file_name = _resource_physical_path;

    if (is_there_index())
        _response_body_file_name = _index_physical_path;

    // the body is all there
    respond();
}

std::string   Response::get_connection() const
{
    if (_request->is_connect_keep_alive())
        return ("keep-alive");
    return ("close");
}

bool    Response::is_status_code_message(const std::string &status_code_message) const
{
    for (std::map<e_status_code, std::string>::const_iterator it = _status_code_messages.begin(); it != _status_code_messages.end(); it++)
    {
        if (status_code_message == it->second)
            return (true);
    }
    return (false);
}

void Response::extract_cgi_response_headers()
{
    // Since the cgi output is all there, we can read the whole header with the max size expected.
    char    buffer[HEADER_MAX_BUFFER_SIZE + 1];

    _body_fd = open(_response_body_file_name.c_str(), O_RDONLY);

    if (_body_fd == -1)
        throw INTERNAL_SERVER_ERROR;

    int read_bytes = read(_body_fd, buffer, HEADER_MAX_BUFFER_SIZE);

    buffer[read_bytes] = '\0';

    if (read_bytes == -1)
        throw INTERNAL_SERVER_ERROR;

    // std::cout << "BUFFER: " << buffer << std::endl;
    std::string _cgi_header_buffer = buffer;

    // if the header is not all there ==> BAD GATEWAY
    size_t pos;

    if ((pos = _cgi_header_buffer.find("\r\n\r\n")) == std::string::npos)
        throw BAD_GATEWAY;
    std::cout << "POSITION: " << pos << std::endl;
    // Header and body separation
    _cgi_header_buffer = _cgi_header_buffer.substr(0, pos + 4);
    lseek(_body_fd, _cgi_header_buffer.length() - read_bytes, SEEK_CUR);

    // cgi headers
    cgi_headers_parsing(_cgi_header_buffer, 0);

    if (_response_headers.find("Content-Type") == _response_headers.end())
        throw BAD_GATEWAY;
}

std::string   Response::get_content_type() const
{
    // checking if the body is html
    if (_temporary_storage_type == RAM_BUFFER)
        return (_content_types.at(".html"));

    std::string extension = ParsingHelpers::get_file_extension(_resource_physical_path);

    // just for adding new content types if not found in the map
    try
    {
        return (_content_types.at(extension));
    }
    catch (std::exception &e)
    {
        std::cout << "Unspecified Content type for " << extension << std::endl;
        return ("");
    }
}

std::string   Response::get_content_length()
{
    int                 c_length = 0;
    std::string         ret;
    std::stringstream   s;

    if (_temporary_storage_type == RAM_BUFFER)
        c_length = _body_buffer.length();
    if (_temporary_storage_type == FILE_BUFFER)
    {
        if (!_cgi_flag && (_body_fd = open(_response_body_file_name.c_str(), O_RDONLY)) == -1)
            throw INTERNAL_SERVER_ERROR;

        int     read_bytes;
        char    buffer[READ_BUFFER_SIZE + 1];

        while ((read_bytes = read(_body_fd, buffer, READ_BUFFER_SIZE)) > 0)
            c_length += read_bytes;
        if (read_bytes == -1)
            throw INTERNAL_SERVER_ERROR;

        lseek(_body_fd, -1 * c_length, SEEK_CUR);
    }
    s << c_length;
    s >> ret;
    return (ret);
}

void    Response::cgi_headers_parsing(const std::string &cgi_header_buffer, size_t start)
{
    bool        term_flag = true;
    size_t      end;
    std::string key;
    std::string value;

    while (cgi_header_buffer[start] && (term_flag = strncmp(cgi_header_buffer.c_str() + start, "\r\n", 2)))
    {
        // Extracting the key and the value
        end = cgi_header_buffer.find(':', start);
        if (end == std::string::npos)
            throw BAD_GATEWAY;
        key = cgi_header_buffer.substr(start, end - start);
        if (key == "Content-type")
            key = "Content-Type";
        if (key == "Content-length")
            key = "Content-Length";
        start = ParsingHelpers::skip_blank(cgi_header_buffer.c_str(), end + 1);
        end = cgi_header_buffer.find("\r\n", start);
        if (end == std::string::npos)
            throw BAD_GATEWAY;
        value = cgi_header_buffer.substr(start, end - start);
        start = end + 2;

        // Checking if the header is too long
        if (key.length() + value.length() >= HEADER_MAX_BUFFER_SIZE)
            throw BAD_GATEWAY;

        // Adding the value to the key values, if the key is not repeated
        if (_response_headers.find(key) != _response_headers.end())
            throw BAD_GATEWAY;

        _response_headers[key] = value;
    }

    // Checking if the response terminates with "\r\n"
    if (term_flag)
        throw BAD_GATEWAY;
}

void    Response::send_ram_buffer(const std::string &buffer)
{
    int sent_bytes = write(_client_socket, buffer.c_str(), buffer.length());

    // Checking if the client disconnected
    if (sent_bytes <= 0)
        throw CLIENT_DISCONNECT;
}

void    Response::send_body_file_chunk()
{
    int     read_bytes, sent_bytes;
    char    body_chunk_buffer[READ_BUFFER_SIZE + 1];

    // reading the body chunk
    read_bytes = read(_body_fd, body_chunk_buffer, READ_BUFFER_SIZE);

    // if (read_bytes == -1)
    //     std::cout << "HEEEEEERE" << std::endl;

    body_chunk_buffer[read_bytes] = '\0';

    if (read_bytes == 0)
    {
        _status = NORMAL_TERM;
        return ;
    }

    // sending the body chunk
    // std::cout << body_chunk_buffer << std::endl;
    sent_bytes = write(_client_socket, body_chunk_buffer, read_bytes);

    if (sent_bytes <= 0)
        throw CLIENT_DISCONNECT;
}

bool    Response::is_there_index() const
{
    return (_is_there_index);
}

// ********************* GETTERS *********************

int Response::get_status() const
{
    return (_status);
}

// ********************* RESPONSE SENDING *********************

void    Response::respond()
{
    // We can finally produce the response header.
    produce_response_header();

    // the next handling station is sending the response.
    _handling_station = RESPONSE_SENDING;
}

void    Response::produce_response_header()
{
    // Response line
    _response_header_buffer = "HTTP/1.1 ";// here
    _response_header_buffer += _status_code_messages[_status_code];
    _response_header_buffer += "\r\n";

    // Extracting the cgi response headers
    if (_cgi_flag && _temporary_storage_type == FILE_BUFFER)
        extract_cgi_response_headers();

    // Setting important headers, after checking if not specified already by the cgi.
    if (_response_headers.find("Content-Type") == _response_headers.end() 
        || _status_code == BAD_GATEWAY || _status_code == GATEWAY_TIMEOUT)
        _response_headers["Content-Type"] = get_content_type();
    if (_response_headers.find("Content-Length") == _response_headers.end()
        || _status_code == BAD_GATEWAY || _status_code == GATEWAY_TIMEOUT)
        _response_headers["Content-Length"] = get_content_length();
    _response_headers["Connection"] = "close";

    // Checking if there is a redirection
    if (_status_code == MOVED_PERMANENTLY)
        _response_headers["Location"] = _redirection;

    // appending the headers to the response header buffer
    std::string key, value;

    for (std::map<std::string, std::string>::const_iterator it = _response_headers.begin(); it != _response_headers.end(); it++)
    {
        key = it->first;
        value = it->second;
        _response_header_buffer += key;
        _response_header_buffer += ": ";
        _response_header_buffer += value;
        _response_header_buffer += "\r\n";
    }
    _response_header_buffer += "\r\n";
}

// ********************* BODY PRODUCERS *********************

void    Response::produce_html_for_status_code()
{
    _temporary_storage_type = RAM_BUFFER;

    std::string html_file_name;

    if (_VServer)
        html_file_name = _VServer->get_error_page(_status_code);

    if (!html_file_name.empty())
    {
        // opening the html file
        std::string     buffer;
        std::ifstream   input_stream(html_file_name.c_str());

        while (std::getline(input_stream, buffer))
            _body_buffer += buffer;

        // closing
        input_stream.close();

        // the body is all there
        respond();
        return ;
    }

    // if the status code doesn't represent an error
    // or no error page extracted from the config file
    // ==> Using the default

    // this try catch is just for adding new status code pages
    try
    {
        _body_buffer = _status_code_pages.at(_status_code);

        // the body is all there
        respond();
    }
    catch (std::exception &e)
    {
        std::cerr << "***********NO DEFAULT PAGE FOR THIS STATUS CODE***********" << std::endl;
        return ;
    }
}

void    Response::produce_html_for_directory_listing()
{
    _temporary_storage_type = RAM_BUFFER;
    _body_buffer = "<html>\n<head>\n<title>Directory Listing </title>\n</head>\n<body>\n<h1>Directory Listing for ";
    _body_buffer += _resource_physical_path;
    _body_buffer += "</h1>\n<ul>\n";

    // Opening the directory and pointing to the stream object, which is pointing on the first entry.
    DIR *dir_stream = opendir(_resource_physical_path.c_str());

    if (dir_stream == NULL)
    {
        std::cerr << "Can't open directory!" << std::endl;
        throw INTERNAL_SERVER_ERROR;
    }

    // Iterating through the entries
    struct dirent *entry;
    std::string entry_full_name;

    while ((entry = readdir(dir_stream)) != NULL)
    {
        if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, ".."))
            continue ;
        _body_buffer += "<li><a href=\"./";
        _body_buffer += entry->d_name;
        _body_buffer += "\">";
        _body_buffer += entry->d_name;
        _body_buffer += "</a></li>\n";
    }

    _body_buffer += "\n</ul>\n</body>\n</html>";

    // Closing the open file (freeing the resources allocated for the stream object)
    closedir(dir_stream);

    // the body is all there
    respond();
}

void    Response::cgi(const std::string &script_path, const std::string &cgi_interpreter)
{
    // Setting the environment variables needed in the cgi
    cgi_environment_setup();

    // using c style for execve
   char **env = new char *[_cgi_env_vector.size() + 1];

    for (size_t i = 0; i < _cgi_env_vector.size(); i++)
        env[i] = (char *)_cgi_env_vector[i].c_str();
    env[_cgi_env_vector.size()] = NULL;

    // setting execve args
    char *args[3];

    args[0] = (char *)cgi_interpreter.c_str();
    args[1] = (char *)script_path.c_str();
    args[2] = NULL;

    // Setting the timer
	struct timeval	t;

	gettimeofday(&t, NULL);
	_cgi_start_time = t.tv_sec;

    // generating a random file name in /tmp for the cgi output
     if (FileHandler::random_file_name_generation(_response_body_file_name, "/tmp"))
        throw INTERNAL_SERVER_ERROR;

    // spowning the cgi process
    _cgi_pid = fork();

    if (_cgi_pid)
        sleep(1);
    std::cout << "PID: " << _cgi_pid << std::endl;

    if (_cgi_pid == -1)
        throw INTERNAL_SERVER_ERROR;

    if (_cgi_pid == 0)
    {
        // redirecting the stdin and stdout of the process executing the cgi
        if (_request_method == "POST")
            redirect_cgi_input();

        redirect_cgi_output();

        // changing the directory to the correct directory (check the subject)
        if (chdir(FileHandler::get_file_root(script_path).c_str()) == -1)
            exit(EXIT_FAILURE);

        // executing the cgi
        execve(args[0], args, env);
        exit(EXIT_FAILURE);
    }

    // releasing the memory in the parent.
    delete [] env;

    // switching the reposonse handling station to 'waiting for cgi'
    _handling_station = WAITING_FOR_CGI;

    wait_for_cgi();
}

// ********************* METHODS HANDLER *********************

void    Response::get_handler()
{
    // (*) regular file handling

    if (FileHandler::is_regular_file(_resource_physical_path.c_str()))
    {
        regular_file_handler(_resource_physical_path);
        return ;
    }

    // (*) directory handling

    // giving the priority to the index file.
    if (is_there_index())
    {
        regular_file_handler(_index_physical_path);
        return ;
    }

    // no index file + directory listing is OFF ==> forbidden
    if (!is_directory_listing_on())
        throw FORBIDDEN;

    // if directory listing ON ==> we should produce the html file for directory listing
    produce_html_for_directory_listing();
}

void    Response::post_handler()
{
    // (*) regular file handling

    if (FileHandler::is_regular_file(_resource_physical_path.c_str()))
    {
        regular_file_handler(_resource_physical_path);
        return ;
    }

    // (*) directory handling

    if (!is_there_index())
        throw FORBIDDEN;

    regular_file_handler(_index_physical_path);
}

void    Response::delete_handler()
{
    // (*) regular file handling

    int delete_status;

    if (FileHandler::is_regular_file(_resource_physical_path.c_str()))
    {
        delete_status = FileHandler::delete_file(_resource_physical_path.c_str());
        if (delete_status)
        {
            if (delete_status == PERMISSION_DENIED)
                throw FORBIDDEN;
            throw INTERNAL_SERVER_ERROR;
        }
        _status_code = NO_CONTENT;
        produce_html_for_status_code();
        return ;
    }

    // (*) Directory handling

    delete_status = FileHandler::delete_directory(_resource_physical_path.c_str());

    if (delete_status)
    {
        if (delete_status == PERMISSION_DENIED)
            throw FORBIDDEN;
        throw INTERNAL_SERVER_ERROR;
    }
    _status_code = NO_CONTENT;
    produce_html_for_status_code();
}

// ********************* RESPONSE STATIONS *********************

void    Response::main_processing()
{
    // Checking if the request failed
    if (_request->get_status() == BAD_TERM)
        throw _request->get_error_type();

    // Checking if the location has a redirection
    _redirection = _location->get_redirect();

    if (!_redirection.empty())
    {
        _status_code = MOVED_PERMANENTLY;
        produce_html_for_status_code();
        return ;
    }

    // preliminary tests if the resource is a directory
    if (FileHandler::is_directory(_resource_physical_path.c_str()))
    {
        // Checking read permission (it's mandatory in all methods)
        if (access(_resource_physical_path.c_str(), R_OK))
            throw FORBIDDEN;

        // Redirecting to the same directory path, terminated by '/'.
        if (_resource_physical_path[_resource_physical_path.length() - 1] != '/')
        {
            _redirection = _resource_logical_path + '/';
            std::cout << "NEW PATH: " << _redirection << std::endl;
            _status_code = MOVED_PERMANENTLY;
            if (_request_method == "DELETE")
                _status_code = CONFLICT;
            produce_html_for_status_code();
            return ;
        }
        // extracting the right index file, if any
        extracting_index_file();
    }

    // Calling the appropriate request method handler
    PtrToMethodHandler method_handler = _methods_handlers[_request_method];

    (this->*method_handler)();
}

void    Response::wait_for_cgi()
{
    // wait for cgi with no hang
    int status;
    int ret = waitpid(_cgi_pid, &status, WNOHANG);

    // if the cgi is still running
    if (ret <= 0)
    {
        struct timeval	t;
        size_t          curr_time;

	    gettimeofday(&t, NULL);
	    curr_time = t.tv_sec;
        if (curr_time - _cgi_start_time >= CGI_TIMEOUT)
        {
            kill(_cgi_pid, SIGTERM);
            throw GATEWAY_TIMEOUT;
        }
        return ;
    }

    // checking the exit status of the cgi.
    if (!WIFEXITED(status) || WEXITSTATUS(status) == EXIT_FAILURE)
        throw INTERNAL_SERVER_ERROR;

    // the cgi is done successfully and the body is all there
    respond();
}

void    Response::response_sending()
{
    // (*) sending the header once
    if (!_is_response_header_sent)
    {
        std::cout << ANSI_COLOR_MAGENTA << "Response Header:" << ANSI_COLOR_RESET << std::endl;
        std::cout << _response_header_buffer << std::endl;
        send_ram_buffer(_response_header_buffer);
        std::cout << "RESPONSE HEADER SENT!" << std::endl;
        _is_response_header_sent = true;
        return ;
    }

    // (*) sending the body

    // send it once, if it's a small html body that is stored in a RAM buffer
    if (_temporary_storage_type == RAM_BUFFER)
    {
        // std::cout << _body_buffer << std::endl;
        send_ram_buffer(_body_buffer);
        std::cout << "HTML BODY SENT!" << std::endl;
        _status = NORMAL_TERM;
        return ;
    }

    // send it chunk by chunk, if it's stored premanently in a file
    send_body_file_chunk();
    std::cout << "BODY CHUNK SENT!" << std::endl;
}

// ********************* MAIN FUNCTION *********************

void    Response::response_handling()
{
    try
    {
        // Calling the current response station method
        PtrToResponseStation station = _stations[_handling_station];

        (this->*station)();
    }
    catch (e_status_code error_type)
    {
        if (error_type == CLIENT_DISCONNECT)
        {
            _status = BAD_TERM;
            return ;
        }
        std::cout << "Caught status code: " << error_type << std::endl;
        _status_code = error_type;
        produce_html_for_status_code();
    }
}
