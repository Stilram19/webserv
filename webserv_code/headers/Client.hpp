/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: obednaou <obednaou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/24 13:40:31 by obednaou          #+#    #+#             */
/*   Updated: 2023/08/13 06:01:46 by obednaou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

class VirtualServer;
class Request;
class Response;

# include "include.hpp"

class Client
{
	private:
		// (*) Attributes

		// Request Infos
		bool	_is_request_done;
		bool	_is_response_done;
		bool	_keep_alive;
		bool	_client_disconnect;
		int		_request_status;

		// socket to read from and write to the client
		int _client_socket;

		// reference to the collection of virtual servers listening at the same endpoint
		// to which the client sent a SYN
		const std::vector<VirtualServer *> &_VServers;

		// Request Handler
		Request	*_request;

		// Response Handler
		Response	*_response;	

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
		void		new_response();
		void    	reset();
	public:
		// Getters
		int		get_client_socket() const;
		bool	is_request_done() const;
		bool	is_response_done() const;
		bool	get_request_status() const;
		bool	did_client_disconnect() const;
		int		get_request_error() const;
		bool	is_connect_keep_alive() const;

	public:
		// Client Handlers
		void	request_handling();
		void    response_handling();
};

# include "Request.hpp"
# include "VirtualServer.hpp"
# include "Response.hpp"

#endif
