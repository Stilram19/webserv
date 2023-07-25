/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: obednaou <obednaou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/18 08:41:50 by obednaou          #+#    #+#             */
/*   Updated: 2023/07/25 19:43:42 by obednaou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "GlobalHeader.hpp"

int	main(int argc, char **argv)
{
	// ConfigFileParsing

	try
	{
		// Config File Parsing
		if (argc != 2)
			throw std::runtime_error("Invalid number of arguments!");

		ConfigFileParser parser(*(argv + 1));

		parser.config_file_parsing();

		// Launching the Server
		WebservCore webserv(parser.get_parsing_result());
	}

	catch (std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		return (EXIT_FAILURE);
	}
	return (EXIT_SUCCESS);
}
