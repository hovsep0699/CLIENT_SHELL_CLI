#include "command.h"
#include <fstream>
#include <thread>
#include <mutex>
#include <stdio.h>

namespace command
{
    invalid_arguments::invalid_arguments( const char* message )
        :   m_message(message)
    {}

    const char* invalid_arguments::what() const noexcept
    {
        return m_message;
    }

    command::command( std::shared_ptr<network::connection_socket> server )
    {
        if ( !server )
            throw std::runtime_error("Client not connected to server");
        m_server = server;
    }

    disconnect::disconnect( std::shared_ptr<network::connection_socket> server )
        :   command(server)
    {}

    void disconnect::validate( std::shared_ptr<network::client> client, const argument_list& arguments )
    {
        if ( arguments.size() )
            throw invalid_arguments("Usgae:\tdisconnect");
    }


    void disconnect::execute( std::shared_ptr<network::client> client, const argument_list& arguments, bool& /* connected */ )
    {
        if ( !client )
            throw std::runtime_error("Client is not connected");
        network::log("Client disconnected from server");
        m_server->get_disconnection( client->get_fd() );
    }

    shell::shell( std::shared_ptr<network::connection_socket> server )
        :   command(server)
    {}

    void shell::validate( std::shared_ptr<network::client> client, const argument_list& arguments )
    {
        if ( arguments.size() < 1 )
            throw invalid_arguments("Usage:\tshell <cmd>");
    }

    void shell::execute( std::shared_ptr<network::client> client, const argument_list& arguments, bool& connected )
    {

        //std::cout << "shell$> execute" << std::endl;
        auto send_line = [&connected]( int fd, std::string msg )
        {
            if ( connected )
            {
                auto all_bytes = msg.size();
                char        buffer[BUFFER_SIZE];

                bzero(buffer, BUFFER_SIZE);

                int bytes = 0;
                int sended_bytes = 0;
                while ( bytes < all_bytes )
                {
                    bzero(buffer, BUFFER_SIZE);
                    int n = sprintf( buffer, "%.*s", BUFFER_SIZE - sended_bytes, msg.c_str() );
                    msg.erase(0, BUFFER_SIZE - 1);
                    sended_bytes = send( fd, buffer, BUFFER_SIZE - sended_bytes, 0 );
                    if ( sended_bytes < 0 )
                        throw std::runtime_error("Cannot send data to server");
                    bytes += sended_bytes;
                }
            }
        };
        if ( !client )
            throw std::runtime_error("Client is not connected to server");
        std::string command;
        for (auto& arg : arguments)
            command += arg + std::string(" ");
        command += "2>&1";
        FILE* pipe;
        char buffer[BUFFER_SIZE];

        // Execute the command and get the output
        pipe = popen( command.c_str(), "r" );
        if ( !pipe )
            throw std::runtime_error("Error while execute shell command");

        std::string output;
        // Read the output from the command
        bzero(buffer, BUFFER_SIZE);
        while ( fgets( buffer, BUFFER_SIZE - 1, pipe ) )
        {
            output.append(buffer);
            bzero(buffer, BUFFER_SIZE);
        }
        pclose( pipe );

        output += "\r";
        send_line( client->get_fd(), output );
    }

    connect::connect( std::shared_ptr<network::connection_socket> server )
        :   command(server)
    {}

    void connect::validate( std::shared_ptr<network::client> client, const argument_list& arguments )
    {
        if ( arguments.size() != 2 )
            throw invalid_arguments("Usage:\tconnect <IP_ADDRESS> <PORT>");
    }

    void connect::execute( std::shared_ptr<network::client> client, const argument_list& arguments, bool& /* connected */ )
    {
        if ( !client )
            throw std::runtime_error("Client is not connected to server");
        m_server->connect( arguments[1], arguments[0] );
        network::log("Client is connected to server");
    }


}




