/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigFileParser.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: obednaou <obednaou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/16 15:11:29 by obednaou          #+#    #+#             */
/*   Updated: 2023/07/17 18:54:03 by obednaou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "ConfigFileParser.hpp"

ConfigFileParser::ConfigFileParser(const char *filename) : _filename(filename) {}

ConfigFileParser::~ConfigFileParser()
{
	for (std::vector<VirtualServer *>::iterator it = _VServers.begin(); it != _VServers.end(); it++)
		delete *it;	
}

// ******************* HELPER METHODS *******************

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

// ******************* SYNTAX CHECKER METHODS *******************

// Main Method
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

// Server Block Syntax checkers
int	ConfigFileParser::server_block_syntax_checker(int start)
{
	if (strncmp(buffer.c_str() + start, "server", 6))
		throw invalid_syntax();
	while (isspace(buffer[start]))
		start++;
	if (buffer[start] != '{')
		throw invalid_syntax();
	start = server_tokens_syntax_checker(start);
	while (isspace(buffer[start]))
		start++;
	if (buffer[start] != '}')
		throw invalid_syntax();
	return (start + 1);
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
		int temp = check_server_token(buffer.c_str, start);
		if (temp != start)
		{
			start = temp;
			continue ;
		}
		if (!strncmp(buffer.c_str() + start, "location", 8))
		{
			start = location_syntax_checker(start + 8);
			continue ;
		}
		throw invalid_syntax();
	}
	return (start);
}

int ConfigFileParser::check_server_token(const char *ptr, int index)
{
	if (!strncmp(ptr + index, "listen", 6))
		return (check_server_token_value(ptr, index, 6));
	if (!strncmp(ptr + index, "server_name", 11))
		return (check_server_token_value(ptr, index, 11));
	if (!strncmp(ptr + index, "max_client_body_size", 20))
		return (check_server_token_value(ptr, index, 20));
	if (!strncmp(ptr + index, "error_page", 10))
		return (check_server_token_value(ptr, index, 10));
	return (index);
}

int	ConfigFileParser::check_server_token_value(const char *ptr, int offset1, int offset2)
{
	int	words_count = 0;
	int	index = offset1 + offset2;

	if (!isblank(ptr[index]))
		throw invalid_syntax();
	while (ptr[index])
	{
		while (isblank(ptr[index]))
			index++;
		if (!ptr[index] || ptr[index] == '\n')
			break ;
		words_count++;
		while (ptr[index] && !isspace(ptr[index]))
			index++;
	}
	if (ptr[index] != '\n')
		throw invalid_syntax();
	if (count != 1)
	{
		if (count == 2 && !strncmp(ptr[offset1], "error_page", 10))
			return (index);
		throw invalid_syntax();
	}
	return (index);
}

// Location block syntax checkers
int	ConfigFileParser::location_syntax_checker(int start)
{
	bool ret = false;

	if (!isblank(buffer[start]))
		throw invalid_syntax();
	while (isspace(buffer[start]))
		start++;
	while (buffer[start] && !isspace(buffer[start]))
	{
		start++;
		ret = true;
	}
	while (isspace(buffer[start]))
		start++;
	if (buffer[start] != '{')
		throw invalid_syntax();
	start = location_tokens_syntax_checker(start);
	while (isspace(buffer[start]))
		start++;
	if (buffer[start] != '}')
		throw invalid_syntax();
	return (start);
}

int ConfigFileParser::location_tokens_syntax_checker(int start)
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
		int temp = check_location_token(buffer.c_str, start);
		if (temp != start)
		{
			start = temp;
			continue ;
		}
		throw invalid_syntax();
	}
}

int	ConfigFileParser::check_location_token_value(const char *ptr, int offset1, int offset2)
{
	int	words_count = 0;
	int	index = offset1 + offset2;

	if (!isblank(ptr[index]))
		throw invalid_syntax();
	while (ptr[index])
	{
		while (isblank(ptr[index]))
			index++;
		if (!ptr[index] || ptr[index] == '\n')
			break ;
		words_count++;
		while (ptr[index] && !isspace(ptr[index]))
			index++;
	}
	if (ptr[index] != '\n')
		throw invalid_syntax();
	if (count != 1)
	{
		if (count >= 1 && count <= 3 && !strncmp(ptr[offset1], "allowed_methods", 15))
			return (index);
		throw invalid_syntax();
	}
	return (index);
}

int ConfigFileParser::check_location_token(const char *ptr, int index)
{
	if (!strncmp(ptr + index, "root", 4))
		return (check_location_token_value(ptr, index, 10));
	if (!strncmp(ptr + index, "allowed_methods", 15))
		return (check_location_token_value(ptr, index, 6));
	if (!strncmp(ptr + index, "redirect", 8))
		return (check_location_token_value(ptr, index, 11));
	if (!strncmp(ptr + index, "index", 5))
		return (check_location_token_value(ptr, index, 20));
	return (index);
}

// ******************* PARSER MAIN METHOD *******************

void	ConfigFileParser::config_file_parsing()
{
	try
	{
		buffering_input_file();
		syntax_checker();
		filling_attributes();
	}
	catch (const std::exception &e)
	{
		std::cout << e.what() << std::endl;
		exit(1);
	}
}
