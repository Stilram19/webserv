/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigFileParser.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: obednaou <obednaou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/16 15:11:29 by obednaou          #+#    #+#             */
/*   Updated: 2023/07/19 13:01:46 by obednaou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "ConfigFileParser.hpp"

// ******************* CONSTRUCTOR & DESTRUCTOR *******************

ConfigFileParser::ConfigFileParser(const char *filename) : _filename(filename) {}

ConfigFileParser::~ConfigFileParser()
{
	for (std::vector<VirtualServer *>::iterator it = _VServers.begin(); it != _VServers.end(); it++)
		delete *it;
	_VServers.clear();
}

// ******************* HELPER METHODS *******************

int ConfigFileParser::skip_blank(const char *ptr, int start) const
{
	while (isblank(ptr[start]))
		start++;
	return (start);
}

int	ConfigFileParser::skip_spaces(const char *ptr, int start) const
{
	while (isspace(ptr[start]))
		start++;
	return (start);
}

int ConfigFileParser::count_and_skip_words(const char *ptr, int *index_ptr) const
{
	int index = *index_ptr;
	int	words_count = 0;

	while (true)
	{
		index = skip_blank(ptr, index);
		if (ptr[index] == '\n' || ptr[index] == '{' || ptr[index] == '}')
			break ;
		words_count++;
		while (ptr[index] != '{' && ptr[index] != '}' && !isspace(ptr[index]))
			index++;
	}
	*index_ptr = index;
	return (words_count);
}

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
		//std::cout << _buffer[index] << std::endl;
	}
}

// Server Block Syntax checkers
int	ConfigFileParser::server_block_syntax_checker(int start) const
{
	if (strncmp(_buffer.c_str() + start, "server", 6))
		throw invalid_syntax();
	start += 6;
	start = skip_spaces(_buffer.c_str(), start);
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
			const char *temp = strchr(_buffer.c_str() + start, '\n');

			start = temp - _buffer.c_str() + 1;
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
	if (!strncmp(ptr + index, "error_page", 10))
		return (check_server_token_value(ptr, index, 10));
	return (index);
}

int	ConfigFileParser::check_server_token_value(const char *ptr, int offset1, int offset2) const
{
	int	words_count = 0;
	int	index = offset1 + offset2;

	if (!isblank(ptr[index]))
		throw invalid_syntax();
	words_count = count_and_skip_words(ptr, &index);
	if (ptr[index] == '{')
		throw invalid_syntax();
	if (!strncmp(ptr + offset1, "error_page", 10))
	{
		if (words_count == 2)
			return (index);
		throw invalid_syntax();
	}
	if (words_count != 1)
		throw invalid_syntax();
	return (index);
}

// Location block syntax checkers
int	ConfigFileParser::location_syntax_checker(int start) const
{
	if (!isblank(_buffer[start]))
		throw invalid_syntax();
	start = skip_spaces(_buffer.c_str(), start);
	if (_buffer[start] == '{')
		throw invalid_syntax();
	while (_buffer[start] && !isspace(_buffer[start]) && _buffer[start] != '{' && _buffer[start] != '}')
		start++;
	start = skip_spaces(_buffer.c_str(), start);
	if (_buffer[start] != '{')
		throw invalid_syntax();
	start++;
	start = location_tokens_syntax_checker(start);
	if (_buffer[start] != '}')
		throw invalid_syntax();
	return (start + 1);
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
			const char *temp = strchr(_buffer.c_str() + start, '\n');

			start = temp - _buffer.c_str() + 1;
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

int ConfigFileParser::check_location_token(const char *ptr, int index) const
{
	if (!strncmp(ptr + index, "root", 4))
		return (check_location_token_value(ptr, index, 4));
	if (!strncmp(ptr + index, "allowed_methods", 15))
		return (check_location_token_value(ptr, index, 15));
	if (!strncmp(ptr + index, "redirect", 8))
		return (check_location_token_value(ptr, index, 8));
	if (!strncmp(ptr + index, "index", 5))
		return (check_location_token_value(ptr, index, 5));
	if (!strncmp(ptr + index, "directory_listing", 17))
		return (check_location_token_value(ptr, index, 17));
	if (!strncmp(ptr + index, "upload_post", 11))
		return (check_location_token_value(ptr, index, 11));
	if (!strncmp(ptr + index, "cgi", 3))
		return (check_location_token_value(ptr, index, 3));
	return (index);
}

int	ConfigFileParser::check_location_token_value(const char *ptr, int offset1, int offset2) const
{
	int	words_count = 0;
	int	index = offset1 + offset2;

	if (!isblank(ptr[index]))
		throw invalid_syntax();
	words_count = count_and_skip_words(ptr, &index);
	if (ptr[index] == '{')
		throw invalid_syntax();
	if (!strncmp(ptr + offset1, "cgi", 3))
	{
		if (words_count == 2)
			return (index);
		// std::cout << "HERE: " << words_count << std::endl;
		throw invalid_syntax();
	}
	if (!strncmp(ptr + offset1, "allowed_methods", 15))
	{
		if (words_count >= 1 && words_count <= 3)
			return (index);
		throw invalid_syntax();
	}
	if (words_count != 1)
		throw invalid_syntax();
	return (index);
}

// ******************* EXTRACTING CONFIG INFOS *******************


// Extracting config infos Main Method
void	ConfigFileParser::extracting_config_infos()
{
	int index = 0;

	while (_buffer[index])
	{
		if (isspace(_buffer[index]) && ++index)
			continue ;
		index = extract_server_tokens(index + 6);
	}
}

VirtualServer *ConfigFileParser::new_virtual_server()
{
	VirtualServer *new_vs = new VirtualServer();

	_VServers.push_back(new_vs);
	return (new_vs);
}

int ConfigFileParser::extract_server_tokens(int index)
{
	VirtualServer *vs = new_virtual_server();

	index = _buffer.find('{', index) + 1;
	index = skip_spaces(_buffer.c_str(), index);
	index = extract_and_set_attributes(vs, index);
	return (index + 1);
}

int	ConfigFileParser::extract_and_set_attributes(VirtualServer *vs, int index)
{
	while (_buffer[index])
	{
		index = skip_spaces(_buffer.c_str(), index);
		if (_buffer[index] == '}')
			return (index);
		index = extract_token(vs, _buffer.c_str(), index);
	}
}

int	ConfigFileParser::extract_token(VirtualServer *vs, const char *ptr, int index)
{
	const char *last = strchr();
	if (!strncmp(ptr + index, "listen", 6))
		return (set_server_token_value("listen", ptr + index + 6));
	if (!strncmp(ptr + index, "server_name", 11))
		return (set_server_token_value("server_name", ptr + index + 11));
	if (!strncmp(ptr + index, "max_client_body_size", 20))
		return (set_server_token_value("max_client_body_size", ptr + index + 20));
	if (!strncmp(ptr + index, "error_page", 10))
		return (set_server_token_value("error_page", ptr + index + 10));
	return (index);
}

int	ConfigFileParser::set_server_token_value(const std::string &token_type, const std::string &token_value)
{
	
}

// ******************* PARSER MAIN METHOD *******************

void	ConfigFileParser::config_file_parsing()
{
	try
	{
		buffering_input_file();
		syntax_checker();
		//extracting_config_infos();
		std::cout << "Valid Syntax!" << std::endl;
	}
	catch (const std::exception &e)
	{
		std::cout << e.what() << std::endl;
		exit(1);
	}
}
