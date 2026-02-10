// Attempt to read the ESP32 ADC more accurately by using a polynomial
// correction curve. The ESP32 ADC is non-linear, especially near the
// extremes. This 4th-degree polynomial maps raw ADC readings (0-4095)
// to corrected voltage values (0-3.3V).
float ReadVoltage(byte pin){
  float reading = analogRead(pin); // Reference voltage is 3v3 so maximum reading is 3v3 = 4095 in range 0 to 4095
  if(reading < 1 || reading > 4095) return 0;
  return -0.000000000000016 * pow(reading,4) + 0.000000000118171 * pow(reading,3)- 0.000000301211691 * pow(reading,2)+ 0.001109019271794 * reading + 0.034143524634089;
}
