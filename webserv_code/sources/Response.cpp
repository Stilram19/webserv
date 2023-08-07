/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: obednaou <obednaou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/05 19:22:49 by obednaou          #+#    #+#             */
/*   Updated: 2023/08/07 09:53:53 by obednaou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "Response.hpp"

// Constructor & Destructor
Response::Response(Request *request) : _request(request)
{
    _VServer = request->get_server();
    _location = request->get_location();

    // Constructing the request resource full path
    const std::string &location_root = _location->get_root_path();
    const std::string &uri_resource_path = request->get_uri_resource_path();\

    _request_resource_full_path = location_root + uri_resource_path;
}

Response::~Response()
{

}

// Helpers
bool    Response::is_request_resource_found() const
{
    if (FileHandler::is_file(_request_resource_full_path.c_str()))
        return (true);
    if (FileHandler::is_directory(_request_resource_full_path.c_str()))
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

// Main Function
void    Response::Respond()
{
    // Checking if the request exited with an error status code
    // if the request resource not found ==> 
}
