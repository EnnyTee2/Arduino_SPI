#define DATAOUT 11//COPI -- SI
#define DATAIN  12//CIPO -- SO
#define SPICLOCK  13//sck
#define CHIPSELECT 10//cs

//opcodes
#define WREN  6
#define WRDI  4
#define RDSR  5
#define WRSR  1
#define READ  3
#define WRITE 2

byte eeprom_output_data;
byte eeprom_input_data=0;
byte clr;
int address=0;
//data buffer
char buffer [32];



void setup()
{

  Serial.begin(2400);

  pinMode(DATAOUT, OUTPUT);

  pinMode(DATAIN, INPUT);

  pinMode(SPICLOCK,OUTPUT);

  pinMode(CHIPSELECT,OUTPUT);

  digitalWrite(CHIPSELECT,HIGH); //disable device
  
  
  // SPCR = 01010000

  //interrupt disabled,spi enabled,msb 1st,controller,clk low when idle,

  //sample on leading edge of clk, system clock/4 rate (fastest)

  SPCR = (1<<SPE)|(1<<MSTR);

  clr=SPSR;

  clr=SPDR;

  delay(10);
  
  //fill buffer with data

  fill_buffer();

  //fill eeprom w/ buffer

  digitalWrite(CHIPSELECT,LOW);

  spi_transfer(WREN); //write enable

  digitalWrite(CHIPSELECT,HIGH);
  
  
  delay(10);

  digitalWrite(CHIPSELECT,LOW);

  spi_transfer(WRITE); //write instruction

  address=0;

  spi_transfer((char)(address>>8));   //send MSByte address first

  spi_transfer((char)(address));      //send LSByte address

  //write 32 bytes

  for (int I=0;I<32;I++)

  {

    spi_transfer(buffer[I]); //write data byte

  }

  digitalWrite(CHIPSELECT,HIGH); //release chip

  //wait for eeprom to finish writing

  delay(3000);
  
  Serial.print('h',BYTE);

  Serial.print('i',BYTE);

  Serial.print('\n',BYTE);//debug

  delay(1000);
}



  void fill_buffer()
{

  for (int I=0;I<32;I++)

  {

    buffer[I]=I;

  }
}


char spi_transfer(volatile char data)
{

  SPDR = data;                    // Start the transmission

  while (!(SPSR & (1<<SPIF)))     // Wait for the end of the transmission

  {

  };

  return SPDR;                    // return the received byte
}


byte read_eeprom(int EEPROM_address)
{

  //READ EEPROM

  int data;
  

  digitalWrite(CHIPSELECT,LOW);

  spi_transfer(READ); //transmit read opcode

  spi_transfer((char)(EEPROM_address>>8));   //send MSByte address first

  spi_transfer((char)(EEPROM_address));      //send LSByte address
  
  data = spi_transfer(0xFF); //get data byte

  digitalWrite(CHIPSELECT,HIGH); //release chip, signal end transfer

  return data;
}
