
const uint16_t BUTTON_SHORT_PRESS_MS   = 1000;    
const uint16_t BUTTON_HOLDOFF          = 50;     

class housecat_input_button
{
  private:
    uint8_t m_shortPress;
    uint8_t m_longPress;

    uint8_t dummy_input;

  public:
    housecat_input_button();
    
    void init();
    void poll();
    
    uint8_t short_press();
    uint8_t long_press();
};
