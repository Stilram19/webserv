/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   VirtualServer.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: obednaou <obednaou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/16 15:44:47 by obednaou          #+#    #+#             */
/*   Updated: 2023/07/16 17:39:00 by obednaou         ###   ########.fr       */
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
		int _max_client_body_size;
		std::string _server_name;
		std::string _port_number;
		std::string _host_address;
		std::map<std::string, std::string> _default_error_pages_paths;
		std::vector<Location *> _routes;

		// Useless Constructors and copy Assignment
		VirtualServer();
		VirtualServer(const VirtualServer &);
		VirtualServer &operator=(const VirtualServer &);
	public:
};

#endif
