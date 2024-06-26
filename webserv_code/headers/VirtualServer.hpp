/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   VirtualServer.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: obednaou <obednaou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/16 15:44:47 by obednaou          #+#    #+#             */
/*   Updated: 2023/08/12 23:04:07 by obednaou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef VIRTUAL_SERVER_HPP
# define VIRTUAL_SERVER_HPP

class Location;

# include "include.hpp"

class VirtualServer
{
	private:
		// Types
		typedef void (VirtualServer::*PtrToSetter)(const std::string &);
	private:
		// Attributes
		unsigned int	_max_client_body_size;
		int				_port_number_int;
		std::string		_server_name;
		std::string		_port_number;
		std::string 	_host_address;
		std::map<std::string, PtrToSetter> _setters;
		std::map<int, std::string> _error_pages;
		std::map<std::string, Location *> _locations;

		// Useless Constructors and copy Assignment
		VirtualServer(const VirtualServer &);
		VirtualServer &operator=(const VirtualServer &);

	public:
		// Constructor & Destructor
		VirtualServer();
		~VirtualServer();
	public:
		// Public Helpers
		Location	*new_location(const std::string &key);
		void		display_server_informations() const;

	private:
		// Private Helpers
		bool	is_error_number(int input) const;

	public:
		// Setter's Public Methods
		void set_server_info(const std::string &info_type, const std::string &info, Location *location = NULL);

	private:
		// Setters
		void set_max_client_body_size(const std::string &);
		void set_server_name(const std::string &);
		void set_listen_infos(const std::string &);
		void set_error_page_path(const std::string &);

	public:
		// Getters
		unsigned int		get_max_client_body_size() const;
		int					get_port_number() const;
		const std::string	&get_port_number_str() const;
		const std::string	&get_server_name() const;
		const std::string	&get_host_address() const;
		std::string			get_error_page(int error_number) const;
		std::string			get_location_key(const std::string &path) const;
		Location			*get_correspondant_location(const std::string &location_key);
		bool				is_there_an_invalid_location() const;

};

# include "Location.hpp"

#endif
