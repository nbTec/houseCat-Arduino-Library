
#ifndef _HOUSECAT_INPUT_BUTTON_H_
#define _HOUSECAT_INPUT_BUTTON_H_

#include "housecat.h"

class housecatInputButton
{
  private:
    static const bool m_debug = true;
    
    static const uint16_t m_longPressTimeMs   = 1000;
    static const uint16_t m_holdOffTimeMs     = 50;

    uint8_t m_inputNumber = 0;
    unsigned long m_timerPrv = 0;
    bool m_shortPress = false;
    bool m_longPress = false;
    bool m_firstPoll = true;

    enum enum_inputState {rising_edge, rising_edge_holdoff, falling_edge, falling_edge_holdoff, long_press_wait};
    enum_inputState m_inputState = rising_edge;

    unsigned long readTimeMs();

  public:
    housecatInputButton(uint8_t inputNumber);

    void poll();

    bool shortPress();
    bool longPress();
};

#endif
