/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FileHandler.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: obednaou <obednaou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/06 14:43:32 by obednaou          #+#    #+#             */
/*   Updated: 2023/08/11 16:51:15 by obednaou         ###   ########.fr       */
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
        static bool         is_directory(const char *path);
        static bool         is_regular_file(const char *path);
        static int          delete_file(const char *path);
        static int          delete_directory(const char *path);
        static int          random_file_name_generation(std::string &file_name, const std::string &root);
        static bool         is_resource_path_found(const char *path);
        static std::string  get_file_root(const std::string &full_file_path);
};

#endif
