/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: obednaou <obednaou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/24 13:40:31 by obednaou          #+#    #+#             */
/*   Updated: 2023/07/30 18:58:59 by obednaou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "GlobalHeader.hpp"

class Client
{
	private:
		// (*) Attributes

		// Request Infos
		bool	_is_request_done;
		bool	_keep_alive;
		bool	_client_disconnect;
		int		_request_status;

		// socket to read from and write to the client
		int _client_socket;

		// reference to the collection of virtual servers listening at the same endpoint
		// to which the client sent a SYN
		const std::vector<VirtualServer *> &_VServers;

		// Virtual Server
		VirtualServer *_VServer;

		// Random Name of the file of the request body
		//! unlink in the response
		std::string _request_body_file_name;

		// Request Handler
		Request *_request;

		// (*) Useless Constructors & Copy Assignment
		Client();
		Client(const Client &);
		Client &operator=(const Client &);

	public:
		// Constructor & Destructor
		Client(int, const std::vector<VirtualServer *> &);
		~Client();

	public:
		// Helpers
		void		new_request();

	public:
		// Getters
		int		get_client_socket() const;
		bool	is_request_done() const;
		bool	get_request_status() const;
		bool	did_client_disconnect() const;

	public:
		// Client Handlers
		void	request_handling();
};

#endif
