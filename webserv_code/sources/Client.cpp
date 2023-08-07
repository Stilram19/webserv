/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: obednaou <obednaou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/24 13:47:32 by obednaou          #+#    #+#             */
/*   Updated: 2023/08/07 10:33:17 by obednaou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "Client.hpp"

// ************* Constructor & Destructor *************

Client::Client(int client_socket, const std::vector<VirtualServer *> &VServers) 
	: _is_request_done(false), _keep_alive(false), _client_disconnect(false), _request_status(WORKING), \
    _client_socket(client_socket), _VServers(VServers), _request(NULL) {}

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

bool    Client::did_client_disconnect() const
{
    return (_client_disconnect);
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
    // std::cout << "FINE" << std::endl;
    _request->request_parsing();
    if (_request->get_status() != WORKING)
    {
        _is_request_done = true;
        _request->close_body_file();
        _keep_alive = _request->is_connect_keep_alive();
    }
    _client_disconnect = (_request->get_error_type() == CLIENT_DISCONNECT);
}
