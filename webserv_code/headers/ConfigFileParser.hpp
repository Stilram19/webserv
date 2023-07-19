/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigFileParser.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: obednaou <obednaou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/16 15:11:23 by obednaou          #+#    #+#             */
/*   Updated: 2023/07/19 12:52:25 by obednaou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_FILE_PARSER_HPP
# define CONFIG_FILE_PARSER_HPP

# include "GlobalHeader.hpp"

class ConfigFileParser
{
	private:
		// Attributes
		std::string						_buffer;
		std::string						_filename;
		std::vector<VirtualServer *>	_VServers;

		// Useless Constructors & Assignment
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
		// Global Helpers
		void	buffering_input_file();
		int		skip_blank(const char *ptr, int start) const;
		int		skip_spaces(const char *ptr, int start) const;

	private:
		// ********************* Syntax Checking *********************

		// Syntax checker's Main Method
		void	syntax_checker() const;

		// Helpers
		int		count_and_skip_words(const char *ptr, int *index_ptr) const;

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
	
	private:
		// ********************* Extracting config infos *********************

		// Helpers
		VirtualServer *new_virtual_server();

		// Extracting config infos Main Method
		void	extracting_config_infos();
		int		extract_server_tokens(int index);
		int		extract_and_set_attributes(VirtualServer *vs, int index);
		int		extract_token(VirtualServer *vs, const char *ptr, int index);
		int		set_server_token_value(const std::string &token_type, const char *token_value);
	public:
		// ConfigFileParser Main Method
		void	config_file_parsing();
};

#endif
