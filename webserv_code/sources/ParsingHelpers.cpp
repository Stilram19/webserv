/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParsingHelpers.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: obednaou <obednaou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/20 20:37:48 by obednaou          #+#    #+#             */
/*   Updated: 2023/07/21 10:01:34 by obednaou         ###   ########.fr       */
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
	end = start;
	while (!isblank(str[end]))
		end++;
	return (str.substr(start, end - start));
}
