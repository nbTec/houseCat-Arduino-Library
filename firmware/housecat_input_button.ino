
housecat_input_button::housecat_input_button()
{

}

void housecat_input_button::init()
{

}

void housecat_input_button::poll()
{
  if(dummy_input)
  {
    
  }
  
}

uint8_t housecat_input_button::short_press()
{
  return m_shortPress;
}

uint8_t housecat_input_button::long_press()
{
  return m_longPress;
}
