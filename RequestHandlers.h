#ifndef _REQUEST_HANDLERS_H
#define _REQUEST_HANDLERS_H

#include "HTTPRequestHandler.h"
class NotFoundRequestHandler : public HTTPRequestHandler
{
public:
  virtual bool        handle(HTTPRequest* request, HTTPResponse* response)
  {
    response->setStatus(404);
    response->printf("Not found: %s", request->path());
    return true;
  }
};

#endif //_REQUEST_HANDLERS_H
