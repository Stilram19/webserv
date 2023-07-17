/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigFileParser.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: obednaou <obednaou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/16 15:11:29 by obednaou          #+#    #+#             */
/*   Updated: 2023/07/17 10:03:43 by obednaou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "ConfigFileParser.hpp"

ConfigFileParser::ConfigFileParser(const char *filename) : _filename(filename) {}

ConfigFileParser::~ConfigFileParser()
{
	for (std::vector<VirtualServer *>::iterator it = _VServers.begin(); it != _VServers.end(); it++)
		delete *it;	
}

// Helper Methods
void	ConfigFileParser::buffering_input_file()
{
	std::string read_line;
	std::istream input_stream(_filename, ios_base::in);

	if (!input_stream.is_open())
		throw std::runtime_error("Can't open file!");
	if (!std::getline(input_stream, read_line))
		throw std::runtime_error("Empty File!");
	buffer = read_line;
	buffer += "\n";
	while (std::getline(input_stream, read_line))
	{
		buffer += read_line;
		buffer += "\n";
	}
}

// start => loop : blank.start|server|EOF
// server => blank.server|{ => server_tokens => }
// server_tokens => loop : blank.server_tokens|listen|server_name|max_client_body_size|error_page|location
// listen => blank.listen|
// server_name => 

// server
// {
//     listen     localhost:3011
//     #server_name            host1
//     max_client_body_size 50000000
//     error_page 404 ../public/errors/404.html
//     error_page 400 ../public/errors/400.html
//     location  /
//     {
//         allow_methods GET POST DELETE
//         #redirect http://www.google.com
//         auto_index on
//         root ../public/
//         #index page2.html
//         upload_pass ./uploads
//         cgi_pass php     /Users/aben-ham/.1337/projects/web_server/cgi_bin/php-cgi
//         cgi_pass .py /usr/local/bin/python3
//     }
//     location /planet {
//         allow_methods GET
//         auto_index off
//         root ../public/planet
//         upload_pass /Users/aben-ham/.1337/projects/web_server/public/uploads
//         #index index.html index.php
//         cgi_pass .php ./cgi-bin/php-cgi
//     }

//     location /planet/image {
//         allow_methods GET
//         auto_index on
//         root ../public
//         index index.html
//         cgi_pass .php ./cgi-bin/php-cgi
//     }

//     location /planet/image/ {
//         allow_methods GET
//         auto_index on
//         root ../public
//         index index.html
//         cgi_pass .php ./cgi-bin/php-cgi
//     }
// }

bool ConfigFileParser::is_token(const char *ptr)
{
	if (!strncmp(ptr, "listen ", 7))
		return (true);
	if (!strncmp(ptr, "server_name ", 12))
		return (true);
	if (!strncmp(ptr, "max_client_body_size ", 21))
		return (true);
	if (!strncmp(ptr, "error_page ", 11))
		return (true);
	return (false);
}

int	check_token_value(const char *ptr)
{
	const char *temp = ptr;

	ptr = strchr(ptr, ' ');
}

int	ConfigFileParser::server_tokens_syntax_checker(int start)
{
	while (buffer[start])
	{
		if (isspace(buffer[start] && ++start))
			continue ;
		if (buffer[start] == '#')
		{
			start = strchr(buffer.c_str + start, '\n') - buffer.c_str + 1;
			continue ;
		}
		if (is_token(buffer.c_str() + start))
		{
			start = check_token_value(buffer.c_str() + start);
			continue ;
		}
		if (!strncmp(buffer.c_str() + start, "location ", 9))
		{
			start = location_syntax_checker();
			continue ;
		}
		throw invliad_syntax();
	}
}

int	ConfigFileParser::server_block_syntax_checker(int start)
{
	// checking server context syntax
	if (strncmp(buffer.c_str() + start, "server", 6))
		throw invalid_syntax();
	while (buffer[start])
	{
		if (isspace(buffer[start]) && ++index)
			continue ;
		if (buffer[start] != '{')
			throw invalid_syntax();
		while (buffer[start])
			start = server_tokens_syntax_checker(start);
		while (isspace(buffer[start]))
			start++;
		if (buffer[start] != '}')
			throw invalid_syntax();
	}
	return (start);
}

// syntax errors test
void	ConfigFileParser::syntax_checker()
{
	int	index = 0;

	while (buffer[index])
	{
		if (isspace(buffer[index]) && ++index)
			continue ;
		index = server_block_syntax_checker(index);
	}
}

// Main Method
void	ConfigFileParser::config_file_parsing()
{
	try
	{
		buffering_input_file();
		syntax_test();
		filling_attributes();
	}
	catch (const std::exception &e)
	{
		std::cout << e.what() << std::endl;
		exit(1);
	}
}
