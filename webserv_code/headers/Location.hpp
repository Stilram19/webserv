/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: obednaou <obednaou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/16 16:22:34 by obednaou          #+#    #+#             */
/*   Updated: 2023/07/20 18:58:08 by obednaou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATION_HPP
# define LOCATION_HPP

# include "GlobalHeader.hpp"

class Location
{
	private:
		// types
		typedef void (Location::*PtrToSetter)(const std::string &);
	private:
		// Attributes
		bool								_directory_listing;
		std::string 						_redirect_path;
		std::string 						_root_path;
		std::string 						_index_path;
		std::string							_upload_path;
		std::map<std::string, PtrToSetter> 	_setters;
		std::map<std::string, std::string> 	_cgi_handler;
		std::vector<std::string> 			_allowed_http_methods;

		// Useless Constructors & Copy Assignment
		Location(const Location &);
		Location &operator=(const Location &);

		// Excpetions
		class bad_input : public std::exception
		{
			private:
				// Attributes
				std::string _message;

				// Can't throw without message
				bad_input();
			public:
				bad_input(const char *message)
				{
					_message = "ConfigFileParser: Location Bad Input: ";
					_message += message;
				}

				const char *what() const throw ()
				{
					return (_message.c_str());
				}
		};
	public:
		// Constructor & Destructor
		Location();
		~Location();
	public:
		// Setters Public Method
		void set_location_info(const std::string &info_type, const std::string &info);
	private:
		// Setters
		void set_directory_listing(const std::string &);
		void set_redirect_path(const std::string &);
		void set_root_path(const std::string &);
		void set_index_path(const std::string &);
		void set_upload_path(const std::string &);
		void set_cgi_handler(const std::string &);
		void set_allowed_http_method(const std::string &);
};

#endif
