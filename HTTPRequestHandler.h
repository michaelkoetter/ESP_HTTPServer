#ifndef _HTTP_REQUEST_HANDLER_H
#define _HTTP_REQUEST_HANDLER_H

#include "HTTPRequest.h"
#include "HTTPResponse.h"

class HTTPRequestHandler
{
public:
  /**
   * This is called whenever the server starts handling a new request.
   * The request will not be fully populated.
   *
   * The RequestHandler implementation can specify the headers it is interested
   * in via HTTPRequest#storeHeader.
   */
  virtual void        newRequest(HTTPRequest* request) {}

  virtual bool        handle(HTTPRequest* request, HTTPResponse* response) = 0;
};

#endif //_HTTP_REQUEST_HANDLER_H
