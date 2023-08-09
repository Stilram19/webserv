/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: obednaou <obednaou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/05 19:22:49 by obednaou          #+#    #+#             */
/*   Updated: 2023/08/09 19:19:53 by obednaou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "Response.hpp"

// Constructor & Destructor
Response::Response(Request *request) : _is_there_index(false), _status(WORKING), _temporary_storage_type(FILE), _handling_station(MAIN_PROCESSING), _status_code(OK), _request(request)
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
    std::string html_file_name = _location->get_error_page();

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

// ********************* BODY PRODUCERS *********************

void    Response::produce_html_for_status_code()
{
    std::string html_file_name;

    
}

void    Response::produce_html_for_directory_listing()
{
    _response_buffer = "<html>\n<head>\n<title>Directory Listing </title>\n</head>\n<body>\n<h1>Directory Listing for ";
    _response_buffer += _request_resource_path;
    _response_buffer += "</h1>\n<ul>\n";

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
        _response_buffer += "<li><a href=\"";
        _response_buffer += "./";
        _response_buffer += entry->d_name;
        _response_buffer += "\">";
        _response_buffer += entry->d_name;
        _response_buffer += "</a></li>\n";
    }

    _response_buffer += "\n</ul>\n</body>\n</html>";

    // Closing the open file (freeing the resources allocated for the stream object)
    closedir(dir_stream);

    // the body is all there
    respond();
}

void    Response::regular_file_handler(const std::string &regular_file)
{
    // Checking if there is a cgi handler for the regular file

    std::string file_extension = ParsingHelpers::get_file_extension(regular_file);
    std::string cgi_handler = _location->get_cgi_handler(file_extension);

    if (!cgi_handler.empty())
    {
        cgi();
        return ;
    }

    // if no cgi handler exists, the request resource file content is returned.
    _response_body_file_name = _request_resource_path;
    respond();
}

void    Response::cgi()
{
}

// ********************* METHODS HANDLER *********************

void    Response::get_handler()
{
    // (*) handling regular file

    if (FileHandler::is_regular_file(_request_resource_path.c_str()))
    {
        regular_file_handler(_request_resource_path.c_str());
        return ;
    }

    // (*) handling directory

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
    _temporary_storage_type = RAM_BUFFER;
    produce_html_for_directory_listing();
}

void    Response::post_handler()
{
    if (FileHandler::is_directory(_request_resource_path.c_str()))
    {
        if (!is_there_index())
            throw FORBIDDEN;
    }
}

void    Response::delete_handler()
{

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
        // Redirecting with a '/' terminated directory path.
        if (_request_resource_path.back() != '/')
        {
            _redirection = _request_resource_path + '/';
            _status = MOVED_PERMANENTLY;
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
        _temporary_storage_type = RAM_BUFFER;
        _status_code = error_type;
        produce_html_for_status_code();
    }
}
