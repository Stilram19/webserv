/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: obednaou <obednaou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/24 13:47:32 by obednaou          #+#    #+#             */
/*   Updated: 2023/07/24 14:05:02 by obednaou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "Client.hpp"

// ************* Constructor & Destructor *************

Client::Client(int client_socket, int listen_socket) : _client_socket(client_socket), _listen_socket(listen_socket), _VServer(NULL) {}

Client::~Client() {}

// Public Functions

bool is_config_set() const
{
	return ((_VServer) ? true : false);
}
