/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   VirtualServer.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: obednaou <obednaou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/16 15:44:49 by obednaou          #+#    #+#             */
/*   Updated: 2023/07/23 16:48:40 by obednaou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "VirtualServer.hpp"

// ******************* CONSTRUCTOR & DESTRUCTOR *******************

VirtualServer::VirtualServer() : _max_client_body_size(4294967295), _port_number_int(-1), _server_name("default_server_name")
{
	// Setters

	_setters["max_client_body_size"] = &VirtualServer::set_max_client_body_size;
	_setters["server_name"] = &VirtualServer::set_server_name;
	_setters["listen"] = &VirtualServer::set_listen_infos;
	_setters["error_page"] = &VirtualServer::set_error_page_path;

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
	// to handle: the key is empty
	Location *n_loc = new Location();

	_locations[key] = n_loc;
	return (n_loc);
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

void	VirtualServer::display_server_informations() const
{
	int location_no = 1;

	std::cout << "********************************************" << std::endl;
	std::cout << "(*) Max_client_body_size: " << get_max_client_body_size() << std::endl;
	std::cout << "(*) Hostname: " << get_host_address() << std::endl;
	std::cout << "(*) Port Number: " << get_port_number() << std::endl;
	std::cout << "(*) Server Name: " << get_server_name() << std::endl;
	std::cout << "(*) error pages: " << std::endl << std::endl;
	std::for_each(_error_pages.begin(), _error_pages.end(), ParsingHelpers::display_pair<int, std::string>);

	// Server's Locations
	for (std::map<std::string, Location *>::const_iterator it = _locations.begin(); it != _locations.end(); it++)
	{
		std::cout << "Location no " << location_no++ << ": " << it->first << std::endl;
		it->second->display_location_informations();
	}
}

// ******************* Setters *******************

// Setters Public Function

void VirtualServer::set_server_info(const std::string &info_type, const std::string &info, Location *location)
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
			(this->*(it->second))(ParsingHelpers::my_trim(info));
			return ;
		}
	}
}

// Private Setters

void VirtualServer::set_max_client_body_size(const std::string &info)
{
	std::string max = "4294967295";
	std::string info_start = ParsingHelpers::skip_leading_zeros(info);

	if (!ParsingHelpers::is_unsigned_int(info))
		throw std::runtime_error("max client body size must be unsigned integer (all digits, no signs)!");
	if (info_start.length() > max.length() || (info_start.length() == max.length() && strcmp(max.c_str(), info_start.c_str()) < 0))
		throw std::runtime_error("too large input for max client body size!");
	_max_client_body_size = ParsingHelpers::my_stoi(info);
}

void VirtualServer::set_server_name(const std::string &info)
{
	_server_name = info;
}

void VirtualServer::set_listen_infos(const std::string &input)
{
	size_t address_end = input.find(':');

	if (address_end == std::string::npos)
		throw std::runtime_error("The Host address (or name) and the Port Number must be separated by ':' !");
	_host_address = input.substr(0, address_end);
	_port_number = input.substr(address_end + 1);

	if (!ParsingHelpers::is_unsigned_int(_port_number))
		throw std::runtime_error("Invalid port number!");

	// Checking if the given host_address and the port number are resolvable
	struct addrinfo hints;
	struct addrinfo *res;

	std::memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	int ret = getaddrinfo(_host_address.c_str(), _port_number.c_str(), &hints, &res);
	freeaddrinfo(res);
	if (ret)
		throw std::runtime_error("The Host address (or The Host name) or The Port Number isn't resolvable!");
	_port_number_int = ParsingHelpers::my_stoi(_port_number);
}

void VirtualServer::set_error_page_path(const std::string &input)
{
	int start = 0, end = 0;
	int	error_number;
	std::string error_number_str, error_page;

	while (!isblank(input[end]))
		end++;
	error_number_str = input.substr(start, end);
	start = ParsingHelpers::skip_blank(input.c_str(), end);

	error_page = input.substr(start);
	// error number parsing
	if (error_number_str.length() != 3 || !ParsingHelpers::is_unsigned_int(error_number_str))
		throw std::runtime_error("Invalid http error number!");
	error_number = ParsingHelpers::my_stoi(error_number_str);
	if (!is_error_number(error_number))
		throw std::runtime_error("Invalid http error number!");

	if (access(error_page.c_str(), F_OK | W_OK | R_OK))
		throw std::runtime_error("Invalid error page path!");
	// setting the pair into the map
	_error_pages[error_number] = error_page;
}

// ******************* Getters *******************

unsigned int VirtualServer::get_max_client_body_size() const
{
	return (_max_client_body_size);
}

const std::string &VirtualServer::get_server_name() const
{
	return (_server_name);
}

int	VirtualServer::get_port_number() const
{
	return (_port_number_int);
}

const std::string	&VirtualServer::get_port_number_str() const
{
	return (_port_number);
}

const std::string &VirtualServer::get_host_address() const
{
	return (_host_address);
}

const std::string	VirtualServer::get_error_page(int error_number) const
{
	try
	{
		const std::string error_page = _error_pages.at(error_number);

		return (error_page);
	}
	catch (std::out_of_range &e)
	{
		return ("");
	}
}
