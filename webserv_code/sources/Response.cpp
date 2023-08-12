/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: obednaou <obednaou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/05 19:22:49 by obednaou          #+#    #+#             */
/*   Updated: 2023/08/12 01:56:39 by obednaou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "Response.hpp"

// Constructor & Destructor
Response::Response(Request *request) : _is_there_index(false), _cgi_flag(false), _status(WORKING), \
    _temporary_storage_type(FILE), _handling_station(MAIN_PROCESSING), _cgi_pid(0), _cgi_fd(-1), _status_code(OK), _cgi_start_time(0), _request(request)
{
    _VServer = request->get_server();
    _location = request->get_location();
    _request_method = request->get_request_method();
    _resource_physical_path = request->get_physical_resource_path();
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

}

// ******************* HELPERS *******************

bool    Response::is_directory_listing_on() const
{
    return (_location->get_directory_listing());
}

void    Response::extracting_index_file()
{
    _index_file = _location->get_index_file(_resource_physical_path);

    // return if no index file was found
    if (_index_file.empty())
        return ;
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
    // Creating a file in /tmp

    if (FileHandler::random_file_name_generation(_response_body_file_name, "/tmp"))
        throw INTERNAL_SERVER_ERROR;

    int fd = open(_response_body_file_name.c_str(), O_WRONLY);

    if (fd == -1)
        throw INTERNAL_SERVER_ERROR;

    // redirecting the output to the randomly generated file.
    if (dup2(fd, 1) == -1)
        throw INTERNAL_SERVER_ERROR;
}

void    Response::cgi_environment_setup(const std::string &script_path)
{
    // manual variable setting
    _cgi_env_vector.push_back("REQUEST_METHOD=" + _request_method);
    _cgi_env_vector.push_back("SERVER_PROTOCOL=" + _request->get_protocol_version());
    _cgi_env_vector.push_back("QUERY_STRING=" + _request->get_query_string());
    _cgi_env_vector.push_back("SCRIPT_NAME=" + _request->get_logical_resource_path());
    _cgi_env_vector.push_back("PATH_INFO=" + _request->get_logical_resource_path());
    _cgi_env_vector.push_back("SCRIPT_FILENAME=" + _request->get_physical_resource_path());
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

        if (it->first == "content-length" || it->first == "content-type")
        {
            _cgi_env_vector.push_back(header_key + it->second.back());
            continue ;
        }

        // adding the HTTP_ prefix
        header_key = "HTTP_" + header_key;

        // extracting the value
        for (std::vector<std::string>::const_iterator it1 = it->second.begin(); it1 != it->second.end(); it++)
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

    // the body is all there
    respond();
}

const std::string   &Response::get_connection() const
{
    if (_request->is_connect_keep_alive())
        return ("keep-alive");
    return ("close");
}

bool    Response::is_status_code_message(const std::string &status_code_message) const
{
    for (std::map<std::string, std::string>::const_iterator it = _status_code_messages.begin(); it != _status_code_messages.end(); it++)
    {
        if (status_code_message == it->second)
            return (true);
    }
    return (false);
}

void    Response::cgi_response_line_parsing(const std::string &cgi_header_buffer)
{
    size_t pos = cgi_header_buffer.find("\r\n");

    if (pos == std::string::npos)
        throw BAD_GATEWAY;
    std::string response_line = cgi_header_buffer.substr(0, pos);

    pos = cgi_header_buffer.find(' ');

    if (pos == std::string::npos)
        throw BAD_GATEWAY;

    std::string protocol_version = response_line.substr(0, pos);

    if (protocol_version != "HTTP/1.1")
        throw BAD_GATEWAY;

    std::string status_code_message = response_line.substr(pos + 1);

    if (!is_status_code_message(status_code_message))
        throw BAD_GATEWAY;

    // After passing the test ==> adding the response line to the response_header_buffer
    _response_header_buffer += response_line + "\r\n";
}

const std::string   &Response::extract_cgi_response_headers() const
{
    char    buffer[HEADER_MAX_BUFFER_SIZE + 1];

    _cgi_fd = open(_response_body_file_name, O_RDONLY);

    if (_cgi_fd)
        throw INTERNAL_SERVER_ERROR;
    int read_bytes = read(_cgi_fd, buffer, HEADER_MAX_BUFFER_SIZE);

    buffer[read_bytes] = '\0';

    if (read_bytes == -1)
        throw INTERNAL_SERVER_ERROR;

    std::string _cgi_header_buffer = buffer;

    // if the header is not all there ==> BAD GATEWAY
    size_t pos;

    if ((pos = _cgi_header_buffer.find("\r\n\r\n") == std::string::npos))
        throw BAD_GATEWAY;

    _cgi_header_buffer = _cgi_header_buffer.substr(0, pos + 4);
    lseek(_cgi_fd, _cgi_header_buffer.length() - read_bytes, SEEK_CUR);

    // response line
    cgi_response_line_parsing(_cgi_header_buffer);

    // cgi headers
}

const std::string   &Response::get_content_type() const
{
    // checking if the body is html
    if (_temporary_storage == RAM_BUFFER)
        return (_content_types[".html"]);

    std::string extension = ParsingHelpers::get_file_extension(_resource_physical_path);

    // just for adding new content types if not found in the map
    try
    {
        return (_content_types.at(extension));
    }
    catch ()
    {
        std::cout << "Unspecified Content type for " << extension << std::endl;
        return ("");
    }
}

const std::string   &Response::get_content_length() const
{
    int                 c_length = 0;
    int                 fd;
    std::string         ret;
    std::stringstream   s;

    if (_temporary_storage == RAM_BUFFER)
        c_length = _body_buffer.length();
    if (_temporary_storage == FILE)
    {
        if (_cgi_flag)
            fd = _cgi_fd;

        if (!_cgi_flag && (fd = open(_response_body_file_name.c_str(), O_RDONLY)) == -1)
            throw INTERNAL_SERVER_ERROR;

        int     read_bytes;
        char    buffer[READ_SIZE_BUFFER + 1];

        while ((read_bytes = read(fd, buffer, READ_SIZE_BUFFER)) > 0)
            c_length += read_bytes;
        if (read_bytes == -1)
            throw INTERNAL_SERVER_ERROR;
        if (_cgi_flag)
            lseek(fd, -1 * c_length, SEEK_CUR);
        else
            close(fd);
    }
    s << c_length;
    s >> ret;
    return (ret);
}

// ********************* GETTERS *********************

int Response::get_status() const
{
    return (_status);
}

bool    Response::is_there_index() const
{
    return (_is_there_index);
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
    // Response Line: http-protocol + status_code_message
    // Headers that exist in every response: content-length, content-type, Server, connection

    // Response line
    if (!_cgi_flag)
    {
        _response_header_buffer = _request->get_protocol_version();
        _response_header_buffer += " ";
        _response_header_buffer += _status_code_messages[_status_code];
        _response_header_buffer += "\r\n";
    }

    // Extracting the cgi response headers
    if (_cgi_flag)
        extract_cgi_response_headers();

    // Setting important headers, after checking if not specified already by the cgi.
    if (_response_headers.find("Content-Type") == _response_headers.end())
        _response_headers["Content-Type"] = get_content_type();
    if (_response_headers.find("Content-Length") == _response_headers.end())
        _response_headers["Content-Length"] = get_content_length();
    if (_response_headers.find("Connection") == _response_headers.end())
        _response_headers["Connection"] = get_connection();

    // Checking if there is a redirection
    if (_status_code == MOVED_PERMANENTLY)
        _response_headers["Location"] = _redirection;
}

// ********************* BODY PRODUCERS *********************

void    Response::produce_html_for_status_code()
{
    _temporary_storage_type = RAM_BUFFER;

    std::string html_file_name = _VServer->get_error_page(_status_code);

    if (!html_file_name.empty())
    {
        // opening the html file
        std::string     buffer;
        std::ifstream   input_stream(html_file_name.c_str());

        if (!input_stream.is_open())
            throw INTERNAL_SERVER_ERROR;

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
        _body_buffer += "<li><a href=\"";
        _body_buffer += "./";
        _body_buffer += entry->d_name;
        if (entry->d_type == DT_DIR)
            _body_buffer += "/";
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
    cgi_environment_setup(script_path);

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

    // spowning the cgi process
    _cgi_pid = fork();

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
    delete [] args;
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
        regular_file_handler(_index_file);
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

    regular_file_handler(_index_file);
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
        _status = MOVED_PERMANENTLY;
        produce_html_for_status_code();
        return ;
    }

    // preliminary tests if the resource is a directory
    if (FileHandler::is_directory(_resource_physical_path.c_str()))
    {
        // Redirecting to the same directory path, terminated by '/'.
        if (_resource_physical_path[_resource_physical_path.length() - 1] != '/')
        {
            _redirection = _resource_physical_path + '/';
            _status = MOVED_PERMANENTLY;
            if (_request_method == "DELETE")
                _status = CONFLICT;
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

    if (ret != -1)
    {
        kill(_cgi_pid, SIGTERM);
        throw INTERNAL_SERVER_ERROR;
    }

    // if the cgi is still running
    if (ret == 0)
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

}

// ********************* MAIN FUNCTION *********************

void    Response::Response_handling()
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
        _status_code = error_type;
        produce_html_for_status_code();
    }
}
