/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   GlobalHeader.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: obednaou <obednaou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/16 16:06:36 by obednaou          #+#    #+#             */
/*   Updated: 2023/07/30 16:46:23 by obednaou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GLOBAL_HEADER_HPP
# define GLOBAL_HEADER_HPP

# define REQUEST_HEADER_BUFFER_SIZE 8192
# define READ_BUFFER_SIZE   100

enum e_request_handling_step
{
    HEADER_READING = 0, HEADER_PARSING, BODY_READING
};

enum e_request_status
{
    WORKING = 0, NORMAL_TERM, BAD_TERM
};

enum e_request_errors
{
    CLIENT_DISCONNECT = 0,
    BAD_REQUEST = 400,
    NOT_IMPLEMENTED = 501,
    HTTP_VERSION_NOT_SUPPORTED = 505,
    REQUEST_HEADER_FIELDS_TOO_LARGE = 431, // This status code is added later at the 6585 RFC
    METHOD_NOT_ALLOWED = 405,
    REQUEST_ENTITY_TOO_LARGE = 413,
    REQUEST_URI_TOO_LONG = 414
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
