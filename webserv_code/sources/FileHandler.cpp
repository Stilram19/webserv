/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FileHandler.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: obednaou <obednaou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/06 15:11:20 by obednaou          #+#    #+#             */
<<<<<<< HEAD
/*   Updated: 2023/08/06 19:12:38 by obednaou         ###   ########.fr       */
=======
/*   Updated: 2023/08/08 18:14:06 by obednaou         ###   ########.fr       */
>>>>>>> c3dda2ce8d1438e118cfb560dd70e7e11bb048a4
/*                                                                            */
/* ************************************************************************** */

# include "FileHandle.hpp"

bool FileHandler::is_directory(const char *path)
{
    struct stat file_info;

    if (stat(path, &file_info))
        return (false);
    return (S_ISDIR(file_info.st_mode));
}

<<<<<<< HEAD
bool FileHandler::is_file(const char *path)
=======
bool FileHandler::is_regular_file(const char *path)
>>>>>>> c3dda2ce8d1438e118cfb560dd70e7e11bb048a4
{
    struct stat file_info;

    if (stat(path, &file_info))
        return (false);
    return (S_ISREG(file_info.st_mode));
}

void FileHandler::delete_file(const char *path)
{
    if (unlink(path))
        std::cerr << "unlink filed!" << std::endl;
}

bool    FileHandler::delete_directory_content(const char *path)
{
    // Opening the directory and pointing to the stream object, which is pointing on the first entry.
    DIR *dir_stream = opendir(path);

    if (dir_stream == NULL)
    {
<<<<<<< HEAD
        std::cerr << "Can't open directory stream!" << std::endl;
        return (ERROR);
=======
        std::cerr << "Can't open directory!" << std::endl;
        return (false);
>>>>>>> c3dda2ce8d1438e118cfb560dd70e7e11bb048a4
    }

    // Iterating through the entries
    struct dirent *entry;
    std::string entry_full_name;

    while ((entry = readdir(dir_stream)) != NULL)
    {
<<<<<<< HEAD
        if (!strcmp(entry, ".") || !strcmp(entry, ".."))
            continue ;
        entry_full_name += path;
        entry_full_name += entry->d_name;
        if (is_directory(entry_full_name.c_str()))
        {
            if (!delete_directory_content(entry_full_name.c_str()))
=======
        if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, ".."))
            continue ;
        entry_full_name = path;
        if (entry_full_name[entry_full_name.length() - 1] != '/')
            entry_full_name += '/';
        entry_full_name += entry->d_name;
        if (is_directory(entry_full_name.c_str()))
        {
            if (delete_directory_content(entry_full_name.c_str()))
>>>>>>> c3dda2ce8d1438e118cfb560dd70e7e11bb048a4
            {
                closedir(dir_stream);
                return (ERROR);
            }
            rmdir(entry_full_name.c_str());
        }
        else if (is_file(entry_full_name.c_str()))
            unlink(entry_full_name.c_str());
    }

    // Closing the open file (freeing the resources allocated for the stream object)
    closedir(dir_stream);

    // deleting the directory
    return (rmdir(path));
}
<<<<<<< HEAD
=======

int FileHandler::random_file_generation(std::string &file_name)
{
    int j = 0, read_bytes = 0;
    int fd = open("/dev/random", O_RDONLY);

    if (fd == -1)
        return (ERROR);
    char buffer[51];

    //file_name = "/tmp/";
    for (int i = 0; i < 14; i++)
    {
        read_bytes = read(fd, buffer, 50);

        if (read_bytes == -1)
            return (ERROR);
        for (j = 0; j < read_bytes; j++)
        {
            if (isalnum(buffer[j]))
            {
                file_name += buffer[j];
                break ;
            }
        }
        if (j == read_bytes)
            i--;
    }

    // Closing /dev/random fd
    close(fd);
    return (SUCCESS);
}
>>>>>>> c3dda2ce8d1438e118cfb560dd70e7e11bb048a4
