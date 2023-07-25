/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebservCore.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: obednaou <obednaou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/23 15:05:37 by obednaou          #+#    #+#             */
/*   Updated: 2023/07/25 20:28:31 by obednaou         ###   ########.fr       */
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
	FD_ZERO(&_read_sockets);
	FD_ZERO(&_write_sockets);
	FD_ZERO(&select_read_sockets);
	FD_ZERO(&select_write_sockets);
	for (std::map<int, std::vector<VirtualServer *> >::const_iterator it = _listens.begin(); it != _listens.end(); it++)
		FD_SET(it->first, &_read_sockets);
}

WebservCore::~WebservCore() {}


// *********************** HELPERS ***********************


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
`
	if (connection_socket == -1)
		throw std::runtime_error("Failed to Create Socket!");

	// Setting the SO_REUSEADDR and SO_NOSIGPIPE flags at the SOL_SOCKET level.
	int set_option_on = 1;
	setsockopt(connection_socket,  SOL_SOCKET,  SO_REUSEADDR , &set_option_on, sizeof(set_option_on));
    setsockopt(connection_socket,  SOL_SOCKET,  SO_NOSIGPIPE, &set_option_on, sizeof(set_option_on));

	// Giving the socket a name
	if (bind(connection_socket, res->ai_addr, res->ai_addrlen))
		throw std::runtime_error("Failed to name a socket!");

	// Listening
	if (listen(connection_socket, 10000))
		throw std::runtime_error("Failed to listen!");

	return (connection_socket);
}

Client		*WebservCore::new_client(int client_socket, int listen_socket)
{
	Client *new_c = new Client(client_socket, listen_socket);

	_clients.push_back(new_c);
	return (new_c);
}

void		WebservCore::drop_client(std::vector<Client *>::iterator it)
{
	Client *client = (*it);
	int client_socket = client->get_client_socket();

	// clearing the bit that represents the client_socket in both read and write fd_sets
	FD_CLR(client_socket, &_read_sockets);
	FD_CLR(client_socket, &_write_sockets);

	FD_CLR(client_socket, &select_read_sockets);
	FD_CLR(client_socket, &select_write_sockets);

	// destroying the client object ==> releasing the heap memory reserved to it ==> removing the useless pointer from the _client vector
	delete client;
	_clients.erase(it);
}

void	WebservCore::accept_new_connection_requests()
{
	for (std::map<int, std::vector<VirtualServer *> >::const_iterator it = _listens.begin(); it != _listens.end(); it++)
	{
		socklen_t	addr_len;
		int			listen_socket = it->first;
		struct sockaddr client_addr;

		// !Check if the server is busy
		if (!FD_ISSET(listen_socket, &select_read_sockets))
			continue ;

		// Send SYN-ACK to client.
		int client_socket = accept(listen_socket, &client_addr, &addr_len);

		if (client_socket)
			throw std::runtime_error("Failed to accept client's connection request!");

		// Setting the new_client's socket in read_sockets, to be selected for read.
		FD_SET(client_socket, &select_read_sockets);

		// create new client handler
		new_client(client_socket, listen_socket);
	}
}

void	WebservCore::serve_connected_clients()
{
	int			client_socket;

	for (std::vector<Client *>::iterator it = _clients.begin(); it != _clients.end(); it++)
	{
		client_socket = (*it)->get_client_socket();

		// (*) Read Client's sent packets, if any.
		if (FD_ISSET(client_socket, &select_read_sockets))
		{
			//!todo handle request reading
			//!todo assign config to client
			continue ;
		}

		// (*) Respond to Client, if ready.
		if (!FD_ISSET(client_socket, &select_write_sockets))
			continue ;

		if ((*it)->is_request_done())
		{
			//! respond
		}
		//! delete_client();
	}
}

int	WebservCore::get_current_nfds() const
{
	int max_fd = -1;

	for (std::map<int, std::vector<VirtualServer *> >::const_iterator it = _listens.begin(); it != _listens.end(); it++)
	{
		int listen_socket = it->first;

		if (listen_socket > max_fd)
			max_fd = listen_socket;
	}

	for (std::vector<Client *>::const_iterator it = _clients.begin(); it != _clients.end(); it++)
	{
		int client_socket = (*it)->get_client_socket();

		if (client_socket > max_fd)
			max_fd = client_socket;
	}
	return (max_fd + 1);
}

// *********************** WebservCore's Main Function ***********************

void	WebservCore::launch_server()
{
	int		nfds;

	while (1)
	{
		// the successor of the max of all fds to be selected
		nfds = get_current_nfds();

		// Using temporaries of read and write sockets (select overwrites the content of fd sets)
		FD_COPY(&select_read_sockets, &_read_sockets);
		FD_COPY(&select_write_sockets, &_write_sockets);

		// (*) Select file descriptors that are ready for I/O operations
		if (select(nfds, &select_read_sockets, &select_write_sockets, NULL, NULL))
			throw std::runtime_error("Select failed!");

		// (*) Check and Accept new Connection requests (the SYN-ACK of the server in the three-way handshake)
		accept_new_connection_requests();

		// (*) Read Client's sent packets, and Answer Clients that are waiting for Response.
		serve_connected_clients();
	}
}
