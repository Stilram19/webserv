/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: obednaou <obednaou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/05 19:23:05 by obednaou          #+#    #+#             */
/*   Updated: 2023/08/13 11:54:16 by obednaou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include "include.hpp"

class Request;
class VirtualServer;
class Location;

class Response
{
    public:
        // Types
        typedef void (Response::*PtrToResponseStation)();
        typedef void (Response::*PtrToMethodHandler)();
    private:
        // Attributes
        bool                                        _is_there_index;
        bool                                        _cgi_flag;
        bool                                        _is_response_header_sent;
        int                                         _status;
        int                                         _temporary_storage_type;
        int                                         _handling_station;
        pid_t                                       _cgi_pid;
        int                                         _body_fd;
        e_status_code                               _status_code;
        int                                         _client_socket;
        size_t                                      _cgi_start_time;
        Request                                     *_request;
        VirtualServer                               *_VServer;
        Location                                    *_location;
        std::string                                 _request_method;
        std::string                                 _resource_physical_path;
        std::string                                 _resource_logical_path;
        std::string                                 _response_body_file_name;
        std::string                                 _index_logical_path;
        std::string                                 _index_physical_path;
        std::string                                 _body_buffer;
        std::string                                 _redirection;
        std::string                                 _request_body_file_path;
        std::string                                 _response_header_buffer;
        std::vector<std::string>                    _cgi_env_vector;
        std::map<std::string, std::string>          _response_headers;
        std::map<int, PtrToResponseStation>         _stations;
        std::map<std::string, PtrToMethodHandler>   _methods_handlers;
        std::map<e_status_code, std::string>        _status_code_pages;
        std::map<e_status_code, std::string>        _status_code_messages;
        std::map<std::string, std::string>          _content_types;

        // Useless Constructors & Copy Assignment
        Response();
        Response(const Response &);
        Response &operator=(const Response &);

    public:
        // Constructor & Destructor
        Response(int client_socket, Request *request);
        ~Response();

    private:
        // Helpers
        bool                is_there_index_file() const;
        bool                is_directory_listing_on() const;
        void                extracting_index_file();
        void                redirect_cgi_input() const;
        void                redirect_cgi_output();
        const std::string   &get_script_name() const;
        const std::string   &get_path_info() const;
        const std::string   &get_script_filename() const;
        void                regular_file_handler(const std::string &regular_file);
        void                cgi_environment_setup();
        std::string         get_connection() const;
        bool                is_status_code_message(const std::string &status_code_message) const;
        void                extract_cgi_response_headers();
        std::string         get_content_type() const;
        std::string         get_content_length();
        void                cgi_headers_parsing(const std::string &cgi_header_buffer, size_t start);
        void                send_ram_buffer(const std::string &buffer);
        void                send_body_file_chunk();
        bool                is_there_index() const;

    public:
        // Getters
        int     get_status() const;

    private:
        // Response Construction
        void    respond();
        void    produce_response_header();

    private:
        // Body Producers
        void    produce_html_for_status_code();
        void    produce_html_for_directory_listing();
        void    cgi(const std::string &script_path, const std::string &cgi_interpreter);

    private:
        // Methods Handlers
        void    get_handler();
        void    post_handler();
        void    delete_handler();

    private:
        // Response stations
        void    main_processing();
        void    wait_for_cgi();
        void    response_sending();

    public:
        // Main Function
        void    response_handling();
};

# include "Request.hpp"
# include "Location.hpp"
# include "VirtualServer.hpp"

#endif
