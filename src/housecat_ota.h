#ifndef _HOUSECAT_OTA_H_
#define _HOUSECAT_OTA_H_

#include <WebServer.h>
#include <ESPmDNS.h>
#include <Update.h>

class housecatOta
{
  private:
	  static WebServer m_otaWebServer;
    
  public:
    housecatOta();
	
    void init();
    void poll();
};

#endif