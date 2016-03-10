#pragma once
#include "config.h"
#include <ESP8266WiFi.h>
#include "HTTPConnection.h"

struct ConnectionEntry
{
  WiFiClient client;
  HTTPConnection httpConnection;
};

class HTTPServer
{
public:
  HTTPServer(int port = 80, bool tcpNoDelay = true);
  ~HTTPServer();

  void              begin();
  void              handle();

private:
  WiFiServer        m_server;
  bool              m_tcpNoDelay;

  ConnectionEntry*  m_connections;
  uint8_t           m_buffer[HTTPSERVER_BUFFER];

  bool              m_canHandle, m_canAccept;
};
