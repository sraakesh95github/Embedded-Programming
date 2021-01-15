/************************************************************************************************************
 * Name  :  SerialFlush
 * Type  :  Function
 * Param@:  None
 * Brief :  Flushes out serial using Serial.read() function. Serial.read returns -1 when serial becomes empty.
 * RetVal:  None
*************************************************************************************************************/
void SerialFlush()                                              // Flush serial bytes
{
  gDebugMode && DISPLAY_SERIAL.println("Flush");                // Display Flush
  int indx =0;                                                  // Index is reset
  char element = '\0';                                          // Element  
  while(indx != IS_SERIAL_EMPTY)                                // Check if serial is empty
  {
    indx = DATA_SERIAL.read();                                  // Read serial to clear buffer
    element = indx;                                             // Element is taken
    gDebugMode && DISPLAY_SERIAL.println(element);              // Display element read from serial    
  }
  if(gTimerMode)
  {
      WriteToLabViEW("FLS");                                        // Flush serial  
  }
}

/************************************************************************************************************
 * Name  :  WriteToLabViEW
 * Type  :  Function
 * Param@:  String to write
 * Brief :  Appends Header and Footer to the string and writes it to LabViEW.
 * RetVal:  None
*************************************************************************************************************/
void WriteToLabViEW(char writeStr[3])                           // Write string to LabView
{
  char writeString[8] = {'\0'};                                 // Write string variable declared    
  char writePack[3] = {'1','1','\0'};
  writeStr[3] = '\0';                                           // Write string
  writeString[7] = '\0';                                        // write string variable      
  
  writeString[0] = writePack[0];                               // Writing header 
  writeString[1] = writePack[1];                               // Writing header  
  
  writeString[2] = writeStr[0];                                 // Writing command
  writeString[3] = writeStr[1];                                 // Writing command
  writeString[4] = writeStr[2];                                 // Writing command  
  
  writeString[5] = writePack[0];                               // Writing footer
  writeString[6] = writePack[1];                               // Writing footer

  DATA_SERIAL.write(writeString);                              // Write formed command to LabView
}
