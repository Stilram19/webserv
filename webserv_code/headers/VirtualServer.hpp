/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   VirtualServer.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: obednaou <obednaou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/16 15:44:47 by obednaou          #+#    #+#             */
/*   Updated: 2023/07/19 11:57:48 by obednaou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef VIRTUAL_SERVER_HPP
# define VIRTUAL_SERVER_HPP

# include "GlobalHeader.hpp"

class Location;

class VirtualServer
{
	private:
		// Types
		typedef void (VirtualServer::*PtrToSetter)(const std::string &);
	private:
		// Attributes
		unsigned int _max_client_body_size;
		std::string _server_name;
		std::string _port_number;
		std::string _host_address;
		PtrToSetter _setters[7];
		std::map<int, std::string> _error_pages_paths;
		std::map<std::string, Location *> _locations;

		// Useless Constructors and copy Assignment
		VirtualServer(const VirtualServer &);
		VirtualServer &operator=(const VirtualServer &);
	public:
		// Constructor
		VirtualServer();

	private:
		// Setters
		void set_max_client_body_size(const std::string &);
		void set_server_name(const std::string &);
		void set_server_port_number(const std::string &);
		void set_server_host_address(const std::string &);
		void set_host_address(const std::string &);
		void set_error_page_path(const std::string &);
		void set_location_info(const std::string &key, const std::string &info);
	public:
		// Setter's Public Function
		void set_server_info(const std::string &info_type, const std::string &info, const std::string &key = "");
};

#endif
