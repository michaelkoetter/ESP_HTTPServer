#include "HTTPRequest.h"
#include "HTTPConnection.h"

unsigned long HTTPRequest::__id = 0;


HTTPRequest::HTTPRequest(HTTPConnection* connection, http_method method)
  : m_connection(connection), m_method(method), m_headerLength(0),
    m_headerValue(true), m_currentHeader(-1), m_age(millis()), m_id(__id++)
{
  HTTP_DEBUG("<%d> <%d> HTTPRequest::HTTPRequest method=%s \n",
    m_connection ? m_connection->id() : 0, m_id, http_method_str(m_method))

  // we store some headers by defult
  storeHeader("Host");
  storeHeader("Content-Type");
  storeHeader("Content-Length");

  // CORS support
  storeHeader("Origin");
  storeHeader("Access-Control-Request-Method");
  storeHeader("Access-Control-Request-Headers");
}

HTTPRequest::~HTTPRequest()
{
  HTTP_DEBUG("<%d> <%d> HTTPRequest::~HTTPRequest \n",
    m_connection ? m_connection->id() : 0, m_id)
}

void HTTPRequest::storeHeader(const char * name, bool store) {
  int i = 0;
  while (i < HTTPSERVER_MAX_HEADER_LINES && m_header[i].field.c_str()) i++;
  if (i < HTTPSERVER_MAX_HEADER_LINES) {
    m_header[i].field.set(name);
    m_header[i].store = true;
  }
}

int HTTPRequest::findHeader(const char * startsWith, size_t length)
{
  int i = 0;
  while (i < HTTPSERVER_MAX_HEADER_LINES && m_header[i].field) {
    if (strncasecmp(m_header[i].field.c_str(), startsWith, length) == 0) return i;
    i++;
  }
  return -1;
}

int HTTPRequest::onUrl(const char * data, size_t length)
{
  return m_url.onData(data, length);
}

int HTTPRequest::onHeaderField(const char * data, size_t length)
{
  m_headerField.onData(data, length);
  m_currentHeader = findHeader(m_headerField.c_str(), m_headerField.size());
  m_headerValue = false;
  return 0;
}

int HTTPRequest::onHeaderValue(const char * data, size_t length)
{
  if (m_currentHeader >= 0 && m_header[m_currentHeader].store) {
    m_header[m_currentHeader].value.onData(data, length);
  }
  m_headerValue = true;
  m_headerField.clear();
  return 0;
}

int HTTPRequest::onHeadersComplete()
{
  HTTP_DEBUG("<%d> <%d> HTTPRequest::onHeadersComplete \n", m_connection->id(), m_id);
  #ifdef HTTPSERVER_DEBUG
  int i = 0;
  while (i < HTTPSERVER_MAX_HEADER_LINES && m_header[i].field) {
    HTTP_DEBUG("<%d> <%d>    %s: %s \n", m_connection->id(), m_id,
    m_header[i].field.c_str(), m_header[i].value.c_str());
    i++;
  }
  #endif

  return 0;
}

void HTTPRequest::log(Print& p)
{
  p.printf("<%d> <%d> %d %s %s %dms \n", m_connection->id(), m_id,
    millis(), http_method_str(m_method), m_url.c_str(), millis() - m_age);
}
