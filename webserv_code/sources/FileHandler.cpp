/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FileHandler.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: obednaou <obednaou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/06 15:11:20 by obednaou          #+#    #+#             */
/*   Updated: 2023/08/06 19:12:38 by obednaou         ###   ########.fr       */
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

bool FileHandler::is_file(const char *path)
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
        std::cerr << "Can't open directory stream!" << std::endl;
        return (ERROR);
    }

    // Iterating through the entries
    struct dirent *entry;
    std::string entry_full_name;

    while ((entry = readdir(dir_stream)) != NULL)
    {
        if (!strcmp(entry, ".") || !strcmp(entry, ".."))
            continue ;
        entry_full_name += path;
        entry_full_name += entry->d_name;
        if (is_directory(entry_full_name.c_str()))
        {
            if (!delete_directory_content(entry_full_name.c_str()))
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
