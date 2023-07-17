/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigFileParser.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: obednaou <obednaou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/16 15:11:23 by obednaou          #+#    #+#             */
/*   Updated: 2023/07/17 09:45:21 by obednaou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_FILE_PARSER_HPP
# define CONFIG_FILE_PARSER_HPP

# include "GlobalHeader.hpp"

class ConfigFileParser
{
	private:
		// Attributes
		std::string						_filename;
		std::string						_buffer;
		std::vector<VirtualServer *>	_VServers;

		// Useless Constructors & Copy Assignment
		ConfigFileParser();
		ConfigFileParser(const ConfigFileParser &);
		ConfigFileParser &operator=(const ConfigFileParser&);

		// Exceptions
		class invalid_syntax : public std::exception
		{
			public:
				const char *what() const throw ()
				{
					return ("ConfigFileParser: Syntax Error!");
				}
		};
	public:
		// Useful Constructor & Destructor
		ConfigFileParser(std::string &);
		~ConfigFileParser();
	private:
		// Helper Methods
		void	buffering_input_file();
		void	syntax_checker();
	public:
		// Main Method
		void	config_file_parsing();
};

#endif
