/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: obednaou <obednaou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/16 16:22:34 by obednaou          #+#    #+#             */
/*   Updated: 2023/07/16 17:38:44 by obednaou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATION_HPP
# define LOCATION_HPP

# include "GlobalHeader.hpp"

class Location
{
	private:
		// Attributes
		bool		_directory_listing;
		std::string _redirect_path;
		std::string _location_match;
		std::string _root_path;
		std::string _index_path;
		std::vector<std::string> _allowed_http_methods;

		// Useless Constructors & Copy Assignment
		Location();
		Location(const Location &);
		Location &operator=(const Location &);
	public:
		// Constructor & Destructor
				
};

#endif
