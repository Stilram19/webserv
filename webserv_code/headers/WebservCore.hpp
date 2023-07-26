/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebservCore.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: obednaou <obednaou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/22 17:31:58 by obednaou          #+#    #+#             */
/*   Updated: 2023/07/26 12:18:16 by obednaou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_CORE_HPP
# define WEBSERV_CORE_HPP

# include "GlobalHeader.hpp"

// To add in ConfigFileParsing: check if there is more than one server with the same server_name and listen informations
// !todo fcntl(fd, F_SETFL, O_NONBLOCK); should be called for every client socket descriptor.

class WebservCore
{
	private:
		// (*) Attributes

		// Pairing each connection socket with the virtual servers that listen using it.
		std::map<int, std::vector<VirtualServer *> >	_listens;

		// Clients Handlers 
		std::vector<Client *> _clients;

		// Socket Descriptor Sets for read and write (used in select)
		int		_fds_count;
		fd_set	_read_sockets, _write_sockets;

		// fd_sets copies used as temporaries in select
		fd_set select_read_sockets, select_write_sockets;

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
		Client		*new_client(int client_socket, int listen_socket);
		void		drop_client(std::vector<Client *>::iterator it);
		void		accept_new_connection_requests();
		void		serve_connected_clients();
		int			get_current_nfds() const;
		bool		is_server_busy() const;

	public:
		// (*) WebservCore's Main Function
		void	launch_server();
};

#endif
