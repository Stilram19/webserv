/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: obednaou <obednaou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/24 13:47:32 by obednaou          #+#    #+#             */
/*   Updated: 2023/07/28 19:35:47 by obednaou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "Client.hpp"

// ************* Constructor & Destructor *************

Client::Client(int client_socket, const std::vector<VirtualServer *> &VServers) \ 
	: _is_request_done(false), _keep_alive(false), _request_status(WORKING), _client_socket(client_socket), _VServers(VServers), _VServer(NULL), _request(NULL) {}

Client::~Client()
{
	// cutting the connection with the client
	close(_client_socket);

    delete _request;
}

// ******************* GETTERS *******************

int Client::get_client_socket() const
{
	return (_client_socket);
}

bool Client::is_request_done() const
{
	return (_is_request_done);
}

bool	Client::get_request_status() const
{
	return (_request->get_status());	
}

// ******************* HELPERS *******************

inline void    Client::new_request()
{
    _request = new Request(_client_socket, _request_body_file_name, _VServers);
}

// ******************* SETTERS ********************

void	Client::request_handling()
{
    if (!_request)
        new_request();
    _request->request_parsing();
    if (_request->get_status() != WORKING)
        _is_request_done = true;
}
