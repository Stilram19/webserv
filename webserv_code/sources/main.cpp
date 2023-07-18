/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: obednaou <obednaou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/18 08:41:50 by obednaou          #+#    #+#             */
/*   Updated: 2023/07/18 09:31:33 by obednaou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "GlobalHeader.hpp"

int	main(int argc, char **argv)
{
	if (argc != 2)
	{
		std::cout << "Invlalid number of arguments!" << std::endl;
		return (1);
	}

	ConfigFileParser parser(*(argv + 1));

	parser.config_file_parsing();
	return (0);
}
