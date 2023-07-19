/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigFileParser.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: obednaou <obednaou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/16 15:11:29 by obednaou          #+#    #+#             */
/*   Updated: 2023/07/19 19:00:14 by obednaou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "ConfigFileParser.hpp"

// ******************* CONSTRUCTOR & DESTRUCTOR *******************

ConfigFileParser::ConfigFileParser(const char *filename) : _filename(filename)
{
	_server_tokens["listen"] = 6;
	_server_tokens["server_name"] = 11;
	_server_tokens["max_client_body_size"] = 20;
	_server_tokens["error_page"] = 10;

	_location_tokens["root"] = 4;
	_location_tokens["allowed_methods"] = 15;
	_location_tokens["redirect"] = 8;
	_location_tokens["index"] = 5;
	_location_tokens["directory_listing"] = 17;
	_location_tokens["upload_post"] = 11;
	_location_tokens["cgi"] = 3;
}

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
	for (std::map<std::string, int>::const_iterator it = _server_tokens.begin(); it != _server_tokens.end(); it++)
	{
		if (!strncmp(ptr + index, it->first.c_str(), it->second))
			return (check_server_token_value(ptr, index, it->second));
	}
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
	for (std::map<std::string, int>::const_iterator it = _location_tokens.begin(); it != _location_tokens.end(); it++)
	{
		if (!strncmp(ptr + index, it->first.c_str(), it->second))
			return (check_server_token_value(ptr, index, it->second));
	}
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
		index = extract_server_infos(index + 6);
	}
}

// Extracting server config infos
VirtualServer *ConfigFileParser::new_virtual_server()
{
	VirtualServer *new_vs = new VirtualServer();

	_VServers.push_back(new_vs);
	return (new_vs);
}

int ConfigFileParser::extract_server_infos(int index)
{
	VirtualServer *vs = new_virtual_server();

	index = _buffer.find('{', index) + 1;
	index = extract_server_token_values(vs, index);
	return (index + 1);
}

int	ConfigFileParser::extract_server_token_values(VirtualServer *vs, int index)
{
	while (_buffer[index])
	{
		index = skip_spaces(_buffer.c_str(), index);
		if (_buffer[index] == '}')
			break ;
		index = extract_server_token_value(vs, _buffer.c_str(), index);
		if (!strncmp(_buffer.c_str() + index, "location", 8))
			index = extract_location_infos(vs, index + 8);
	}
	return (index);
}

int	ConfigFileParser::extract_server_token_value(VirtualServer *vs, const char *ptr, int index)
{
	int end, temp;

	end = _buffer.find(ptr + index, '\n');
	temp = _buffer.find(ptr + index, '}');
	if (temp < end)
		end = temp;
	for (std::map<std::string, int>::const_iterator it = _server_tokens.begin(); it != _server_tokens.end(); it++)
	{
		const char *first = it->first.c_str();
		if (!strncmp(ptr + index, it->first.c_str(), it->second))
		{
			vs->set_server_info(it->first, _buffer.substr(index + it->second, end - index - it->second));
			return (end);
		}
	}
	return (index);
}

// Extracting Location config infos
int ConfigFileParser::extract_location_infos(VirtualServer *vs, int index)
{
	int			key_end;
	Location	*loc;

	index = skip_spaces(_buffer.c_str(), index);
	key_end = _buffer.find('{', index) - 1;
	loc = vs->new_location(_buffer.substr(index, key_end));
	index = key_end + 2;
	index = extract_location_token_values(vs, loc, index);
	return (index + 1);
}

int	ConfigFileParser::extract_location_token_values(VirtualServer *vs, Location *loc, int index)
{
	while (_buffer[index])
	{
		index = skip_spaces(_buffer.c_str(), index);
		if (_buffer[index] == '}')
			break ;
		index = extract_location_token_value(vs, loc, _buffer.c_str(), index);
	}
	return (index);
}

int	ConfigFileParser::extract_location_token_value(VirtualServer *vs, Location *loc, const char *ptr, int index)
{
	int end, temp;

	end = _buffer.find(ptr + index, '\n');
	temp = _buffer.find(ptr + index, '}');
	if (temp < end)
		end = temp;
	for (std::map<std::string, int>::const_iterator it = _server_tokens.begin(); it != _server_tokens.end(); it++)
	{
		const char *first = it->first.c_str();
		if (!strncmp(ptr + index, it->first.c_str(), it->second))
		{
			vs->set_server_info(it->first, _buffer.substr(index + it->second, end - index - it->second), loc);
			return (end);
		}
	}
	return (index);
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
