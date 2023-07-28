/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: obednaou <obednaou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/26 18:23:13 by obednaou          #+#    #+#             */
/*   Updated: 2023/07/28 19:52:17 by obednaou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "GlobalHeader.hpp"

class Request
{
	public:
		// types
		typedef void (Request::*PtrToRequestHandler)();
	private:
		// (*) Attributes

		// set when the client requests the server to keep the connection open for incomming requests
		bool _keep_alive;

		int			_http_method;
		std::string url;

		// the request handling status (still working or terminated due to a disconnect, bad request ...)
		int	_status;

		// the request handling step (header reading, header parsing, body reading)
		int	_handling_step;
		int	_client_socket;
		std::string &_body_file_name;
		std::string _header_buffer;
		const std::vector<VirtualServer *> &_VServers;

		std::string body_comsumed_bytes;

		// key : the handling step | Value : the corresponding handler method
		std::map<int, PtrToRequestHandler>	_handlers;

		// Useless Constructors & Destructors
		Request();
		Request(const Request &);
		Request &operator=(const Request &);

	public:
		// Constructor & Destructor
		Request(int, std::string &, const std::vector<VirtualServer *> &);
		~Request();

	private:
		// Helpers
		static void	random_file_name_generation(std::string &file_name);
		static int	skip_crlf(const char *temp);

	private:
		// Request Handlers
		void	header_reader();
		void	header_parser();
		void	body_reader();

	public:
		// Getters
		bool get_status() const;
		bool get_keep_alive() const;

	public:
		// Main Method
		void	request_parsing();
};
