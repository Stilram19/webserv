/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: obednaou <obednaou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/05 19:22:49 by obednaou          #+#    #+#             */
/*   Updated: 2023/08/08 19:58:53 by obednaou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "Response.hpp"

// Constructor & Destructor
Response::Response(Request *request) : _status(WORKING), _temporary_storage_type(FILE), _handling_station(MAIN_PROCESSING), _status_code(OK), _request(request)
{
    _VServer = request->get_server();
    _location = request->get_location();
    _request_method = request->get_request_method();
    _request_resource_path = request->get_uri_resource_path();

    // Mapping the response handling states to their corresponding methods
    _stations[MAIN_PROCESSING] = &Response::main_processing;

    // Mapping http methods to their corresponding handlers
    _methods_handlers["GET"] = &Response::get_handler;
    _methods_handlers["POST"] = &Response::post_handler;
    _methods_handlers["DELETE"] = &Response::delete_handler;
}

Response::~Response()
{

}

// ******************* HELPERS *******************

bool    Response::is_request_resource_found() const
{
    if (FileHandler::is_regular_file(_request_resource_path.c_str()))
        return (true);
    if (FileHandler::is_directory(_request_resource_path.c_str()))
        return (true);
    return (false);
}

bool    Response::is_there_index_file() const
{
    const std::string &index_file = _location->get_index_path();

    return (!index_file.empty());
}

bool    Response::is_directory_listing_on() const
{
    return (_location->get_directory_listing());
}

const std::string   &Response::get_request_method() const
{
    return (_request->get_request_method());
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
}

// ********************* BODY PRODUCERS *********************

void    Response::produce_html_for_status_code()
{
    
}

void    Response::produce_html_for_directory_listing()
{
    _response_buffer = "<html>\n<head>\n<title>Directory Listing </title>\n</head>\n<body>\n<h1>Directory Listing for ";
    _response_buffer += _request_resource_path;
    _response_buffer += "</h1>\n<ul>\n";

    // Opening the directory and pointing to the stream object, which is pointing on the first entry.
    DIR *dir_stream = opendir(path);

    if (dir_stream == NULL)
    {
        std::cerr << "Can't open directory!" << std::endl;
        return (false);
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

    // giving the priority to the index file, anyway.
    if (is_there_index_file())
    {
        regular_file_handler(_location->get_index_path());
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

    // Checking if the request resource is found in the filesystem
    if (!is_request_resource_found())
        throw NOT_FOUND;

    // Calling the appropriate request method handler
    PtrToMethodHandler method_handler = _methods_handlers[_request_method];

    (this->*method_handler)();
}

// ********************* MAIN FUNCTION *********************

void    Response::Response_handling()
{
    try
    {
        // the only two exceptions that can be thrown: CLIENT_DISCONNECT | INTERNAL_SERVER_ERROR
        // Calling the current response handler
        PtrToResponseStation station = _stations[_handling_station];

        (this->*station)();
    }
    catch (e_status_code error_type)
    {
        if (!error_type)
            _status = BAD_TERM;
        _temporary_storage_type = RAM_BUFFER;
        _status_code = error_type;
        produce_html_for_status_code();
    }
}
