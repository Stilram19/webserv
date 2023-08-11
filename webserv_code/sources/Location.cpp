/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: obednaou <obednaou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/16 17:08:37 by obednaou          #+#    #+#             */
/*   Updated: 2023/08/10 23:53:13 by obednaou         ###   ########.fr       */
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

	// Default allowed http method
	_allowed_http_methods.push_back("GET");

	// Setters
	_setters["directory_listing"] = &Location::set_directory_listing;
	_setters["redirect"] = &Location::set_redirect;
	_setters["root"] = &Location::set_root_path;
	_setters["index"] = &Location::set_index;
	_setters["upload_post"] = &Location::set_upload_path;
	_setters["cgi"] = &Location::set_cgi_handler;
	_setters["allowed_methods"] = &Location::set_allowed_http_method;
}

Location::~Location() {}

// ********************* HELPERS *********************

void Location::display_location_informations() const
{
	std::string directory_listing = "OFF";

	if (get_directory_listing())
		directory_listing = "ON";
	std::cout << "********************************************" << std::endl;
	std::cout << "(*) Directory listing: " << directory_listing << std::endl;
	std::cout << "(*) Redirect: " << get_redirect() << std::endl;
	std::cout << "(*) Root Path: " << get_root_path() << std::endl;
	for (std::vector<std::string>::const_iterator it = _indices.begin(); it != _indices.end(); it++)
		std::cout << "(*) index: " << (*it) << std::endl;
	std::cout << "(*) upload path: " << get_upload_path() << std::endl;
	std::cout << "(*) allowed methods: ";
	for (std::vector<std::string>::const_iterator it = _allowed_http_methods.begin(); it != _allowed_http_methods.end(); it++)
	{
		std::cout << (*it);
		if (it + 1 != _allowed_http_methods.end())
			std::cout << ", ";
	}
	std::cout << std::endl;
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

void Location::set_redirect(const std::string &input)
{
	_redirect = input;
}

void Location::set_root_path(const std::string &input)
{
	int mode = F_OK | R_OK | W_OK;
	if (access(input.c_str(), mode))
		throw std::runtime_error("Invalid root path!");
	if (!FileHandler::is_directory(input.c_str()))
		throw std::runtime_error("a Location Root is not a directory!");
	_root_path = input;
}

void Location::set_index(const std::string &input)
{
	_indices.push_back(input);
}

void Location::set_upload_path(const std::string &input)
{
	int mode = F_OK | R_OK | W_OK;

	if (access(input.c_str(), mode))
		throw std::runtime_error("Invalid upload path!");
	if (!FileHandler::is_directory(input.c_str()))
		throw std::runtime_error("the Upload is not a directory!");
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
	if (access(cgi_interpret.c_str(), F_OK | X_OK))
		throw std::runtime_error("Invalid cgi interpreter!");

	// setting the pair into the map
	_cgi_handler[extension] = cgi_interpret;
}

void Location::set_allowed_http_method(const std::string &input)
{
	int start = 0, end = 0;

	// clearing the default methods (pushed while constructing)
	_allowed_http_methods.clear();

	while (input[end])
	{
		while (input[end] && !isblank(input[end]))
			end++;
		const std::string method = input.substr(start, end - start);

		if (!ParsingHelpers::is_http_method(method))
			throw std::runtime_error("Unkown http method! The Only supported methods are (GET, POST, DELETE)!");
		_allowed_http_methods.push_back(method);

		end = ParsingHelpers::skip_spaces(input.c_str(), end);
		start = end;
	}
}

// ********************* GETTERS *********************

bool	Location::get_directory_listing() const
{
	return (_directory_listing);
}

bool	Location::is_http_method_allowed(const std::string &_http_method) const
{
	std::vector<std::string>::const_iterator it = std::find(_allowed_http_methods.begin(), _allowed_http_methods.end(), _http_method);

	return (it != _allowed_http_methods.end());
}

const std::string	&Location::get_redirect() const
{
	return (_redirect);
}

const std::string	&Location::get_root_path() const
{
	return (_root_path);
}

const std::string	&Location::get_upload_path() const
{
	return (_upload_path);
}

std::string	Location::get_cgi_handler(const std::string &extension) const
{
	try
	{
		const std::string cgi_handler = _cgi_handler.at(extension);

		return (cgi_handler);
	}
	catch(std::out_of_range& e)
	{
		return ("");
	}
}

const std::string	&Location::get_index_file(const std::string &root) const
{
	for (std::vector<std::string>::const_iterator it = _indices.begin(); it != _indices.end(); it++)
	{
		std::string index = (*it);

		// if the index is a relative path, join it with the root to have a full path.
		if (index[0] != '/')
			index = root + index;

		// checking if it's a regular file with read access
		if (access(index.c_str(), F_OK | R_OK))
			continue ;
		if (FileHandler::is_regular_file(index.c_str()))
			return (*it);
	}
	// returning an empty string to indicate that no index file was found
	return ("");
}
