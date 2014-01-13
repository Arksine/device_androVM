#ifndef SOCKET_HPP_
#define SOCKET_HPP_

#include <queue>
#include <sstream>

#include "genymotion.hpp"

#define BUFFER_SIZE 8192

/*
** C socket encapsulation
*/
class Socket {

public:
    enum ReadStatus {
        ReadError,
        NoMessage,
        NewMessage,
        UnknownMessage,
    };

    enum WriteStatus {
        WriteError,
        BadSerialize,
        WriteSuccess
    };

public:
    Socket(int socket);
    ~Socket(void);

private:
    Socket(void);
    Socket(const Socket &);
    Socket &operator=(const Socket &);

private:
    // Socket fd
    int socket;

    // Request to treat
    Request request;

    // Datastream (fed by socket buffer and consume by Protobuf)
    std::stringstream istream;

    // Replies list
    std::queue<Reply *> replies;

    // Requests list
    std::queue<Request *> requests;

public:
    // Read data from socket
    ReadStatus read(void);


    // Read data from socket
    ReadStatus read(std::string *data);

    // Write data to the socket
    WriteStatus write(const char *buf, int size);

    // Return whether there're replies to send or not
    bool hasReplies(void) const;

    // Return whether there're requests to send or not
    bool hasRequests(void) const;

    // Try and write a reply on the socket
    WriteStatus reply(void);

    // Try and write a request on the socket
    WriteStatus ask(void);

    // Get the socket fd
    int getFD(void) const;

    // Get the current request to treat
    const Request &getRequest(void) const;

    // Add a reply to the replies list
    void addReply(Reply *reply);

    // Add a request to the requests list
    void addRequest(Request *request);

};


#endif
