
#ifndef _HOUSECAT_INPUT_SENSOR_H_
#define _HOUSECAT_INPUT_SENSOR_H_

#include "housecat.h"

class housecatInputSensor
{
  private:
    static const bool m_debug = true;
    
    static const uint16_t m_holdOffTimeMs = 50;

    uint8_t m_inputNumber;
    bool m_activeState = true;
    unsigned long m_timerPrv = 0;
    bool m_pulse;
    bool m_firstPoll = true;

    enum enum_inputState {rising_edge, rising_edge_holdoff, falling_edge, falling_edge_holdoff};
    enum_inputState m_inputState = rising_edge;

    unsigned long readTimeMs();

  public:
    housecatInputSensor(uint8_t inputNumber, bool activeState);

    void poll();

    bool pulse();
};

#endif
