#include "HTTPServer.h"
#include "HTTPConnection.h"

HTTPServer::HTTPServer(int port)
  : m_server(port)
{
  m_server.close();
}

HTTPServer::~HTTPServer()
{

}

void HTTPServer::begin()
{
  m_server.begin();
}

void HTTPServer::handle()
{
  // accept new clients
  WiFiClient _client = m_server.available();
  if (_client) {
    HTTP_DEBUG("New client \n")
    new HTTPConnection(_client);
  }

  // handle connections
  if (HTTPConnection::first()) {
    HTTPConnection::first()->handle();
  }
}
