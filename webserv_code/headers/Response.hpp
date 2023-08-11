/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: obednaou <obednaou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/05 19:23:05 by obednaou          #+#    #+#             */
/*   Updated: 2023/08/11 03:37:36 by obednaou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include "include.hpp"

// (*) the request has found an error ==> send the correspondant html page to the error.

// (*) if the method is GET:
    // (is_request_resource_found == false) ==> 404 NOT FOUND
    // (request_type == DIRECTORY):
        // (is_there_index_file == true):
            // (is_there_cgi_handler == true) ==> RUN CGI FOR GET MEtHOD (Status code depends on cgi)
            // false ==> return requested file, 200 OK.
        // (get_directory_listing == ON) ==> list the directory content, 200 OK.
        // OFF ==> 403 forbidden.
    // (request_type == FILE)
        // (is_there_cgi_handler == true) ==> RUN CGI FOR GET MEtHOD (Status code depends on cgi)
        // false ==> return requested file, 200 OK.

// (*) if the method is POST:

// (*) if the method is DELETE:

class Request;

class Response
{
    public:
        // Types
        typedef void (Response::*PtrToResponseStation)();
        typedef void (Response::*PtrToMethodHandler)();
    private:
        // Attributes
        bool                                        _is_there_index;
        bool                                        _cgi_flag;
        int                                         _status;
        int                                         _temporary_storage_type;
        int                                         _handling_station;
        e_status_code                               _status_code;
        size_t                                      _cgi_start_time;
        Request                                     *_request;
        VirtualServer                               *_VServer;
        Location                                    *_location;
        std::string                                 _request_method;
        std::string                                 _request_resource_path;
        std::string                                 _response_body_file_name;
        std::string                                 _index_file;
        std::string                                 _header_buffer;
        std::string                                 _body_buffer;
        std::string                                 _redirection;
        std::string                                 _request_body_file_path;
        std::map<std::string>                       _cgi_env_vector;
        std::map<int, PtrToResponseStation>         _stations;
        std::map<std::string, PtrToMethodHandler>   _methods_handlers;
        std::map<e_status_code, std::string>        _status_code_pages;

        // Useless Constructors & Copy Assignment
        Response();
        Response(const Response &);
        Response &operator=(const Response &);
    public:
        // Constructor & Destructor
        Response(Request *request);
        ~Response();

    private:
        // Helpers
        bool                is_there_index_file() const;
        bool                is_directory_listing_on() const;
        const std::string   &get_request_method() const;
        void                extracting_index_file();
        void                redirect_cgi_input() const;
        void                redirect_cgi_output() const;
        void                regular_file_handler(const std::string &regular_file);
        void                cgi_environment_setup(const std::string &script_path);

    private:
        // Getters
        int     get_status() const;
        bool    is_there_index() const;

    private:
        // Response Construction
        void    respond();
        void    produce_response_header();

    private:
        // Body Producers
        void    produce_html_for_status_code();
        void    produce_html_for_directory_listing();
        void    cgi();
        
    private:
        // Methods Handlers
        void    get_handler();
        void    post_handler();
        void    delete_handler();

    private:
        // Response stations
        void    main_processing();
        void    response_sending();

    public:
        // Main Function
        void    Response_handling();
};

# include "Request.hpp"

#endif

// Response = Headers + Body
// Headers = status_code + content_length + content_type + cgi headers if exist + ...
// status_code = forest of if else
// content_length = sizeof(Body)
// content_type = typeof(Body)
// cgi_headers = the first part of the output of the cgi
// Body = if (http_method == POST or DELETE) => html file representing the status code
//        if (http_method == GET)
//              if (status_code == ERROR) => html file representing the status code
//              else if (regular file) => the second part of the cgi or the regular file itself
//              else if (directory) => html file representing the content of the directory
// http_method = GET | POST | DELETE



// Response steps: prelimanary processing ==> call the right body producer ==> constructing the header based on the body ==> sending the reponse
