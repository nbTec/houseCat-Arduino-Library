
#include "housecat.h"

housecat hc;

void setup()
{
  hc.udpEnable();
  hc.udpSetAddress(1);
  hc.udpSetReceiver(55555);
  hc.udpSetSender(IPAddress(192, 168, 1, 101), 44444);

  hc.init();
}

void loop()
{
  hc.poll();
}
