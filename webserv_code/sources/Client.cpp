/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: obednaou <obednaou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/24 13:47:32 by obednaou          #+#    #+#             */
/*   Updated: 2023/08/13 06:00:37 by obednaou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "Client.hpp"

// ************* Constructor & Destructor *************

Client::Client(int client_socket, const std::vector<VirtualServer *> &VServers) 
	: _is_request_done(false), _is_response_done(false), _keep_alive(false), _client_disconnect(false), _request_status(WORKING), \
    _client_socket(client_socket), _VServers(VServers), _request(NULL), _response(NULL) {}

Client::~Client()
{
	// cutting the connection with the client
	close(_client_socket);

    delete _request;
    delete _response;
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

bool Client::is_response_done() const
{
    return (_is_response_done);
}

bool	Client::get_request_status() const
{
	return (_request->get_status());	
}

bool    Client::did_client_disconnect() const
{
    return (_client_disconnect);
}

bool    Client::is_connect_keep_alive() const
{
    return (_keep_alive);
}

// ******************* HELPERS *******************

inline void Client::new_request()
{
    _request = new Request(_client_socket, _VServers);
}

inline void Client::new_response()
{
    _response = new Response(_client_socket, _request);
}

void    Client::reset()
{
    _is_request_done = false;
	_is_response_done = false;
	_keep_alive = false;
	_client_disconnect = false;
	_request_status = WORKING;
}

// ******************* Client Handlers ********************

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

void    Client::response_handling()
{
    if (!_response)
        new_response();

    _response->response_handling();
    std::cout << (void *)_response << std::endl;

    if (_response->get_status() != WORKING)
    {
        _is_response_done = true;
        _client_disconnect = (_response->get_status() == BAD_TERM);
        delete _response;
        delete _request;
        _response = NULL;
        _request = NULL;
    }
}
