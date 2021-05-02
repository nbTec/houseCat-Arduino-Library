
#ifndef _HOUSECAT_INPUT_BUTTON_H_
#define _HOUSECAT_INPUT_BUTTON_H_

#include "housecat_inputs.h"
#include "housecat_protocol.h"

class housecatInputButton
{
  private:
    housecatProtocol &m_protocol;
    housecatInputs &m_inputs;
    
    static const uint16_t m_longPressTimeMs   = 1000;
    static const uint16_t m_holdOffTimeMs     = 50;

    uint8_t m_inputNumber;
    unsigned long m_timerPrv = 0;
    bool m_shortPress;
    bool m_longPress;
    bool m_firstPoll = true;

    enum enum_inputState {rising_edge, rising_edge_holdoff, falling_edge, falling_edge_holdoff, long_press_wait};
    enum_inputState m_inputState = rising_edge;

    unsigned long readTimeMs();

  public:
    housecatInputButton(housecatProtocol &protocol, housecatInputs &inputs, uint8_t inputNumber);

    void poll();

    bool shortPress();
    bool longPress();
};

#endif
