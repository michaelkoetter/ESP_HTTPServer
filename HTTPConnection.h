#pragma once

#include <ESP8266WiFi.h>
#include "HTTPServer.h"

#include <http_parser.h>

#ifdef HTTPSERVER_DEBUG
#define HTTP_PARSER_DEBUG_CB(CB, DATA, ...) \
    HTTP_DEBUG("[%d] HTTPConnection::" #CB " data=%s \n", reinterpret_cast<HTTPConnection *>(p->data)->id, DATA) \
    return reinterpret_cast<HTTPConnection *>(p->data)->CB(__VA_ARGS__);
#else
#define HTTP_PARSER_DEBUG_CB(CB, DATA, ...) \
    return reinterpret_cast<HTTPConnection *>(p->data)->CB(__VA_ARGS__);
#endif

#define HTTP_PARSER_CB(CB, ...) \
  static int cb_##CB(http_parser* p) { HTTP_PARSER_DEBUG_CB(CB, 0) } \
  int CB();

#define HTTP_PARSER_CB_DATA(CB, ...) \
  static int cb_##CB(http_parser* p, const char *at, size_t length) \
    { char buf[length + 1]; memcpy(buf, at, length); buf[length] = 0x00; \
      HTTP_PARSER_DEBUG_CB(CB, buf, reinterpret_cast<const char *>(buf)) } \
  int CB(const char * data);


class HTTPConnection
{
public:
    HTTPConnection(WiFiClient client);
    ~HTTPConnection();

    void              handle();
    static HTTPConnection*  first() { return m_first; }

private:
    // http_parser callback functions
    HTTP_PARSER_CB(onMessageBegin)
    HTTP_PARSER_CB(onHeadersComplete)
    HTTP_PARSER_CB(onMessageComplete)
    HTTP_PARSER_CB_DATA(onUrl)
    HTTP_PARSER_CB_DATA(onHeaderField)
    HTTP_PARSER_CB_DATA(onHeaderValue)
    HTTP_PARSER_CB_DATA(onBody)

    static void       add(HTTPConnection *self);
    static void       remove(HTTPConnection *self);

    WiFiClient        m_client;
    http_parser       m_parser;
    http_parser_settings  m_settings;
    size_t            m_received;

    static HTTPConnection*    m_first;
    HTTPConnection*           m_next;
    int                   id;
};
