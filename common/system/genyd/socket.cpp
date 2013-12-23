
#include <unistd.h>
#include <strings.h>
#include <sys/socket.h>

#include "socket.hpp"

Socket::Socket(int socket) :
    socket(socket)
{

}

Socket::~Socket(void)
{
    if (socket != -1) {
        close(socket);
    }
}

Socket::ReadStatus Socket::read(void)
{
    int len = 0;
    char *cmd = 0;
    char buffer[1024];

    if ((len = recv(socket, buffer, 1023, MSG_NOSIGNAL)) < 0) {
        SLOGE("recv() error");
        return Socket::ReadError;
    }

    if (len == 0) {
        return Socket::NoMessage;
    }

    istream.write(buffer, len);

    // Try to parse the current stream
    if (request.ParseFromIstream(&istream)) {
        // On success, clear stream
        istream.str("");
        istream.clear();
        return Socket::NewMessage;
    } else {
        SLOGE("Can't parse request");
        return Socket::UnknownMessage;
    }
}

// Read data from socket
Socket::ReadStatus Socket::read(char *buf, int size)
{
    int len = 0;
    char *cmd = 0;

    if ((len = ::read(socket, buf, size)) < 0) {
        SLOGE("recv() error");
        return Socket::ReadError;
    }

    if (len == 0) {
        return Socket::NoMessage;
    }

    return Socket::NewMessage;
}

// Write data to the socket
Socket::WriteStatus Socket::write(const char *buf, int size)
{
    int len = 0;
    char *cmd = 0;

    if ((len = send(socket, buf, size, MSG_NOSIGNAL)) < 0) {
        SLOGE("write() error");
        return Socket::WriteError;
    }

    return Socket::WriteSuccess;
}

bool Socket::hasReplies(void) const
{
    return replies.size();
}

bool Socket::hasRequests(void) const
{
    return requests.size();
}

Socket::WriteStatus Socket::reply(void)
{
    std::string data;
    int len = 0;

    Reply *reply = replies.front();
    replies.pop();

    if (!reply->SerializeToString(&data)) {
        SLOGE("Can't serialize reply");
        delete reply;
        return Socket::BadSerialize;
    }
    if ((len = send(socket, data.c_str(), data.size(), MSG_NOSIGNAL)) < 0) {
        SLOGE("Can't send reply");
        delete reply;
        return Socket::WriteError;
    }

    delete reply;
    return Socket::WriteSuccess;
}

Socket::WriteStatus Socket::ask(void)
{
    std::string data;
    int len = 0;

    Request *request = requests.front();
    requests.pop();

    if (!request->SerializeToString(&data)) {
        SLOGE("Can't serialize request");
        delete request;
        return Socket::BadSerialize;
    }
    if ((len = send(socket, data.c_str(), data.size(), MSG_NOSIGNAL)) < 0) {
        SLOGE("Can't send request");
        delete request;
        return Socket::WriteError;
    }

    delete request;
    return Socket::WriteSuccess;
}

int Socket::getFD(void) const
{
    return socket;
}

const Request &Socket::getRequest(void) const
{
    return request;
}

void Socket::addReply(Reply *reply)
{
    replies.push(reply);
}


void Socket::addRequest(Request *request)
{
    requests.push(request);
}
