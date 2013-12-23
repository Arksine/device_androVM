
#include <algorithm>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <strings.h>

#include "genyd.hpp"

#define SERVER_PORT 22666

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
        SLOGD("New connection from %s", clientIp);
        if(strcmp(clientIp, "127.0.0.1") == 0) {
            SLOGD("clipboardProxy connection", clientIp);
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
    sendHostClipboardToAndroid(request);
    client->addReply(dispatcher.dispatchRequest(request));
}

void Genyd::sendHostClipboardToAndroid(const Request &request)
{
    if(request.type() == Request::SetParam &&
            request.parameter().type() == Parameter::Clipboard) {

        Parameter param = request.parameter();
        if (param.has_value()) {
            // If clipboardProxy already connected, send the new clipboard
            if(clipboardProxy) {
                clipboardProxy->write(param.value().stringvalue().c_str(), param.value().stringvalue().size());
                SLOGD("sendHostClipboardToAndroid %s", param.value().stringvalue().c_str());
            }
            else {
                // Else, keep clipboard value, it will be send when clipboardProxy will connect
                clipboard = param.value().stringvalue();
            }
        }
    }
}

void Genyd::sendAndroidClipboardToHost()
{
    if(clipboardProxy) {
        char clipboardText[1024];

        Socket::ReadStatus status = clipboardProxy->read(clipboardText, 1024);

        switch (status) {
        case Socket::ReadError:
            SLOGD("clipboardProxy read error");
        case Socket::NoMessage:
            SLOGD("clipboardProxy deconnection");
            clients.erase(clipboardProxy->getFD());
            delete clipboardProxy;
            clipboardProxy = NULL;
            break;
        case Socket::NewMessage:
            SLOGD("sendAndroidClipboardToHost %s", clipboardText);
            // Broadcast message to all clients
        {
            std::map<int, Socket*>::iterator begin = clients.begin();
            std::map<int, Socket*>::iterator end = clients.end();
            while (begin != end) {
                if(begin->second != clipboardProxy) {
                    Request *request = new Request();
                    request->set_type(Request::PushData);
                    Parameter *parameter = request->mutable_parameter();
                    parameter->set_type(Parameter::Clipboard);
                    Value *value = parameter->mutable_value();
                    value->set_type(Value::String);
                    value->set_stringvalue(clipboardText);

                    begin->second->addRequest(request);
                    begin->second->ask();
                }
                ++begin;
            }
        }
            break;
        case Socket::UnknownMessage:
        default:
            SLOGE("Unknown clipboardProxy status");
        }
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

        std::map<int, Socket*>::iterator begin = clients.begin();
        std::map<int, Socket*>::iterator end = clients.end();

        // Handle messages for clients
        while (begin != end) {
            // Ready read
            if (FD_ISSET(begin->first, &readfs)) {
                if(begin->second == clipboardProxy) {
                    sendAndroidClipboardToHost();
                }
                else {
                    Socket::ReadStatus status = begin->second->read();
                    switch (status) {
                    case Socket::ReadError:
                        SLOGD("Socket read error");
                    case Socket::NoMessage:
                        delete begin->second;
                        clients.erase(begin++);
                        SLOGD("Socket deconnection");
                        continue;
                        break;
                    case Socket::NewMessage:
                    case Socket::UnknownMessage:
                        treatMessage(begin->second);
                        break;
                    default:
                        SLOGE("Unknown socket status");
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
                    continue;
                }
            }

            ++begin;
        }
    }
}

bool Genyd::isInit(void) const
{
    return (server);
}
