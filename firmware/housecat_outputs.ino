
housecat_outputs::housecat_outputs()
{

}

void housecat_outputs::init()
{
  for (int i = 0; i < 4; i++)
  {
    m_io_expander[i].begin(i+4);
    for (int j = 0; j < 16; j++)
    {
      m_io_expander[i].pinMode(j, OUTPUT);
      m_io_expander[i].digitalWrite(j, HIGH);
    }
  }
}
