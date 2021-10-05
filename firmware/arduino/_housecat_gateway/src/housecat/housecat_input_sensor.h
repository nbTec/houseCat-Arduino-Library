
#ifndef _HOUSECAT_INPUT_SENSOR_H_
#define _HOUSECAT_INPUT_SENSOR_H_

#include "housecat.h"

class housecatInputSensor
{
  private:
    
    static const uint16_t m_holdOffTimeMs     = 50;

    uint8_t m_inputNumber;
    unsigned long m_timerPrv = 0;
    bool m_pulse;
    bool m_firstPoll = true;

    enum enum_inputState {rising_edge, rising_edge_holdoff, falling_edge, falling_edge_holdoff};
    enum_inputState m_inputState = rising_edge;

    unsigned long readTimeMs();

  public:
    housecatInputSensor(uint8_t inputNumber);

    void poll();

    bool pulse();
};

#endif
