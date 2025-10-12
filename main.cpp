#include <boost/asio.hpp>
#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>
#include <mutex>

using boost::asio::ip::tcp;
std::mutex mtx;

void show_progress(uint64_t* bytes_sent, uint64_t* bytes_sent_during_second, uint64_t* fileSize){
    std::chrono::time_point second_start = std::chrono::steady_clock::now();
    while (*bytes_sent < *fileSize){
        if (std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - second_start).count() == 10){
            mtx.lock();
            second_start = std::chrono::steady_clock::now();
            double current_percentage = 100 * (double)*bytes_sent/ *fileSize;
            std::cout << current_percentage << "%" <<  " speed: " << (double)*bytes_sent_during_second/1000/10 << " kB/s" << std::endl;
            *bytes_sent_during_second = 0;
            mtx.unlock();
        }
    }

}

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
            uint64_t fileSize;
            socket.receive(boost::asio::buffer(&fileSize, sizeof(uint64_t)));
            uint64_t bytes_read = 0;
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
            uint64_t fileSize = file.tellg();
            std::cout << fileSize << " Bytes to send" << std::endl;
            file.seekg(0);
            socket.send(boost::asio::buffer(&fileSize, sizeof(uint64_t)));
            std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
            std::chrono::steady_clock::time_point second_start = std::chrono::steady_clock::now();
            boost::
            uint64_t bytes_sent = 0;
            uint64_t bytes_sent_during_second = 0;
            std::thread thread(show_progress, &bytes_sent, &bytes_sent_during_second, &fileSize);
            while(bytes_sent < fileSize){
                uint8_t byte = file.get();
                socket.send(boost::asio::buffer(&byte, sizeof(byte)));
                mtx.lock();
                bytes_sent++;
                bytes_sent_during_second++;
                mtx.unlock();

            }
            std::cout << "Finished in " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count() << " !" << std::endl;
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
