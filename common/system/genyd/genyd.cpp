
#include <algorithm>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <strings.h>

#include "genyd.hpp"

// Not defined for Android 2.3
#ifndef INET_ADDRSTRLEN
#define INET_ADDRSTRLEN 16
#endif

Genyd::Genyd(void):
    server(NULL),
    clipboardProxy(NULL)
{
    int server_sock = -1;
    struct sockaddr_in server_addr;

    bzero(&server_addr, sizeof(server_addr));

    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);

    if((server_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        SLOGE("Cannot create socket");
        return;
    }

    if (bind(server_sock, (sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        SLOGE("Cannot bind socket");
        return;
    }

    if (listen(server_sock, 5) == -1) {
        SLOGE("Cannot listen on socket");
        return;
    }
    server = new Socket(server_sock);
}

Genyd::~Genyd(void)
{
    if (server) {
        delete server;
    }

    std::map<int, Socket*>::iterator begin = clients.begin();
    std::map<int, Socket*>::iterator end = clients.end();

    while (begin != end) {
        delete begin->second;
        ++begin;
    }
}

int Genyd::setFS(fd_set *readfs, fd_set *writefs) const
{
    int highest = server->getFD();

    std::map<int, Socket*>::const_iterator begin = clients.begin();
    std::map<int, Socket*>::const_iterator end = clients.end();

    FD_ZERO(readfs);
    FD_ZERO(writefs);

    // For "accept()"
    FD_SET(server->getFD(), readfs);

    while (begin != end) {
        FD_SET(begin->first, readfs);
        if (begin->second->hasReplies() || begin->second->hasRequests()) {
            FD_SET(begin->first, writefs);
        }
        highest = std::max(highest, begin->first);
        ++begin;
    }

    return highest;
}

void Genyd::acceptNewClient(void)
{
    int client = -1;
    sockaddr_in clientAddr;
    int clientAddrSize = sizeof(clientAddr);

    client = accept(server->getFD(), (sockaddr *)&clientAddr, &clientAddrSize);

    if (client == -1) {
        SLOGE("Cannot accept connection");
        return;
    }

    clients[client] = new Socket(client);

    // Detect if the connection comes from clipboardProxy
    char clientIp[INET_ADDRSTRLEN];
    if(inet_ntop(PF_INET, &(clientAddr.sin_addr), clientIp, INET_ADDRSTRLEN) != NULL) {
        if(strcmp(clientIp, "127.0.0.1") == 0) {
            clipboardProxy = clients[client];
            if(!clipboard.empty()) {
                // If the clipboard was set before clipboardProxy connect,
                // send it the value we saved before
                clipboardProxy->write(clipboard.c_str(), clipboard.size());
            }
        }
    }
}

void Genyd::treatMessage(Socket *client)
{
    const Request &request = client->getRequest();
    client->addReply(dispatcher.dispatchRequest(request, this));
}

void Genyd::sendAndroidClipboardToHost(void)
{
    if (!clipboardProxy) {
        return;
    }

    std::string clipboardText = "";

    Socket::ReadStatus status = clipboardProxy->read(&clipboardText);

    switch (status) {
    case Socket::ReadError:
        SLOGE("clipboardProxy read error");
        break;
    case Socket::NoMessage:
        clients.erase(clipboardProxy->getFD());
        delete clipboardProxy;
        clipboardProxy = NULL;
        break;
    case Socket::NewMessage: {
        // Broadcast message to all clients
        std::map<int, Socket *>::iterator begin = clients.begin();
        std::map<int, Socket *>::iterator end = clients.end();
        while (begin != end) {
            if (begin->second != clipboardProxy) {
                Request *request = new Request();
                request->set_type(Request::PushData);
                Parameter *parameter = request->mutable_parameter();
                parameter->set_type(Parameter::Clipboard);
                Value *value = parameter->mutable_value();
                value->set_type(Value::String);
                value->set_stringvalue(clipboardText);
                begin->second->addRequest(request);
            }
            ++begin;
        }
        break;
    }
    case Socket::UnknownMessage:
    default:
        break;
    }
}

void Genyd::run(void)
{
    fd_set readfs;
    fd_set writefs;

    // Server loop
    while (true) {
        int highest = setFS(&readfs, &writefs);

        // Wait for operations
        if (select(highest + 1, &readfs, &writefs, NULL, NULL) < 0) {
            SLOGE("select() error");
            return;
        }

        // Accept new connection
        if (FD_ISSET(server->getFD(), &readfs)) {
            acceptNewClient();
        }

        std::map<int, Socket *>::iterator begin = clients.begin();
        std::map<int, Socket *>::iterator end = clients.end();

        // Handle messages for clients
        while (begin != end) {
            // Ready read
            if (FD_ISSET(begin->first, &readfs)) {
                if (begin->second == clipboardProxy) {
                    sendAndroidClipboardToHost();
                    ++begin;
                    // Next client
                    continue;
                }
                else {
                    Socket::ReadStatus status = begin->second->read();
                    switch (status) {
                    case Socket::ReadError:
                        SLOGE("Socket read error");
                        break;
                    case Socket::NoMessage:
                        delete begin->second;
                        if (begin->second == clipboardProxy) {
                            clipboardProxy = NULL;
                        }
                        clients.erase(begin++);
                        SLOGD("Socket deconnection");
                        // Next client
                        continue;
                    case Socket::NewMessage:
                        treatMessage(begin->second);
                        break;
                    case Socket::UnknownMessage:
                        break;
                    default:
                        SLOGE("Unknown socket status");
                        break;
                    }
                }
            }

            // Ready write
            if (FD_ISSET(begin->first, &writefs)) {
                Socket::WriteStatus status = Socket::WriteSuccess;

                // Higher priority for replies
                if (begin->second->hasReplies()) {
                    status = begin->second->reply();
                } else if (begin->second->hasRequests()) {
                    status = begin->second->ask();
                } else {
                    // Nothing
                }

                if (status == Socket::WriteError) {
                    SLOGD("Socket write error with client %d", begin->first);
                    delete begin->second;
                    clients.erase(begin++);
                    // Next client
                    continue;
                }
            }

            // Next client
            ++begin;
        }
    }
}

bool Genyd::isInit(void) const
{
    return (server);
}

Socket *Genyd::getClipboardClient(void)
{
    return clipboardProxy;
}

void Genyd::storeClipboard(const std::string &clipboard)
{
    this->clipboard = clipboard;
}
