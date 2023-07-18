/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigFileParser.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: obednaou <obednaou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/16 15:11:23 by obednaou          #+#    #+#             */
/*   Updated: 2023/07/18 09:16:31 by obednaou         ###   ########.fr       */
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
		ConfigFileParser(const char *);
		~ConfigFileParser();
	private:
		// Helper Methods
		void	buffering_input_file();

	private:
		// Syntax checker Methods

		// Syntax checker Main Method
		void	syntax_checker() const;

		// Server block syntax checkers
		int		server_block_syntax_checker(int start) const;
		int		server_tokens_syntax_checker(int start) const;
		int 	check_server_token(const char *ptr, int index) const;
		int		check_server_token_value(const char *ptr, int offset1, int offset2) const;

		// Location block syntax checkers
		int		location_syntax_checker(int start) const;
		int 	location_tokens_syntax_checker(int start) const;
		int		check_location_token_value(const char *ptr, int offset1, int offset2) const;
		int 	check_location_token(const char *ptr, int index) const;
	public:
		// Main Method
		void	config_file_parsing();
};

#endif
