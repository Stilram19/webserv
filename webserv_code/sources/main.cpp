/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/18 08:41:50 by obednaou          #+#    #+#             */
/*   Updated: 2023/08/02 14:05:37 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "ConfigFileParser.hpp"
# include "WebservCore.hpp"

int	main(int argc, char **argv)
{
	try
	{
		if (argc != 2)
			throw std::runtime_error("Invalid number of arguments!");

		// Config File Parsing
		ConfigFileParser parser(*(argv + 1));

		parser.config_file_parsing();

		// Launching the Server
		WebservCore webserv(parser.get_parsing_result());
		webserv.launch_server();
	}
	catch (std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		return (EXIT_FAILURE);
	}
	return (EXIT_SUCCESS);
}
