/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: obednaou <obednaou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/26 18:23:13 by obednaou          #+#    #+#             */
<<<<<<< HEAD
/*   Updated: 2023/08/06 15:54:46 by obednaou         ###   ########.fr       */
=======
/*   Updated: 2023/08/08 16:12:06 by obednaou         ###   ########.fr       */
>>>>>>> c3dda2ce8d1438e118cfb560dd70e7e11bb048a4
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
# define REQUEST_HPP

# include "TEChunkedMiddleWare.hpp"

class Location;

class VirtualServer;

class Request
{
	public:
		// types
		typedef void (Request::*PtrToRequestHandler)();
	private:
		// (*) Attributes

		// Reference to the body_file_name field in the client
		int			_body_fd;
		std::string &_body_file_name;

		// the collection of virtual server targeted by the client
		const std::vector<VirtualServer *> &_VServers;

		// The virtual server and the location
		VirtualServer						*_VServer;
		std::pair<std::string, Location *>	_location;

		// set when the client requests the server to keep the connection open for incomming requests
		bool _keep_alive;

		// Request Line Infos
		std::string	_http_method;
		std::string _request_uri;
		std::string _resource_path;
		std::string	_query_string;
		std::string _fragment;

		// Headers Infos
		std::map<std::string, std::vector<std::string> > _request_headers;

		// the request handling status (still working or terminated due to a disconnect, bad request ...)
		int				_status;
		e_status_code	_error_type;

		// the request handling step (header reading, header parsing, body reading)
		int	_handling_step;

		int				_client_socket;
		size_t			header_read_bytes;
		RawDataBuffer	*_raw_header_buffer;
		std::string		_header_buffer;

		// Body related
		bool			transfer_encoding_chunked;
		size_t			content_length;
		size_t			body_read_bytes;
		size_t			_consumed_body_bytes_size;
		RawDataBuffer	*_consumed_body_bytes;

		// 'Transfer-Encoding: chunked' handler
		TEChunkedMiddleWare *te_chunked_middle_ware;

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
		void		random_file_name_generation(std::string &file_name);
		int			request_line_parsing();
		void		headers_parsing(int start);
		int			get_http_method(const std::string &method);
		void		request_uri_parsing();
		void		request_uri_decoding();
		void		extracting_body_consumed_bytes();
		void		set_config_infos();
		void		set_virtual_server();
		void		set_location();
		void		set_real_resource_path();
		void		extracting_body_headers();
		void		important_headers_extraction();
		void		extracting_connection_type();
		void		display_request_header_infos(); // for debugging purposes
		void		extract_body_chunk(const char *body_packet, size_t read_bytes);
		void		append_chunk_to_body_file(const char *body_chunk, size_t read_bytes);
		int			read_sent_packet(char *buffer);
		void		close_body_file();

	private:
		// Request Handlers
		void	header_reader();
		void	header_parser();
		void	body_reader();

	public:
		// Getters
		bool				get_status() const;
<<<<<<< HEAD
		int					get_error_type() const;
=======
		e_status_code		get_error_type() const;
>>>>>>> c3dda2ce8d1438e118cfb560dd70e7e11bb048a4
		bool				is_connect_keep_alive() const;
		VirtualServer		*get_server() const;
		Location			*get_location() const;
		const std::string 	&get_uri_resource_path() const;
		const std::string	&get_request_method() const;

	public:
		// Main Method
		void	request_parsing();
};

# include "VirtualServer.hpp"

#endif
