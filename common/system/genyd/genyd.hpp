#ifndef GENYD_HPP_
#define GENYD_HPP_

#include <map>

#include <unistd.h>

#include "global.hpp"
#include "socket.hpp"
#include "dispatcher.hpp"

#define SERVER_PORT 22666

class Genyd
{

public:
    Genyd(void);
    ~Genyd(void);

private:
    Genyd(const Genyd &);
    Genyd operator=(const Genyd &);

private:
    Socket *server;
    Dispatcher dispatcher;

    // Clients list
    std::map<int, Socket *> clients;

    // socket to the clipboardProxy service
    // this socket is also present in the clients list
    Socket *clipboardProxy;

    // Keep clipboard content
    std::string clipboard;

    // Initialize fd_set for select() monitoring
    int setFS(fd_set *readfs, fd_set *writefs) const;

    // Accept a new connection
    void acceptNewClient(void);

    // Handle Socket::read status for a given client
    void treatMessage(Socket *client);

    void sendHostClipboardToAndroid(const Request &request);
    void sendAndroidClipboardToHost();

public:
    // Start server
    void run(void);

    // Check if the server is well initialized
    bool isInit(void) const;

    // Retreive the connection to the ClipboardService
    Socket *getClipboardClient(void);

    // Store the clipboard
    void storeClipboard(const std::string &clipboard);

};

#endif
