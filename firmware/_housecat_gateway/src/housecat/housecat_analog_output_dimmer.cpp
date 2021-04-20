
#ifdef __AVR
  #include <avr/pgmspace.h>
#elif defined(ESP8266)
  #include <pgmspace.h>
#endif

#include "housecat_analog_output_dimmer.h"

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

housecatAnalogOutputDimmer::housecatAnalogOutputDimmer(housecatAnalogOutputs &analogOutputs, uint8_t outputNumber, float startValue, float currentValue)
: m_analogOutputs(analogOutputs), m_outputNumber(outputNumber), m_startValue(startValue), m_outputValue(currentValue)
{
}

unsigned long housecatAnalogOutputDimmer::readTimeMs()
{
  return millis();
}

void housecatAnalogOutputDimmer::poll(bool toggleInput, bool cycleInput)
{
  uint8_t toggle_pressed = toggleInput && (!m_toggleInputPrv);
  //uint8_t cycle_pressed = cycleInput && (!m_cycleInputPrv);

  if (m_firstPoll)
  {
      m_firstPoll = false;
  }

  if (toggle_pressed)
  {
    m_outputState = !m_outputState;
    if(m_outputState == true)
    {
        m_analogOutputs.write(m_outputNumber, m_outputValue);
    }
    else
    {
        m_analogOutputs.write(m_outputNumber, 0.0);
    }
  }

  if (cycleInput && m_outputState)
  {
      if((readTimeMs() - m_prvCycleTimeMs) >= m_cycleIncrementTimerMs)
      {
        m_outputValue += m_cycleIncrementValue;

        if(m_outputValue > m_maxValue)
        {
            m_outputValue = m_startValue;
        }

        m_analogOutputs.write(m_outputNumber, m_outputValue);
        m_prvCycleTimeMs = readTimeMs();
      }
  }


  m_toggleInputPrv = toggleInput;
  //m_cycleInputPrv = cycleInput;
}
