
housecat_inputs::housecat_inputs()
{

}

void housecat_inputs::init()
{
  for (int i = 0; i < 4; i++)
  {
    m_io_expander[i].begin(i);
    m_io_expander[i].setupInterrupts(true, true, LOW);
    for (int j = 0; j < 16; j++)
    {
      m_io_expander[i].pinMode(j, INPUT);
      m_io_expander[i].setupInterruptPin(j, CHANGE);
    }
  }
}

void IRAM_ATTR housecat_inputs::interrupt_callback()
{
  uint8_t interrupt, value;
  for (int i = 0; i < 4; i++)
  {
    interrupt = m_io_expander[i].getLastInterruptPin();
    if (interrupt != MCP23017_INT_ERR)
    {
      value = m_io_expander[i].getLastInterruptPinValue();
      Serial.print("IO Expander: ");
      Serial.print(i);
      Serial.print(", Input: ");
      Serial.print(interrupt);
      Serial.print(", Value: ");
      Serial.println(value);
    }
  }
}
