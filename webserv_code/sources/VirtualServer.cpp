/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   VirtualServer.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: obednaou <obednaou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/16 15:44:49 by obednaou          #+#    #+#             */
/*   Updated: 2023/07/20 20:55:20 by obednaou         ###   ########.fr       */
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
	if (str.empty())
		return (false);
	for (int i = 0; str[i]; i++)
	{
		if (!isdigit(str[i]))
			return (false);
	}
	return (true);
}

bool VirtualServer::is_error_number(int input) const
{
	int error_numbers[7] = {400, 403, 404, 418, 500, 502, 503};

	for (int i = 0; i < 7; i++)
	{
		if (input == error_numbers[i])
			return (true);
	}
	return (false);
}

int VirtualServer::my_stoi(const std::string &str) const
{
	int					ret;
	std::stringstream	s;

	s << str;
	s >> ret;
	return (ret);
}

const std::string VirtualServer::my_trim(const std::string &str) const
{
	int	start, end;

	start = skip_blank(str.c_str(), 0);
	end = start;
	while (!isblank(str[end]))
		end++;
	return (str.substr(start, end - start));
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
	std::string max = "4294967295";

	if (!is_unsigned_int(info))
		throw bad_input("Invalid Max client body size!");
	if (info.length() > max.length() || (info.length() == max.length() && strcmp(max.c_str(), info.c_str()) < 0))
		throw bad_input("Too large value for client body size!");
	_max_client_body_size = my_stoi(info);
}

void VirtualServer::set_server_name(const std::string &info)
{
	_server_name = info;
}

void VirtualServer::set_listen_infos(const std::string &input)
{
	int address_end = input.find(':');

	if (address_end == std::string::npos)
		throw bad_input("':' required to separate the address and the port!");
	_host_address = input.substr(0, address_end);
	_port_number = input.substr(address_end + 1);

	// Checking if the given host_address and the port_number are resolvable
	struct addrinfo hints;
	struct addrinfo *res;

	std::memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	int ret = getaddrinfo(_host_address.c_str(), _port_number.c_str(), &hints, &res);
	freeaddrinfo(res);
	if (ret)
		throw bad_input("Invalid port or host_address (or host_name)!");
}

void VirtualServer::set_error_page_path(const std::string &input)
{
	int start = 0, end = 0;
	int	error_number;
	const char *error_number_message("Invalid Error Number!");
	std::string error_number_str, error_page;

	while (!isblank(input[end]))
		end++;
	error_number_str = input.substr(start, end);
	start = skip_blank(input.c_str(), end);
	error_page = input.substr(start);

	// error number parsing
	if (error_number_str.length() != 3 || !is_unsigned_int(error_number_str))
		throw bad_input(error_number_message);
	error_number = my_stoi(error_number_str);
	if (!is_error_number(error_number))
		throw bad_input(error_number_message);
	// error page parsing

	// setting the pair into the map
	_error_pages[error_number] = error_page;
}
