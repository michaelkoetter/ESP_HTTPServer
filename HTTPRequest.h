#ifndef _HTTP_REQUEST_H
#define _HTTP_REQUEST_H

#include <Print.h>

#include "config.h"
#include "http_parser.h"

class HTTPConnection;

class HTTPRequestValue
{
public:
  HTTPRequestValue() : _value(0), _size(0) {}
  ~HTTPRequestValue() { if (_value) free(_value); }

  inline int onData(const char * data, size_t length) {
    bool is_empty = _size == 0;
    _size += length;
    _value = (char*) realloc(_value, _size + 1);
    if (is_empty) {
      strncpy(_value, data, length);
    } else {
      strncat(_value, data, length);
    }
    _value[_size] = 0x00; // zero-terminate
    return 0;
  }

  void set(const char * str) {
    clear(); onData(str, strlen(str));
  }

  void clear() {
    _size = 0;
    if (_value) _value[0] = 0x00;
  }

  operator bool() const { return _size > 0; }

  const char* c_str() { return _value; }
  size_t size() { return _size; }
private:
  char* _value;
  size_t _size;
};

struct HTTPHeader
{
  HTTPRequestValue field;
  HTTPRequestValue value;
  bool store = false;
};

class HTTPRequest
{

friend HTTPConnection;

public:
  void            storeHeader(const char * name, bool store = true);

private:
  HTTPRequest(HTTPConnection* connection, http_method method = (http_method)1337);
  ~HTTPRequest();

  int             onUrl(const char * data, size_t length);
  int             onHeaderField(const char * data, size_t length);
  int             onHeaderValue(const char * data, size_t length);
  int             onHeadersComplete();
  int             findHeader(const char * startsWith, size_t length);

  void            log(Print& p);

  HTTPConnection* m_connection;
  http_method     m_method;

  HTTPRequestValue  m_url;
  HTTPRequestValue  m_headerField;
  HTTPHeader      m_header[HTTPSERVER_MAX_HEADER_LINES];
  int             m_currentHeader;
  int             m_headerLength;
  bool            m_headerValue;

  unsigned long   m_age;
  unsigned long   m_id;

  static unsigned long __id;
};

#endif //_HTTP_REQUEST_H
