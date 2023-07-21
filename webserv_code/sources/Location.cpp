/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: obednaou <obednaou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/16 17:08:37 by obednaou          #+#    #+#             */
/*   Updated: 2023/07/21 19:13:05 by obednaou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "Location.hpp"

// ********************* CONSTRUCTOR & DESTRUCTOR *********************

Location::Location()
{
	_directory_listing = false;
	// default redirect path
	// default root path
	// default index path
	// default cgi_handler

	_allowed_http_methods.push_back("GET");

	// Setters
	_setters["directory_listing"] = &Location::set_directory_listing;
	_setters["redirect"] = &Location::set_redirect_path;
	_setters["root"] = &Location::set_root_path;
	_setters["index"] = &Location::set_index_path;
	_setters["upload_post"] = &Location::set_upload_path;
	_setters["cgi"] = &Location::set_cgi_handler;
	_setters["allowed_methods"] = &Location::set_allowed_http_method;
}

Location::~Location() {}

// ********************* HELPERS *********************

// private

bool Location::is_http_method(const std::string &input) const
{
	return (input == "GET" || input == "POST" || input == "DELETE");
}

// public

void Location::display_location_informations() const
{
	std::string directory_listing = "OFF";

	if (get_directory_listing())
		directory_listing = "ON";
	std::cout << "********************************************" << std::endl;
	std::cout << "(*) Directory listing: " << directory_listing << std::endl;
	std::cout << "(*) Redirect path: " << get_redirect_path() << std::endl;
	std::cout << "(*) Root Path: " << get_root_path() << std::endl;
	std::cout << "(*) index path: " << get_index_path() << std::endl;
	std::cout << "(*) upload path: " << get_upload_path() << std::endl;
	std::cout << "(*) cgi handlers: " << std::endl << std::endl;
	std::for_each(_cgi_handler.begin(), _cgi_handler.end(), ParsingHelpers::display_pair<std::string, std::string>);
}

// ********************* SETTERS *********************

// Public Setter Method

void Location::set_location_info(const std::string &info_type, const std::string &info)
{
	for (std::map<std::string, PtrToSetter>::const_iterator it = _setters.begin(); it != _setters.end(); it++)
	{
		if (it->first == info_type)
		{
			(this->*(it->second))(ParsingHelpers::my_trim(info));
			return ;
		}
	}
}

// Private Setters

void Location::set_directory_listing(const std::string &input)
{
	if (input != "ON" && input != "OFF")
		throw std::runtime_error("Directory listing value is either 'ON' or 'OFF' ");
	_directory_listing = true;
	if (input == "OFF")
		_directory_listing = false;
}

void Location::set_redirect_path(const std::string &input)
{
	int mode = F_OK | R_OK | W_OK;

	if (access(input.c_str(), mode)) // check read permission
		throw std::runtime_error("Invalid redirection path!");
	_redirect_path = input;
}

void Location::set_root_path(const std::string &input)
{
	int mode = F_OK | R_OK | W_OK;
	if (access(input.c_str(), mode))
		throw std::runtime_error("Invalid root path!");
	_root_path = input;
}

void Location::set_index_path(const std::string &input)
{
	int mode = F_OK | R_OK | W_OK;

	if (access(input.c_str(), mode))
		throw std::runtime_error("Invalid index path!");
	_index_path = input;
}

void Location::set_upload_path(const std::string &input)
{
	int mode = F_OK | R_OK | W_OK;

	if (access(input.c_str(), mode))
		throw std::runtime_error("Invalid upload path!");
	_upload_path = input;
}

void Location::set_cgi_handler(const std::string &input)
{
	int			start = 0, end = 0;
	std::string extension, cgi_interpret;

	while (!isblank(input[end]))
		end++;
	extension = input.substr(start, end - start);
	start = ParsingHelpers::skip_blank(input.c_str(), end);
	cgi_interpret = input.substr(start);
	if (access(cgi_interpret.c_str(), F_OK))
		throw std::runtime_error("Invalid cgi interpreter!");

	// setting the pair into the map
	_cgi_handler[extension] = cgi_interpret;
}

void Location::set_allowed_http_method(const std::string &input)
{
	int start = 0, end = 0;

	// clearing the default methods (pushed while constructing)
	_allowed_http_methods.clear();

	while (input[start])
	{
		while (!isblank(input[end]))
			end++;
		if (!input[start])
			return ;
		const std::string method = input.substr(start, end - start);

		if (!is_http_method(method))
			throw std::runtime_error("Unkown http method! The Only supported methods are (GET, POST, DELETE)!");
		_allowed_http_methods.push_back(method);
		start = ParsingHelpers::skip_spaces(input.c_str(), end);
	}
}

// ********************* GETTERS *********************

bool				Location::get_directory_listing() const
{
	return (_directory_listing);
}

const std::string	&Location::get_redirect_path() const
{
	return (_redirect_path);
}

const std::string	&Location::get_root_path() const
{
	return (_root_path);
}

const std::string	&Location::get_index_path() const
{
	return (_index_path);
}

const std::string	&Location::get_upload_path() const
{
	return (_upload_path);
}

const std::string	Location::get_cgi_handler(const std::string &extension) const
{
	try
	{
		const std::string cgi_handler = _cgi_handler.at(extension);

		return (cgi_handler);
	}
	catch(const std::exception& e)
	{
		return ("");
	}
}
