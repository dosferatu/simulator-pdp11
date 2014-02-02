class Memory
{
  public:
    Memory();
    ~Memory();
    short Read(short effectiveAddress);
    void Write(short effectiveAddress, short data);

  private:
    short *RAM;
};
