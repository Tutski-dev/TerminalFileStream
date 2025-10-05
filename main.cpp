#include <boost/asio.hpp>
#include <iostream>
#include <fstream>

using boost::asio::ip::tcp;

int main(int argc, char** argv){
    if (argc == 1 or argc > 5){
        std::cerr << "USAGE: TerminalFileStream receive/send OPTION\nUse receive/send without options for more info\n";
        return 0;
    }
    if (argc == 2){
        if (strcmp(argv[1], "receive") == 0){
            std::cerr << "USAGE: TerminalFileStream receive PORT\nWait for connection on PORT and write data to stout\n";
        }
        else if (strcmp(argv[1], "send") == 0){
            std::cerr << "USAGE: TerminalFileStream send IP PORT FILE\nConnect to IP on PORT and send FILE\n";
        }
        else{
            std::cerr << "USAGE: TerminalFileStream receive/send OPTION\nUse receive/send without options for more info\n";
        }
        return 0;
    }
    if (strcmp(argv[1], "receive") == 0){
        if (argc != 3){
            std::cerr << "USAGE: TerminalFileStream receive PORT\nWait for connection on PORT and write data to stout\n";
            return 0;
        }
        int port;
        try{
            port = std::stoi(argv[2]);
        }
        catch(std::exception& e){
            std::cerr << "Bad PORT !\nUSAGE: TerminalFileStream receive PORT\nWait for connection on PORT and write data to stout\n";
        }

        boost::asio::io_context io;
        tcp::acceptor acceptor(io);
        try{
            acceptor = tcp::acceptor(io, tcp::endpoint(tcp::v4(), port));
        }
        catch(std::exception& e){
            std::cerr << "/!\\ Error opening port: " << e.what() << std::endl;
            return -1;
        }

        tcp::socket socket(io);
        acceptor.accept(socket);
        try{
            int fileSize;
            socket.receive(boost::asio::buffer(&fileSize, sizeof(int)));
            int bytes_read = 0;
            while (bytes_read < fileSize){
                uint8_t byte;
                socket.receive(boost::asio::buffer(&byte, 1));
                std::cout.write((char*)&byte, 1);
                bytes_read++;
            }

        }
        catch(std::exception& e){
            std::cerr << "/!\\ Error while receiving data: " << e.what() << std::endl;
            return -1;
        }

    }
    else if (strcmp(argv[1], "send") == 0){
        if (argc != 5){
            std::cerr << "USAGE: TerminalFileStream send IP PORT FILE\nConnect to IP on PORT and send FILE\n";
            return 0;
        }
        boost::asio::io_context io;

        int port;
        try{
            port = std::stoi(argv[3]);
        }
        catch(std::exception& e){
            std::cerr << "Bad PORT !\nUSAGE: TerminalFileStream send IP PORT FILE\nConnect to IP on PORT and send FILE\n";
        }

        tcp::socket socket(io);
        tcp::resolver resolver(io);
        tcp::resolver::results_type endpoints;

        std::string address = argv[2];

        try{
            endpoints = resolver.resolve(address, argv[3]);
            boost::asio::connect(socket, endpoints);
        }
        catch(std::exception& e){
            std::cerr << "Error while connecting ! : " << e.what() << std::endl;
        }

        std::ifstream file;
        try{
            file.open(argv[4], std::ios::binary);
        }
        catch(std::exception& e){
            std::cerr << "Error loading file ! : " << e.what() << std::endl;
        }
        try{
            file.seekg(0, std::ios::end);
            int fileSize = file.tellg();
            file.seekg(0);
            socket.send(boost::asio::buffer(&fileSize, sizeof(int)));
            int percentage_1 = (int)fileSize/100;
            int bytes_sent = 0;
            int percentage = 0;
            while(1){
                uint8_t byte = file.get();
                if (file.eof()){
                    break;
                }
                socket.send(boost::asio::buffer(&byte, sizeof(byte)));
                bytes_sent++;
                if (bytes_sent % percentage_1 == 0){
                    std::cout << percentage << "%" << std::endl;
                    percentage++;
                }
            }
        }
        catch(std::exception& e){
            std::cerr << "Error sending file ! : " << e.what() << std::endl;
        }


    }
    else{
        std::cerr << "USAGE: TerminalFileStream receive/send OPTION\nUse receive/send without options for more info\n";
        return 0;
    }
}
