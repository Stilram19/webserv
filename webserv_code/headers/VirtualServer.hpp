/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   VirtualServer.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: obednaou <obednaou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/16 15:44:47 by obednaou          #+#    #+#             */
/*   Updated: 2023/07/18 15:43:04 by obednaou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef VIRTUAL_SERVER_HPP
# define VIRTUAL_SERVER_HPP

# include "GlobalHeader.hpp"

class Location;

class VirtualServer
{
	private:
		// Attributes
		unsigned int _max_client_body_size;
		std::string _server_name;
		std::string _port_number;
		std::string _host_address;
		std::map<int, std::string> _error_pages_paths;
		std::map<std::string, Location *> _locations;

		// Useless Constructors and copy Assignment
		VirtualServer();
		VirtualServer(const VirtualServer &);
		VirtualServer &operator=(const VirtualServer &);
	public:
};

#endif
