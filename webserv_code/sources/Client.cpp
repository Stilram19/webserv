/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: obednaou <obednaou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/24 13:47:32 by obednaou          #+#    #+#             */
/*   Updated: 2023/07/25 11:43:50 by obednaou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "Client.hpp"

// ************* Constructor & Destructor *************

Client::Client(int client_socket, int listen_socket) : _is_request_done(false), _client_socket(client_socket), _listen_socket(listen_socket), _VServer(NULL) {}

Client::~Client() {}

// Public Functions

bool Client::is_config_set() const
{
	return ((_VServer) ? true : false);
}

// ******************* GETTERS *******************

int Client::get_client_socket() const
{
	return (_client_socket);
}

bool Client::is_request_done()
{
	bool ret = _is_request_done;

	_is_request_done = false;
	return (ret);
}
