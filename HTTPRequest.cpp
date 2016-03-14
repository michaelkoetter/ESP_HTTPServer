#include "HTTPRequest.h"
#include "HTTPConnection.h"

unsigned long HTTPRequest::__id = 0;


HTTPRequest::HTTPRequest()
  : m_method((http_method) -1),
    m_currentHeader(-1), m_headerLength(0), m_headerValue(true),
    m_age(millis()), m_id(__id++)
{
  HTTP_DEBUG("   <%lu> HTTPRequest::HTTPRequest\n", m_id)

  // we store some headers by defult
  storeHeader("Host");
  storeHeader("Content-Type");
  storeHeader("Content-Length");

  // CORS support
  storeHeader("Access-Control-Request-Method");
  storeHeader("Access-Control-Request-Headers");
}

HTTPRequest::~HTTPRequest()
{
  HTTP_DEBUG("   <%lu> HTTPRequest::~HTTPRequest \n", m_id)
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

int HTTPRequest::onUrl(http_method method, const char * data, size_t length)
{
  m_method = method;
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
  HTTP_DEBUG("   <%lu> HTTPRequest::onHeadersComplete \n", m_id);
  #ifdef HTTPSERVER_DEBUG
  int i = 0;
  while (i < HTTPSERVER_MAX_HEADER_LINES && m_header[i].field) {
    HTTP_DEBUG("   <%lu>    %s: %s \n", m_id,
    m_header[i].field.c_str(), m_header[i].value.c_str());
    i++;
  }
  #endif

  return 0;
}

void HTTPRequest::log(Print& p)
{
  unsigned long ticks = millis();
  p.printf("@%lu %s %s %-lums \n", ticks, http_method_str(m_method),
    m_url.c_str(), ticks - m_age);
}
