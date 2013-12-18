#ifndef GENYD_HPP_
#define GENYD_HPP_

#include <map>

#include <unistd.h>

#include "global.hpp"
#include "socket.hpp"
#include "dispatcher.hpp"

class Genyd {

public:
  Genyd(void);
  ~Genyd(void);

private:
  Genyd(const Genyd &);
  Genyd operator=(const Genyd &);

private:
  Socket *server;
  Dispatcher dispatcher;
  std::map<int, Socket *> clients;

  // socket to the clipboardProxy service
  // this socket is included in clients too
  Socket* clipboardProxy;

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

};

#endif
