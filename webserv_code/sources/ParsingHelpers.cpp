/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParsingHelpers.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: obednaou <obednaou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/20 20:37:48 by obednaou          #+#    #+#             */
/*   Updated: 2023/08/12 21:45:23 by obednaou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "ParsingHelpers.hpp"

int ParsingHelpers::skip_blank(const char *str, int start)
{
	while (isblank(str[start]))
		start++;
	return (start);
}

int	ParsingHelpers::skip_spaces(const char *str, int start)
{
	while (isspace(str[start]))
		start++;
	return (start);
}

bool	ParsingHelpers::is_unsigned_int(const std::string &str)
{
	if (str.empty())
		return (false);
	for (int i = 0; str[i]; i++)
	{
		if (!isdigit(str[i]))
			return (false);
	}
	return (true);
}

int	ParsingHelpers::my_stoi(const std::string &str)
{
	int					ret;
	std::stringstream	s;

	s << str;
	s >> ret;
	return (ret);
}

const std::string	ParsingHelpers::my_trim(const std::string &str)
{
	int	start, end;

	start = skip_blank(str.c_str(), 0);
	end = str.length() - 1;
	while (end > 0 && isblank(str[end]))
		end--;
	return (str.substr(start, end - start + 1));
}

std::string ParsingHelpers::skip_leading_zeros(const std::string &info)
{
	int start = 0;

	while (info[start] == '0')
		start++;
	return (info.c_str() + start);
}

char ParsingHelpers::get_hexa_remainder(char remainder)
{
	char decrement = 0;

	if (isdigit(remainder))
		decrement = '0';
	else if (islower(remainder))
		decrement = 'a' - 10;
	else if (isupper(remainder))
		decrement = 'A' - 10;
	return (remainder - decrement);
}

char ParsingHelpers::decode_percent_encoded_character(const char *hexa)
{
	char first = *hexa;
	char second = *(hexa + 1);

	first = get_hexa_remainder(first);
	second = get_hexa_remainder(second);
	return (first * 16 + second);
}

size_t ParsingHelpers::hexa_to_decimal(const char *hexa_str)
{
	size_t ret = 0;

	for (int i = 0; hexa_str[i]; i++)
	{
		ret *= 16;
		ret += get_hexa_remainder(hexa_str[i]);
	}
	return (ret);
}

int ParsingHelpers::skip_crlf(const char *temp)
{
    int i = 0;

    for (i = 0; temp[i]; i += 4)
    {
        if (strncmp(temp + i, "\r\n\r\n", 4))
            break ;
    }
    return (i);
}

bool ParsingHelpers::is_http_method(const std::string &input)
{
	return (input == "GET" || input == "POST" || input == "DELETE");
}

size_t ParsingHelpers::first_space(const char *str, size_t pos)
{
	while (!isspace(str[pos]))
		pos++;
	return (pos);
}

std::string	ParsingHelpers::get_file_extension(const std::string &file)
{
	size_t		extension_pos = file.rfind('.');
	std::string	extension = "";

	if (extension_pos != std::string::npos)
		extension = file.substr(extension_pos);
	return (extension);
}

void	ParsingHelpers::str_to_upper(std::string &str)
{
	for (int i = 0; str[i]; i++)
		str[i] = toupper(str[i]);
}

void	ParsingHelpers::str_tr(std::string &str, char old_c, char new_c)
{
	for (int i = 0; str[i]; i++)
		if (str[i] == old_c)
			str[i] = new_c;
}
