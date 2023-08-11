/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: obednaou <obednaou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/05 19:22:49 by obednaou          #+#    #+#             */
/*   Updated: 2023/08/11 03:50:10 by obednaou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "Response.hpp"

// Constructor & Destructor
Response::Response(Request *request) : _is_there_index(false), _cgi_flag(false), _status(WORKING), \
    _temporary_storage_type(FILE), _handling_station(MAIN_PROCESSING), _status_code(OK), _cgi_start_time(0), _request(request)
{
    _VServer = request->get_server();
    _location = request->get_location();
    _request_method = request->get_request_method();
    _request_resource_path = request->get_uri_resource_path();
    _request_body_file_path = request->get_body_file_path();

    // Mapping the response handling states to their corresponding methods
    _stations[MAIN_PROCESSING] = &Response::main_processing;

    // Mapping http methods to their corresponding handlers
    _methods_handlers["GET"] = &Response::get_handler;
    _methods_handlers["POST"] = &Response::post_handler;
    _methods_handlers["DELETE"] = &Response::delete_handler;

    // Mapping each status code to its default html page
    _status_code_pages[OK] = "<html><head><title>200</title></head><body><h1>200 OK</h1></body></html>";
    _status_code_pages[CREATED] = "<html><head><title>201</title></head><body><h1>201 CREATED</h1></body></html>";
    _status_code_pages[MOVED_PERMANENTLY] = "<html><head><title>301</title></head><body><h1>301 moved permanently</h1></body></html>";
    _status_code_pages[BAD_REQUEST] = "<html><head><title>400</title></head><body><h1>400 bad request</h1></body></html>";
    _status_code_pages[NOT_IMPLEMENTED] = "<html><head><title>501</title></head><body><h1>501 not implemented</h1></body></html>";
    _status_code_pages[HTTP_VERSION_NOT_SUPPORTED] = "<html><head><title>505</title></head><body><h1>505 http version not supported</h1></body></html>";
    _status_code_pages[REQUEST_HEADER_FIELDS_TOO_LARGE] = "<html><head><title>431</title></head><body><h1>431 request header fields too large</h1></body></html>";
    _status_code_pages[METHOD_NOT_ALLOWED] = "<html><head><title>405</title></head><body><h1>405 Method not allowed</h1></body></html>";
    _status_code_pages[REQUEST_ENTITY_TOO_LARGE] = "<html><head><title>413</title></head><body><h1>413 request entity too large</h1></body></html>";
    _status_code_pages[REQUEST_URI_TOO_LONG] = "<html><head><title>414</title></head><body><h1>414 request uri too long</h1></body></html>";
    _status_code_pages[INTERNAL_SERVER_ERROR] = "<html><head><title>500</title></head><body><h1>500 internal server error</h1></body></html>";
    _status_code_pages[NOT_FOUND] = "<html><head><title>404</title></head><body><h1>404 not found</h1></body></html>";
    _status_code_pages[FORBIDDEN] = "<html><head><title>403</title></head><body><h1>403 forbidden</h1></body></html>";
    _status_code_pages[CONFLICT] = "<html><head><title>409</title></head><body><h1>409 conflict</h1></body></html>";
    _status_code_pages[NO_CONTENT] = "<html><head><title>204</title></head><body><h1>204 no content</h1></body></html>";
}

Response::~Response()
{

}

// ******************* HELPERS *******************

bool    Response::is_directory_listing_on() const
{
    return (_location->get_directory_listing());
}

const std::string   &Response::get_request_method() const
{
    return (_request->get_request_method());
}

void    Response::extracting_index_file()
{
    _index_file = _location->get_index_file(_request_resource_path);

    // return if no index file was found
    if (_index_file.empty())
        return ;
    _is_there_index = true;
}

void    Response::redirect_cgi_input() const
{
    int fd = open(request_body_file_path.c_str(), O_RDONLY);

    if (fd == -1)
        throw INTERNAL_SERVER_ERROR;

    // redirecting the input to the request body file.
    if (dup2(fd, 0) == -1)
        throw INTERNAL_SERVER_ERROR;
}

void    Response::redirect_cgi_output() const
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
    _cgi_env_vector.push_back("SCRIPT_NAME=" + _request->get_script_name()); // the logical path of the script
    _cgi_env_vector.push_back("PATH_INFO=" + _request->get_path_info());
    _cgi_env_vector.push_back("SCRIPT_FILENAME=" + _request->get_script_fullname()); // the full path of the script in the server filesystem
    _cgi_env_vector.push_back("REDIRECT_STATUS=200");
    _cgi_env_vector.push_back("REQUEST_URI=" + _request->get_script_name());

    // Setting the request headers in the environment
    const std::map<std::string, std::vector<std::string> >    &_request_headers = _request->get_headers();

    for (std::map<std::string, std::vector<std::string> >::const_iterator it = _request_headers.begin(); it != _request_headers.end(); it++)
    {
        std::string header_key = it->first;
        std::string header_value;

        if (header_key == "content-length")
        {
            _cgi_env_vector.push_back("CONTENT_LENGTH=" + _request->get_content_length());
            continue ;
        }

        if (header_key == "content-type")
        {
            _cgi_env_vector.push_back("CONTENT_TYPE=" + _request->get_content_type());
            continue ;
        }

        // converting the header to uppercase & replacing '-' with '_'
        std::str_to_upper(header_key);
        std::str_tr(header_key, '-', '_');

        // finally adding the HTTP_ prefix
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
    _response_body_file_name = _request_resource_path;

    // the body is all there
    respond();
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
    if (_cgi_flag)
    {

    } 
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

    // this try catch is just for debugging.
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
    _body_buffer += _request_resource_path;
    _body_buffer += "</h1>\n<ul>\n";

    // Opening the directory and pointing to the stream object, which is pointing on the first entry.
    DIR *dir_stream = opendir(_request_resource_path.c_str());

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

void    Response::cgi(std::string &script_path, std::string &cgi_interpreter)
{

    // Setting the environment variables needed in the cgi

    cgi_environment_setup(script_path);

    // using c style for execve
    char **env = new char *[_cgi_env_vector.length() + 1];

    for (size_t i = 0; i < _cgi_env_vector.length(); i++)
        env[i] = _cgi_env_vector[i].c_str();
    env[_cgi_env_vector.length()] = NULL;

    // redirecting the stdin and stdout of the process executing the cgi

    if (_request_method == "POST")
        redirect_cgi_input();

    redirect_cgi_output();

    // setting execve args
    char **args = new char *[3];

    args[0] = cgi_interpreter.c_str();
    args[1] = script_path.c_str();
    args[2] = NULL;

    // Setting the timer
	struct timeval	t;

	gettimeofday(&t, NULL);
	_cgi_timer = t.tv_sec * 1000000 + t.tv_usec;

    // spowning the cgi process
    int pid = fork();

    if (pid == -1)
        throw INTERNAL_SERVER_ERROR;

    if (pid == 0)
    {
        // changing the directory to the correct directory (check the subject)
        if (chdir(_request->get_script_directory()) == -1)
            exit(EXIT_FAILURE);
        execve(args[0], args, env);
        exit(EXIT_FAILURE);
    }

    // releasing the memory in the parent.
    delete [] args;
    delete [] env;

    // switching the reposonse handling station to 'waiting for cgi'
    _handling_station = WAITING_FOR_CGI;

    wait_for_cgi(pid);
}

// ********************* METHODS HANDLER *********************

void    Response::get_handler()
{
    // (*) regular file handling

    if (FileHandler::is_regular_file(_request_resource_path.c_str()))
    {
        regular_file_handler(_request_resource_path);
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

    if (FileHandler::is_regular_file(_request_resource_path.c_str()))
    {
        regular_file_handler(_request_resource_path);
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

    if (FileHandler::is_regular_file(_request_resource_path.c_str()))
    {
        delete_status = FileHandler::delete_file(_request_resource_path.c_str());
        if (delete_status)
        {
            if (delete_status == PERMISSION_DENIED)
                throw FORBIDDEN;
            throw INTERNAL_SERVER_ERROR;
        }
        return ;
    }

    // (*) Directory handling

    delete_status = FileHandler::delete_directory(_request_resource_path.c_str());

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
    if (FileHandler::is_directory(_request_resource_path.c_str()))
    {
        // Redirecting to the same directory path, terminated by '/'.
        if (_request_resource_path[_request_resource_path.length() - 1] != '/')
        {
            _redirection = _request_resource_path + '/';
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

void    Response::wait_for_cgi(int pid)
{
    // wait for cgi with no hang
    int status;
    int ret = waitpid(pid, &status, WNOHANG);

    if (ret != -1)
    {
        kill(pid, SIGTERM);
        throw INTERNAL_SERVER_ERROR;
    }

    if (ret == 0)
    {
        struct timeval	t;
        size_t          curr_time;

	    gettimeofday(&t, NULL);
	    curr_time = t.tv_sec * 1000000 + t.tv_usec;
        if (curr_time - _cgi_start_time >= CGI_TIMEOUT)
        {
            kill(pid, SIGTERM);
            throw GATEWAY_TIMEOUT;
        }
        return ;
    }

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
