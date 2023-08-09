/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: obednaou <obednaou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/05 19:23:05 by obednaou          #+#    #+#             */
/*   Updated: 2023/08/06 16:08:40 by obednaou         ###   ########.fr       */
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
    private:
        // Attributes
            Request         *_request;
            VirtualServer   *_VServer;
            Location        *_location;
            std::string     _request_resource_full_path;

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
        bool                is_request_resource_found() const;
        bool                is_there_index_file() const;
        bool                is_directory_listing_on() const;
        const std::string   &get_request_method() const;

    public:
        // Main Function
        void    Respond();
};

# include "Request.hpp"

#endif
