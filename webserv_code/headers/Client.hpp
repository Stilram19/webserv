/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: obednaou <obednaou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/24 13:40:31 by obednaou          #+#    #+#             */
/*   Updated: 2023/07/25 11:43:47 by obednaou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "GlobalHeader.hpp"

class Client
{
	private:
		// (*) Attributes

		// this flag answers the question: "is the Client Done sending the request and is ready to receive the response?"
		bool _is_request_done;

		// socket to read from and write to the client
		int _client_socket;

		// the listen socket that the client got accepted through (useful as a key to the map of _listens in the WebservCore)
		int _listen_socket;

		// Clients Configuration
		VirtualServer *_VServer;

		// (*) Useless Constructors & Copy Assignment
		Client();
		Client(const Client &);
		Client &operator=(const Client &);

	public:
		// Constructor & Destructor
		Client(int, int);
		~Client();
	public:
		// this function answers the question: "Does the client already knows its virtual server?"
		bool is_config_set() const;

	public:
		// getters
		int		get_client_socket() const;
		bool	is_request_done();
}

#endif
