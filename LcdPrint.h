class lcdPrint: public LiquidCrystal_I2C
{

    public:
    lcdPrint(int address, int size, int lines) : LiquidCrystal_I2C(address,size, lines) {}
    void printParam(int statNum, String str)
    {
      if (statNum == 1)
        this->setCursor(0, 0);
      else if (statNum == 2)
        this->setCursor(8, 0);
      else if (statNum == 3)
        this->setCursor(0, 1);
      else if (statNum == 4)
        this->setCursor(8, 1);
      while (str.length() < 8)
        str += " ";
      this->print(str);
    }

    void printLine(int lineNum, String str)
    {
      if (lineNum == 1)
        this->setCursor(0, 0);
      else if (lineNum == 2)
        this->setCursor(0, 1);
      while (str.length() < 16)
        str += " ";
      this->print(str);
    }
};