/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: obednaou <obednaou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/24 13:40:31 by obednaou          #+#    #+#             */
/*   Updated: 2023/07/24 14:21:17 by obednaou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "GlobalHeader.hpp"

class Client
{
	private:
		// (*) Attributes

		// socket to read from and write to the client
		int _client_socket;

		// the listen socket that the client got accepted through (useful as a key to the map of _listens in the WebservCore)
		int _listen_socket;

		// Clients Configuration
		VirtualServer *VServer;

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

}

#endif
