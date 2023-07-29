/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: obednaou <obednaou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/26 18:23:13 by obednaou          #+#    #+#             */
/*   Updated: 2023/07/29 17:57:18 by obednaou         ###   ########.fr       */
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

		// Request Line Infos
		int			_http_method;
		std::string _request_uri;
		std::string _resource_path;
		std::string	_query_string;
		std::string _fragment;

		// Headers Infos
		std::map<std::string, std::vector<std::string> > _request_headers;

		// the request handling status (still working or terminated due to a disconnect, bad request ...)
		int	_status;
		int	_error_type;

		// the request handling step (header reading, header parsing, body reading)
		int	_handling_step;

		int	_client_socket;
		std::string &_body_file_name;
		std::string _header_buffer;
		std::string _body_consumed_bytes;

		// Body related headers
		bool	transfer_encoding_chunked;
		size_t	content_length;

		// the client server is in this collection
		const std::vector<VirtualServer *> &_VServers;

		// Reference to the client's Virtual Server
		VirtualServer * &_VServer;

		// key : the handling step | Value : the corresponding handler method
		std::map<int, PtrToRequestHandler>	_handlers;

		// Useless Constructors & Destructors
		Request();
		Request(const Request &);
		Request &operator=(const Request &);

	public:
		// Constructor & Destructor
		Request(int, std::string &, const std::vector<VirtualServer *> &, VirtualServer * &);
		~Request();

	private:
		// Helpers
		static void	random_file_name_generation(std::string &file_name);
		int			request_line_parsing();
		void		headers_parsing(int start);
		int			get_http_method(const std::string &method);
		void		request_uri_parsing();
		void		request_uri_decoding();
		void		extracting_body_consumed_bytes();
		void		set_the_virtual_server();
		void		check_body_headers();

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
