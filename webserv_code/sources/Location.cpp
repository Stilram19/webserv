/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: obednaou <obednaou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/16 17:08:37 by obednaou          #+#    #+#             */
/*   Updated: 2023/07/21 13:06:14 by obednaou         ###   ########.fr       */
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

bool Location::is_http_method(const std::string &input) const
{
	return (input == "GET" || input == "POST" || input == "DELETE");
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
		throw bad_input();
	_directory_listing = true;
	if (input == "OFF")
		_directory_listing = false;
}

void Location::set_redirect_path(const std::string &input)
{
	int mode = F_OK | R_OK | W_OK;

	if (access(input.c_str(), mode)) // check read permission
		throw bad_input();
	_redirect_path = input;
}

void Location::set_root_path(const std::string &input)
{
	int mode = F_OK | R_OK | W_OK;
	if (access(input.c_str(), mode))
		throw bad_input();
	_root_path = input;
}

void Location::set_index_path(const std::string &input)
{
	int mode = F_OK | R_OK | W_OK;

	if (access(input.c_str(), mode))
		throw bad_input();
	_index_path = input;
}

void Location::set_upload_path(const std::string &input)
{
	int mode = F_OK | R_OK | W_OK;

	if (access(input.c_str(), mode))
		throw bad_input();
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
		throw bad_input();

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
			throw bad_input();
		_allowed_http_methods.push_back(method);
		start = ParsingHelpers::skip_spaces(input.c_str(), end);
	}
}
