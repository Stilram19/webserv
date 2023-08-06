/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FileHandler.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: obednaou <obednaou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/06 14:43:32 by obednaou          #+#    #+#             */
/*   Updated: 2023/08/06 15:11:46 by obednaou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FILEHANDLER_HPP
# define FILEHANDLER_HPP

# include "include.hpp"

class FileHandler
{
    private:
        // Private Constructor (Static class)
        FileHandler();

    public:
        static bool is_directory(const char *path);
        static bool is_file(const char *path);
        static void delete_file(const char *path);
        static void delete_directory_content(const char *path);
};

#endif
