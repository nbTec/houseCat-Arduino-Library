
#include "housecat.h"

housecat hc;

void setup()
{
  g_housecat_protocol.udpEnable();
  g_housecat_protocol.udpSetAddress(1);
  g_housecat_protocol.udpSetReceiver(55555);
  g_housecat_protocol.udpSetSender(IPAddress(192, 168, 1, 101), 44444);

  hc.init();
}

void loop()
{
  hc.poll();
}
