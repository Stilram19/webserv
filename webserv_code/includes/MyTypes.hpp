/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MyTypes.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: obednaou <obednaou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/02 13:46:58 by codespace         #+#    #+#             */
/*   Updated: 2023/08/12 00:52:41 by obednaou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MY_TYPES_HPP
# define MY_TYPES_HPP

# define HEADER_MAX_BUFFER_SIZE 8192
# define READ_BUFFER_SIZE   1024

# define SUCCESS 0
# define ERROR 1
# define PERMISSION_DENIED 2
# define CGI_TIMEOUT 30

enum e_request_handling_step
{
    HEADER_READING = 0, BODY_READING
};

// Stations where the response might call the handler more than one time
enum e_response_handling_stations
{
    MAIN_PROCESSING = 0, WAITING_FOR_CGI, RESPONSE_SENDING
};

enum e_status
{
    WORKING = 0, NORMAL_TERM, BAD_TERM
};

enum e_temporary_storage_type
{
    RAM_BUFFER = 0, FILE
};

enum e_status_code
{
    CLIENT_DISCONNECT = 0, // this is not an http status code (exception)
    OK = 200,
    CREATED = 201,
    MOVED_PERMANENTLY = 301,
    BAD_REQUEST = 400,
    NOT_IMPLEMENTED = 501,
    HTTP_VERSION_NOT_SUPPORTED = 505,
    REQUEST_HEADER_FIELDS_TOO_LARGE = 431, // This status code is added later at the 6585 RFC
    METHOD_NOT_ALLOWED = 405,
    REQUEST_ENTITY_TOO_LARGE = 413,
    REQUEST_URI_TOO_LONG = 414,
    INTERNAL_SERVER_ERROR = 500,
    NOT_FOUND = 404,
    FORBIDDEN = 403,
    CONFLICT = 409,
    NO_CONTENT = 204,
    GATEWAY_TIMEOUT = 504,
    BAD_GATEWAY = 502
};

# define ANSI_COLOR_RESET   "\x1b[0m"
# define ANSI_COLOR_BLACK   "\x1b[30m"
# define ANSI_COLOR_RED     "\x1b[31m"
# define ANSI_COLOR_GREEN   "\x1b[32m"
# define ANSI_COLOR_YELLOW  "\x1b[33m"
# define ANSI_COLOR_BLUE    "\x1b[34m"
# define ANSI_COLOR_MAGENTA "\x1b[35m"
# define ANSI_COLOR_CYAN    "\x1b[36m"
# define ANSI_COLOR_WHITE   "\x1b[37m"

# endif
