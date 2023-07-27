/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   GlobalHeader.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: obednaou <obednaou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/16 16:06:36 by obednaou          #+#    #+#             */
/*   Updated: 2023/07/27 18:34:34 by obednaou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GLOBAL_HEADER_HPP
# define GLOBAL_HEADER_HPP

# define MAX_REQUEST_HEADER 8192
# define READ_BUFFER_SIZE   100

enum e_request_handling_state
{
    HEADER_READING = 0, HEADER_PARSING, BODY_READING
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
