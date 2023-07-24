/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebservCore.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: obednaou <obednaou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/23 15:05:37 by obednaou          #+#    #+#             */
/*   Updated: 2023/07/24 14:21:09 by obednaou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "WebservCore.hpp"

// ******************** Constructor & Destructor ********************

WebservCore::WebservCore(const std::vector<VirtualServer *> &VServers)
{
	// (*) Constructing the 
	// collecting communication endpoints
	std::vector<std::pair<std::string, std::string> > endpoints;

	for (std::vector<VirtualServer *>::const_iterator it = VServers.begin(); it != VServers.end(); it++)
	{
		std::pair<std::string, std::string> endpoint;

		endpoint.first = (*it)->get_host_address();
		endpoint.second = (*it)->get_port_number_str();
		if (std::find(endpoints.begin(), endpoints.end(), endpoint) == endpoints.end())
			endpoints.push_back(endpoint);
	}

	// Constructing the _listens map
	for (std::vector<std::pair<std::string, std::string> >::const_iterator it = endpoints.begin(); it != endpoints.end(); it++)
	{
		int									connection_socket;
		std::vector<VirtualServer *>		v_servers;

		connection_socket = create_socket(it->first, it->second);
		for (std::vector<VirtualServer *>::const_iterator it1 = VServers.begin(); it1 != VServers.end(); it1++)
		{
			if ((*it1)->get_host_address() == it->first
				&& (*it1)->get_port_number_str() == it->second)
				v_servers.push_back((*it1));
		}
		_listens[connection_socket] = v_servers;
	}

	// Constructing the socket descriptors sets
	FD_ZERO(&read_sockets);
	FD_ZERO(&write_sockets);
	for (std::map<int, std::vector<VirtualServer *> >::const_iterator it = _listens.begin(); it != _listens.end(); it++)
		FD_SET(it->first, &read_sockets);
}

WebservCore::~WebservCore() {}

// ***********************            HELPERS          ***********************

// Constructor Helpers

int	create_socket(const std::string &hostname, const std::string &port_number)
{
	int				connection_socket;
	struct addrinfo	hints;
	struct addrinfo *res;

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = PF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	hints.ai_protocol = IPPROTO_TCP;

	getaddrinfo(hostname.c_str(), port_number.c_str(), &hints, &res);

	// creating a socket
	connection_socket = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

	if (connection_socket == -1)
	{
		std::cout << "Failed to create a socket!" << std::endl;
		exit(EXIT_FAILURE);
	}

	// Setting the SO_REUSEADDR and SO_NOSIGPIPE flags at the SOL_SOCKET level.
	int set_option_on = 1;
	setsockopt(connection_socket,  SOL_SOCKET,  SO_REUSEADDR , &set_option_on, sizeof(set_option_on));
    setsockopt(connection_socket,  SOL_SOCKET,  SO_NOSIGPIPE, &set_option_on, sizeof(set_option_on));

	// Giving the socket a name
	if (bind(connection_socket, res->ai_addr, res->ai_addrlen))
	{
		std::cout << "Failed to name a socket!" << std::endl;
		exit(EXIT_FAILURE);
	}

	// Listening
	if (listen(connection_socket, 10000))
	{
		std::cout << "Failed to listen!" << std::endl;
		exit(EXIT_FAILURE);
	}
	return (connection_socket);
}

Client		*WebservCore::new_client(int client_socket, int listen_socket)
{
	Client *new_c = new Client(client_socket, listen_socket);

	_clients.push_back(new_c);
	return (new_c);
}

// *********************** WebservCore's Main Function ***********************

void	WebservCore::launch_server()
{
	// Select file descriptors that are ready for I/O operations

	// iterate through the _listens map checking if a listen endpoint has received a connection, if so accept it and create
	// a client object that will handle the connection later on.

	// iterate through the _clients vector ,checking if a client has sent a packet ,if yes ,handle read, otherwise check if 
	// 
}
