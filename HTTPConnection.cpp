#include "HTTPConnection.h"

HTTPConnection* HTTPConnection::m_first = 0;

HTTPConnection::HTTPConnection(WiFiClient client)
  : m_client(client), m_received(0), m_next(0)
{
  http_parser_init(&m_parser, HTTP_REQUEST);
  m_parser.data = this; // used by callbacks

  m_settings.on_message_begin = &cb_onMessageBegin;
  m_settings.on_headers_complete = &cb_onHeadersComplete;
  m_settings.on_message_complete = &cb_onMessageComplete;
  m_settings.on_url = &cb_onUrl;
  m_settings.on_header_field = &cb_onHeaderField;
  m_settings.on_header_value = &cb_onHeaderValue;
  m_settings.on_body = &cb_onBody;

  HTTPConnection::add(this);
}

HTTPConnection::~HTTPConnection()
{
  HTTPConnection::remove(this);
}

void HTTPConnection::handle()
{
  if (m_next) {
    m_next->handle();
  }

  uint8_t buf[HTTPSERVER_BUFFER];
  int received = 0;
  int parsed = 0;
  do {
    received = m_client.read(buf, HTTPSERVER_BUFFER);
    m_received += received;
    if (m_received > 0) {
      HTTP_DEBUG("[%d] HTTPConnection::handle Received %d bytes... \n", id, received)
      parsed = http_parser_execute(&m_parser, &m_settings, reinterpret_cast<const char*>(buf), received);
      HTTP_DEBUG("[%d] HTTPConnection::handle Parsed %d bytes... \n", id, parsed)
      if (parsed != received) {
        HTTP_DEBUG("[%d] HTTPConnection::handle Parsed != Received! \n", id)
      }

      if (received == 0) {
        // EOF
        delete this;
      }
    }
  } while(received > 0);
}

void HTTPConnection::add(HTTPConnection *self)
{
  HTTPConnection* tmp = m_first;

  if (tmp == 0) {
    self->id = 0;
  } else {
    self->id = tmp->id + 1;
  }

  m_first = self;
  self->m_next = tmp;
  HTTP_DEBUG("[%d] HTTPConnection::add \n", self->id)
}

void HTTPConnection::remove(HTTPConnection *self)
{
  HTTP_DEBUG("[%d] HTTPConnection::remove \n", self->id)
  if (m_first == self) {
    m_first = self->m_next;
    self->m_next = 0;
    return;
  }

  for (HTTPConnection* prev = m_first; prev->m_next; prev = prev->m_next ) {
    if (prev->m_next == self) {
      prev->m_next = self->m_next;
      self->m_next = 0;
      return;
    }
  }
}

int HTTPConnection::onMessageBegin()
{

}

int HTTPConnection::onHeadersComplete()
{

}

int HTTPConnection::onMessageComplete()
{
  m_client.write("HTTP/1.1 200 OK\r\n"
                 "Content-Type: text/ascii\r\n"
                 "\r\n"
                 "OK");
}

int HTTPConnection::onUrl(const char * data)
{

}

int HTTPConnection::onHeaderField(const char * data)
{

}

int HTTPConnection::onHeaderValue(const char * data)
{

}

int HTTPConnection::onBody(const char * data)
{

}
