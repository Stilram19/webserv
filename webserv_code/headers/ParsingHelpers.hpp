/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParsingHelpers.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: obednaou <obednaou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/20 20:38:20 by obednaou          #+#    #+#             */
/*   Updated: 2023/07/21 19:01:59 by obednaou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSING_HELPERS_HPP
# define PARSING_HELPERS_HPP

# include "include.hpp"

class ParsingHelpers
{
	private:
		// Private Constructor (Static Class)
		ParsingHelpers();

	public:
		// Parsing Helpers

		static int 					skip_blank(const char *str, int start);
		static int 					skip_spaces(const char *str, int start);
		static bool					is_unsigned_int(const std::string &str);
		static int					my_stoi(const std::string &str);
		static const std::string	my_trim(const std::string &str);

		template <typename T1, typename T2>
		static void					display_pair(const std::pair<T1, T2> &pair)
		{
			std::cout << pair.first << ", " << pair.second << std::endl;
		}

		static std::string			skip_leading_zeros(const std::string &info);
};

#endif