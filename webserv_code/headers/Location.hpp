/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: obednaou <obednaou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/16 16:22:34 by obednaou          #+#    #+#             */
<<<<<<< HEAD
/*   Updated: 2023/08/06 15:01:55 by obednaou         ###   ########.fr       */
=======
/*   Updated: 2023/08/08 17:15:44 by obednaou         ###   ########.fr       */
>>>>>>> c3dda2ce8d1438e118cfb560dd70e7e11bb048a4
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATION_HPP
# define LOCATION_HPP

# include "include.hpp"

class Location
{
	private:
		// types
		typedef void (Location::*PtrToSetter)(const std::string &);
	private:
		// Attributes
		bool								_directory_listing;
		std::string 						_redirect;
		std::string 						_root_path;
		std::string 						_index_path;
		std::string							_upload_path;
		std::map<std::string, PtrToSetter> 	_setters;
		std::map<std::string, std::string> 	_cgi_handler;
		std::vector<std::string> 			_allowed_http_methods;

		// Useless Constructors & Copy Assignment
		Location(const Location &);
		Location &operator=(const Location &);

	public:
		// Constructor & Destructor
		Location();
		~Location();

	public:
		// public helpers
		void display_location_informations() const;
	public:
		// Setters Public Method
		void set_location_info(const std::string &info_type, const std::string &info);
	private:
		// Setters
		void	set_directory_listing(const std::string &);
		void	set_redirect(const std::string &);
		void	set_root_path(const std::string &);
		void	set_index_path(const std::string &);
		void	set_upload_path(const std::string &);
		void	set_cgi_handler(const std::string &);
		void	set_allowed_http_method(const std::string &);

	public:
		// Getters
		bool				get_directory_listing() const;
		bool				is_http_method_allowed(const std::string &) const;
		const std::string	&get_redirect() const;
		const std::string	&get_root_path() const;
		const std::string	&get_index_path() const;
		const std::string	&get_upload_path() const;
		std::string			get_cgi_handler(const std::string &extension) const;
};

#endif
