/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   GlobalHeader.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: obednaou <obednaou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/16 16:06:36 by obednaou          #+#    #+#             */
/*   Updated: 2023/07/29 11:40:41 by obednaou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GLOBAL_HEADER_HPP
# define GLOBAL_HEADER_HPP

# define REQUEST_HEADER_BUFFER_SIZE 8192
# define READ_BUFFER_SIZE   100

enum e_request_handling_step
{
    HEADER_READING = 0, HEADER_PARSING, BODY_READING, DONE
};

enum e_request_status
{
    WORKING = 0, NORMAL_TERM, BAD_TERM
};

enum e_http_request_methods
{
    GET = 0, POST, DELETE, UNSUPPORTED_METHOD, REQUEST_URI_TOO_LONG
};

enum e_request_errors
{
    CLIENT_DISCONNECT = 0, BAD_REQUEST, NOT_IMPLEMENTED, HTTP_VERSION_NOT_SUPPORTED, 
};

class WebservCore;

class Client;

class Request;

class ConfigFileParser;

class VirtualServer;

class Location;

# include "include.hpp"

# include "ParsingHelpers.hpp"

# include "ConfigFileParser.hpp"

# include "VirtualServer.hpp"

# include "Location.hpp"

# include "Request.hpp"

# include "Client.hpp"

# include "WebservCore.hpp"

# endif
