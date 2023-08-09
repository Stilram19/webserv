/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FileHandler.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: obednaou <obednaou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/06 14:43:32 by obednaou          #+#    #+#             */
<<<<<<< HEAD
/*   Updated: 2023/08/06 15:11:46 by obednaou         ###   ########.fr       */
=======
/*   Updated: 2023/08/08 18:14:20 by obednaou         ###   ########.fr       */
>>>>>>> c3dda2ce8d1438e118cfb560dd70e7e11bb048a4
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
<<<<<<< HEAD
        static bool is_file(const char *path);
        static void delete_file(const char *path);
        static void delete_directory_content(const char *path);
=======
        static bool is_regular_file(const char *path);
        static void delete_file(const char *path);
        static void delete_directory_content(const char *path);
        static int  random_file_name_generation(std::string &file_name);
>>>>>>> c3dda2ce8d1438e118cfb560dd70e7e11bb048a4
};

#endif
