/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MyTypes.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/02 13:46:58 by codespace         #+#    #+#             */
/*   Updated: 2023/08/03 22:42:39 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MY_TYPES_HPP
# define MY_TYPES_HPP

# define REQUEST_HEADER_BUFFER_SIZE 8192
# define READ_BUFFER_SIZE   1024

enum e_request_handling_step
{
    HEADER_READING = 0, BODY_READING
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
    REQUEST_URI_TOO_LONG = 414,
    INTERNAL_SERVER_ERROR = 500
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
