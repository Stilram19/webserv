/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   VirtualServer.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: obednaou <obednaou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/16 15:44:49 by obednaou          #+#    #+#             */
/*   Updated: 2023/07/20 14:05:01 by obednaou         ###   ########.fr       */
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

// public

Location *VirtualServer::new_location(const std::string &key)
{
	// checking if the key is valid
	Location *n_loc = new Location();

	_locations[key] = n_loc;
	return (n_loc);
}

// private

int VirtualServer::skip_blank(const char *ptr, int start) const
{
	while (isblank(ptr[start]))
		start++;
	return (start);
}

bool VirtualServer::is_unsigned_int(const std::string &str) const
{
	int	start = 0;

	if (str.empty() || str[0] == '-')
		return (false);
	if (str[0] == '+')
		start = 1;
	for (int i = start; str[i]; i++)
	{
		if (!isdigit(str[i]))
			return (false);
	}
	return (true);
}

const std::string my_trim(const std::string &str) const
{
	int start = skip_blank(str.c_str(), 0);

	return (str.c_str() + start);
}

// ******************* Setters *******************

// Setters Public Function

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
			(this->*(it->second))(my_trim(info));
			return ;
		}
	}
}

// Private Setters

void VirtualServer::set_max_client_body_size(const std::string &info)
{
	if (!is_unsigned_int(info))
		throw bad_input("Invalid Max client body size!");
	std::stringstream s;

	info >> s;
	s >> _set_max_client_body_size;
	// check if the value is within the range

}

void VirtualServer::set_server_name(const std::string &info)
{
	// You can parse the name
	_server_name = info;
}

void VirtualServer::set_listen_infos(const std::string &input)
{
	int address_end = input.find(':');

	if (address_end == string::npos)
		throw bad_input("':' required to separate the address and the port!");
	_host_address = input.substr(0, address_end);
	_port_number = input.substr(address_end + 1);
	// check if the values are valid
}

void VirtualServer::set_error_page_path(const std::string &input)
{
	int start = 0;

	
}
