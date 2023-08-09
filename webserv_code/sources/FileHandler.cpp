/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FileHandler.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: obednaou <obednaou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/06 15:11:20 by obednaou          #+#    #+#             */
/*   Updated: 2023/08/09 22:06:39 by obednaou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "FileHandle.hpp"

bool    FileHandler::is_directory(const char *path)
{
    struct stat file_info;

    if (stat(path, &file_info))
        return (false);
    return (S_ISDIR(file_info.st_mode));
}

bool    FileHandler::is_regular_file(const char *path)
{
    struct stat file_info;

    if (stat(path, &file_info))
        return (false);
    return (S_ISREG(file_info.st_mode));
}

int     FileHandler::delete_file(const char *path)
{
    if (access(path, W_OK))
        return (PERMISSION_DENIED);
    if (unlink(path))
        return (ERROR);
    return (SUCCESS);
}

int    FileHandler::delete_directory_content(const char *path)
{
    // Opening the directory and pointing to the stream object, which is pointing on the first entry.
    DIR *dir_stream = opendir(path);

    if (dir_stream == NULL)
    {
        std::cerr << "Can't open directory!" << std::endl;
        return (ERROR);
    }

    // Checking directory permissions

    if (access(path, R_OK | W_OK))
        return (PERMISSION_DENIED);

    // Iterating through the entries
    struct dirent *entry;
    std::string entry_full_name;

    while ((entry = readdir(dir_stream)) != NULL)
    {
        if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, ".."))
            continue ;
        entry_full_name = path;
        if (entry_full_name[entry_full_name.length() - 1] != '/')
            entry_full_name += '/';
        entry_full_name += entry->d_name;
        if (is_directory(entry_full_name.c_str()))
        {
            int delete_status = delete_directory_content(entry_full_name.c_str());

            if (delete_status)
            {
                closedir(dir_stream);
                return (delete_status);
            }
        }
        else if (is_file(entry_full_name.c_str()))
            unlink(entry_full_name.c_str());
    }

    // Closing the open file (freeing the resources allocated for the stream object)
    closedir(dir_stream);

    // deleting the directory
    return (rmdir(path));
}

int FileHandler::random_file_name_generation(std::string &file_name, std::string &root)
{
    int j = 0, read_bytes = 0;
    int fd = open("/dev/random", O_RDONLY);
    char buffer[51];

    if (fd == -1)
        return (ERROR);

    // specifying the directory
    file_name = root;

    if (file_name.back() != '/')
        file_name += '/';

    // generating random name
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

bool FileHandler::is_resource_path_found(const char *path)
{
    if (FileHandler::is_regular_file(path))
        return (true);
    if (FileHandler::is_directory(path))
        return (true);
    return (false);
}
