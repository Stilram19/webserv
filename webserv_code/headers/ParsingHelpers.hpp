/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParsingHelpers.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: obednaou <obednaou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/20 20:38:20 by obednaou          #+#    #+#             */
/*   Updated: 2023/07/20 20:41:00 by obednaou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSING_HELPERS_HPP
# define PARSING_HELPERS_HPP

class ParsingHelpers
{
	public:
		int skip_blank(const char *str, int start);
		int skip_spaces(const char *str, int start);
		
};

#endif
