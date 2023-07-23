/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebservCore.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: obednaou <obednaou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/22 17:31:58 by obednaou          #+#    #+#             */
/*   Updated: 2023/07/23 18:28:26 by obednaou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_CORE_HPP
# define WEBSERV_CORE_HPP

# include "GlobalHeader.hpp"

// To add in ConfigFileParsing: check if there is more than one server with the same server_name and listen informations
/* 
fcntl(fd, F_SETFL, O_NONBLOCK); should be called for every socket descriptor.
For each listen endpoint a listen socket for connexions must be created
Using select we select the socket descriptors that are ready to read from.
If a connexion socket is selected this means that a connexion request is sent, then it should be accepted using accept.
If a client socket (assigned already when this client got accepted), it means that a client has sent a sequence of bytes, that should be read and handled appropriately.
 */

class WebservCore
{
	private:
		// (*) Attributes

		// the collection of sockets used for establishing connections with clients
		std::map<int, std::vector<VirtualServer *> >	_connection_sockets;

		// Socket Descriptor Sets for read and write (parameters of select)
		fd_set read_sockets, write_sockets;

		// (*) Useless Constructors & Copy Assignment
		WebservCore();
		WebservCore(const WebservCore &);
		WebservCore &operator=(const WebservCore &);
	public:
		// (*) Constructor & Destructor
		WebservCore(const std::vector<VirtualServer *> &VServers);
		~WebservCore();

	private:
		// Helpers
		static int	create_socket(const std::string &hostname, const std::string &port_number);

	public:
		// (*) WebservCore's Main Function
		launch_server();
};

#endif
