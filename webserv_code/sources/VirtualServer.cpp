/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   VirtualServer.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: obednaou <obednaou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/16 15:44:49 by obednaou          #+#    #+#             */
/*   Updated: 2023/07/19 21:34:14 by obednaou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "VirtualServer.hpp"

// ******************* CONSTRUCTOR & DESTRUCTOR *******************

VirtualServer::VirtualServer() : _max_client_body_size(10000)
{
	// Setters
	_setters["max_client_body_size"] = VirtualServer::set_max_client_body_size;
	_setters["server_name"] = VirtualServer::set_server_name;
	_setters["listen"] = VirtualServer::set_listen_infos;
	_setters["error_page_path"] = VirtualServer::set_error_page_path;

	// default error pages
}

VirtualServer::~VirtualServer()
{
	for (std::map<std::string, Location *>::const_iterator it = _locations.begin(); it != _locations.end(); it++)
		delete it->second;
}

// ******************* Helpers *******************

Location *VirtualServer::new_location(const std::string &key)
{
	// checking if the key is valid
	Location *n_loc = new Location();

	_locations[key] = n_loc;
	return (n_loc);
}

int VirtualServer::skip_blank(const char *ptr, int start) const
{
	while (isblank(ptr[start]))
		start++;
	return (start);
}

std::string VirtualServer::my_strtrim(const std::string &input) const
{
	int start = skip_blank(value.c_str(), 0);
	int end = input.find('\n', start);
	int	temp = input.find('}', start);

	if (temp < end)
		end = temp;
	return (input.substr(start, end - start));
}

// ******************* Setters *******************

// Setters Main Function

void VirtualServer::set_server_info(const std::string &info_type, const std::string &info, Location *location = NULL)
{
	if (location)
	{
		location->set_location_info(info_type, info);
		return ;
	}
	for (std::map<std::string, PtrToSetter>::const_iterator it = _setters.cbegin(); it != _setters.cend(); it++)
	{
		if (it->first == info_type)
		{
			(this->*(it->second))(info);
			return ;
		}
	}
}

void VirtualServer::set_directory_listing(const std::string &input)
{
	int start = skip_blank(value.c_str(), 0);
	int end = input.find('\n', start);
	int	temp = input.find('}', start);
	std::string value;

	if (temp < end)
		end = temp;
	value = input.substr(start, end - start);
	
}

void VirtualServer::set_redirect_path(const std::string &)
{

}

void VirtualServer::set_root_path(const std::string &)
{

}

void VirtualServer::set_index_path(const std::string &)
{

}

void VirtualServer::cgi_handler(const std::string &, const std::string &)
{

}

void VirtualServer::_allowed_http_method(const std::string &)
{
	
}
