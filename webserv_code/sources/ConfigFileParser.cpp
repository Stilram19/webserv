/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigFileParser.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: obednaou <obednaou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/16 15:11:29 by obednaou          #+#    #+#             */
/*   Updated: 2023/07/18 10:32:04 by obednaou         ###   ########.fr       */
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
	std::ifstream input_stream(_filename);

	if (!input_stream.is_open())
		throw std::runtime_error("Can't open file!");
	if (!std::getline(input_stream, read_line))
		throw std::runtime_error("Empty File!");
	_buffer = read_line;
	_buffer += "\n";
	while (std::getline(input_stream, read_line))
	{
		_buffer += read_line;
		_buffer += "\n";
	}
	std::cout << _buffer << std::endl;
	input_stream.close();
}

// ******************* SYNTAX CHECKER METHODS *******************

// Main Method
void	ConfigFileParser::syntax_checker() const
{
	int	index = 0;

	while (_buffer[index])
	{
		if (isspace(_buffer[index]) && ++index)
			continue ;
		index = server_block_syntax_checker(index);
	}
}

// Server Block Syntax checkers
int	ConfigFileParser::server_block_syntax_checker(int start) const
{
	if (strncmp(_buffer.c_str() + start, "server", 6))
		throw invalid_syntax();
	start += 6;
	while (isspace(_buffer[start]))
		start++;
	if (_buffer[start] != '{')
		throw invalid_syntax();
	start++;
	start = server_tokens_syntax_checker(start);
	if (_buffer[start] != '}')
		throw invalid_syntax();
	return (start + 1);
}

int	ConfigFileParser::server_tokens_syntax_checker(int start) const
{
	while (_buffer[start])
	{
		if (isspace(_buffer[start]) && ++start)
			continue ;
		if (_buffer[start] == '}')
			return (start);
		if (_buffer[start] == '#')
		{
			start = strchr(_buffer.c_str() + start, '\n') - _buffer.c_str() + 1;
			continue ;
		}
		int temp = check_server_token(_buffer.c_str(), start);
		if (temp != start)
		{
			start = temp;
			continue ;
		}
		if (!strncmp(_buffer.c_str() + start, "location", 8))
		{
			start = location_syntax_checker(start + 8);
			continue ;
		}
		throw invalid_syntax();
	}
	return (start);
}

int ConfigFileParser::check_server_token(const char *ptr, int index) const
{
	if (!strncmp(ptr + index, "listen", 6))
		return (check_server_token_value(ptr, index, 6));
	if (!strncmp(ptr + index, "server_name", 11))
		return (check_server_token_value(ptr, index, 11));
	if (!strncmp(ptr + index, "max_client_body_size", 20))
		return (check_server_token_value(ptr, index, 20));
	if (!strncmp(ptr + index, "error_pages", 11))
		return (check_server_token_value(ptr, index, 11));
	return (index);
}

int	ConfigFileParser::check_server_token_value(const char *ptr, int offset1, int offset2) const
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
	if (words_count != 1)
	{
		if (words_count == 2 && !strncmp(ptr + offset1, "error_pages", 11))
			return (index);
		throw invalid_syntax();
	}
	return (index);
}

// Location block syntax checkers
int	ConfigFileParser::location_syntax_checker(int start) const
{
	bool ret = false;

	if (!isblank(_buffer[start]))
		throw invalid_syntax();
	while (isspace(_buffer[start]))
		start++;
	while (_buffer[start] && !isspace(_buffer[start]))
	{
		start++;
		ret = true;
	}
	while (isspace(_buffer[start]))
		start++;
	if (_buffer[start] != '{')
		throw invalid_syntax();
	start++;
	start = location_tokens_syntax_checker(start);
	if (_buffer[start] != '}')
		throw invalid_syntax();
	return (start);
}

int ConfigFileParser::location_tokens_syntax_checker(int start) const
{
	while (_buffer[start])
	{
		if (isspace(_buffer[start]) && ++start)
			continue ;
		if (_buffer[start] == '}')
			return (start);
		if (_buffer[start] == '#')
		{
			start = strchr(_buffer.c_str() + start, '\n') - _buffer.c_str() + 1;
			continue ;
		}
		int temp = check_location_token(_buffer.c_str(), start);
		if (temp != start)
		{
			start = temp;
			continue ;
		}
		throw invalid_syntax();
	}
	return (start);
}

int	ConfigFileParser::check_location_token_value(const char *ptr, int offset1, int offset2) const
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
	if (words_count != 1)
	{
		if (words_count >= 1 && words_count <= 3 && !strncmp(ptr + offset1, "allowed_methods", 15))
			return (index);
		throw invalid_syntax();
	}
	return (index);
}

int ConfigFileParser::check_location_token(const char *ptr, int index) const
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
		std::cout << "Valid Syntax!" << std::endl;
		// filling_attributes();
	}
	catch (const std::exception &e)
	{
		std::cout << e.what() << std::endl;
		exit(1);
	}
}
