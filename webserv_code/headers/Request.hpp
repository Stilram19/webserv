/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: obednaou <obednaou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/26 18:23:13 by obednaou          #+#    #+#             */
/*   Updated: 2023/07/27 18:12:17 by obednaou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "GlobalHeader.hpp"

class Request
{
	public:
		// types
		typedef void (Request::*PtrToRequestHandler)();
	private:
		// Attributes
		bool		_request_handling_state;
		int			_client_socket;
		std::string body_file_name;
		std::string header_buffer;
		std::map<int, PtrToRequestHandler> _request_handlers;

		// Useless Constructors & Destructors
		Request();
		Request(const Request &);
		Request &operator=(const Request &);
	public:
		// Constructor & Destructor
		Request(int client_socket);
		~Request();

	private:
		// Helpers
		static void	random_file_name_generation(std::string &file_name);

		// Request Handlers
		void		header_reader();
		void		header_parser();
		void		body_reader();

	public:
		// Getters

	public:
		// Main Method
		void	request_parsing();
};
