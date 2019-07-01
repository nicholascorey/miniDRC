#include "Wire.h"

const unsigned char WM8731_ADDRESS            = 0x1A;   // WM8731 chip address on I2C bus
const unsigned char WM8731_REG_LLINE_IN       = 0x00;   // Left Channel Line Input Volume Control
const unsigned char WM8731_REG_RLINE_IN       = 0x01;   // Right Channel Line Input Volume Control
const unsigned char WM8731_REG_LHPHONE_OUT    = 0x02;   // Left Channel Headphone Output Volume Control
const unsigned char WM8731_REG_RHPHONE_OUT    = 0x03;   // Right Channel Headphone Output Volume Control
const unsigned char WM8731_REG_ANALOG_PATH    = 0x04;   // Analog Audio Path Control
const unsigned char WM8731_REG_DIGITAL_PATH   = 0x05;   // Digital Audio Path Control
const unsigned char WM8731_REG_PDOWN_CTRL     = 0x06;   // Power Down Control Register
const unsigned char WM8731_REG_DIGITAL_IF     = 0x07;   // Digital Audio Interface Format
const unsigned char WM8731_REG_SAMPLING_CTRL  = 0x08;   // Sampling Control Register
const unsigned char WM8731_REG_ACTIVE_CTRL    = 0x09;   // Active Control
const unsigned char WM8731_REG_RESET          = 0x0F;   // Reset register

const unsigned short _WM8731_LEFT_LINEIN  = 0X0117;     // 0dB, DISABLE MUTE, ENABLE SIMULTANEOUS LOAD TO LEFT AND RIGHT CHANNELS
const unsigned short _WM8731_RIGHT_LINEIN = 0X0117;     // 0dB, DISABLE MUTE, ENABLE SIMULTANEOUS LOAD TO LEFT AND RIGHT CHANNELS
const unsigned short _WM8731_LEFT_HP      = 0X01F9;     // 0dB, Zero cross detect, Simultaneous load
const unsigned short _WM8731_RIGHT_HP     = 0X01F9;     // 0dB, Zero cross detect, Simultaneous load

const unsigned short _WM8731_ANALOGAUDIO  = 0XD2;       // No mic boost, Mute mic, Line input select, Disable bypass, Select DAC, Disable side tone
//const unsigned short _WM8731_ANALOGAUDIO  = 0XDA;       // No mic boost, Mute mic, Line input select, Enable bypass, Select DAC, Disable side tone

const unsigned short _WM8731_DIGITALAUDIO = 0X04;       // Enable ADC high pass, disable DAC de-emphasis, enable soft mute, clear DC offset when high pass filter disabled
const unsigned short _WM8731_POWER        = 0X00;       // DISABLE POWER DOWN
const unsigned short _WM8731_DAIF         = 0X4A;       // I2S format, 24 bit data, right channel low, no channel swap, master mode, don't invert BCLK 
const unsigned short _WM8731_SAMPLING     = 0X00;       // Normal mode, 256fs, 48000hz with 12.288MHz oscillator, no clock div
const unsigned short _WM8731_ACTIVATE     = 0X01;       // MODULE IS ON
const unsigned short _WM8731_DEACTIVATE   = 0X00;       // MODULE IS OFF
const unsigned short _WM8731_RESET        = 0X00;       // RESET VALUE

void wm8731_cmd(unsigned char address, unsigned short cmd) {
  unsigned char addr;
  int blah;
  addr = address << 1;                 // Shift left for one positions
  addr = addr | ((cmd >> 8) & 0x01);
  Wire.beginTransmission(WM8731_ADDRESS); // transmit to device #44 (0x2c)
  Wire.write(addr);            // sends instruction byte
  Wire.write((unsigned char)(cmd & 0xff));             // sends potentiometer value byte
  Wire.endTransmission();
}

void wm8731_config() {
  Wire.begin();

  wm8731_cmd(WM8731_REG_RESET,          _WM8731_RESET);         // RESET MODULE
  wm8731_cmd(WM8731_REG_LLINE_IN,       _WM8731_LEFT_LINEIN);   // LEFT LINE IN SETTINGS
  wm8731_cmd(WM8731_REG_RLINE_IN,       _WM8731_RIGHT_LINEIN);  // RIGTH LINE IN SETTINGS
  wm8731_cmd(WM8731_REG_LHPHONE_OUT,    _WM8731_LEFT_HP);       // LEFT HEADPHONE OUT SETTINGS
  wm8731_cmd(WM8731_REG_RHPHONE_OUT,    _WM8731_RIGHT_HP);      // RIGHT HEADPHONE OUT SETTINGS
  wm8731_cmd(WM8731_REG_ANALOG_PATH,    _WM8731_ANALOGAUDIO);   // ANALOG PATHS
  wm8731_cmd(WM8731_REG_DIGITAL_PATH,   _WM8731_DIGITALAUDIO);  // DIGITAL PATHS
  wm8731_cmd(WM8731_REG_PDOWN_CTRL,     _WM8731_POWER);         // POWER DOWN CONTROL
  wm8731_cmd(WM8731_REG_DIGITAL_IF,     _WM8731_DAIF);          // DIGITAL INTERFACE
  wm8731_cmd(WM8731_REG_SAMPLING_CTRL,  _WM8731_SAMPLING);      // SAMPLING CONTROL
  wm8731_cmd(WM8731_REG_ACTIVE_CTRL,    _WM8731_ACTIVATE);      // ACTIVATE MODULE
}
