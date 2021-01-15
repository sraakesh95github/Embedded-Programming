
/*****************************************************************************************************
  Name  :  SetPinMode
  Type  :  Function
  Param@:  Pin Number, Mode
  Brief :  Sets pin mode based on the parameter received.
           Also makes all DIOs OFF to be safe.
  RetVal:  None
*****************************************************************************************************/
void SetPinMode(int pin, int mode)                    // Configure IO
{
  if (mode == 0)                                      // Checking mode to be zero
  {
    pinMode(pin, INPUT);                              // pinMode is input  
    gDebugMode && DISPLAY_SERIAL.println("Set input");
  }
  else                                                // If not input
  {
    pinMode(pin, OUTPUT);                             // Configure it output
    digitalWrite(pin,LOW);                            // Write Low by default
    gDebugMode && DISPLAY_SERIAL.println("Set Output");
  }
}

/*****************************************************************************************************
  Name  :  WritePinStatus
  Type  :  Function
  Param@:  Pin Number
  Brief :  Reads pin number and writes it to LabViEW with Pin Number.
  RetVal:  None
*****************************************************************************************************/
void WritePinStatus(int Pin)                          // Write pin status to LabView
{
  short int state = digitalRead(Pin);                 // State variable 
  char IoNum[4];
  IoNum[0] = (Pin / 10) + '0';                       // First character
  IoNum[1] = (Pin % 10) + '0';                       // Second character
  IoNum[2] = state + '0';                            // State is made character  
  IoNum[3] = '\0';
  WriteToLabViEW(IoNum);                             // Writing LabView IO number with status of the same
}

void disablePollInputs(int pin)                       // Disable poll inputs
{
  int pollIndex;// Poll index is declared       
  for (pollIndex = 0; pollIndex < MAX_POLL_INPUTS; pollIndex++) // Run the loop for Max poll times
  {
    if(pollingIOs[pollIndex].pin == pin) // Pin is compared with checking each elements of structure
    {
      pollingIOs[pollIndex].pin = 0; // Pin is made zero if condition matches
      pollElements.pop(); 
      gDebugMode && DISPLAY_SERIAL.println("Polling Disabled");
    }
  }
}

void enablePollInputs(int pin, int state)             // Enable poll inputs
{
  gDebugMode && DISPLAY_SERIAL.println("Enabled");
  int pollIndex;                                      // Poll index is declared
  boolean flag = false;
  for (pollIndex = 0; pollIndex < MAX_POLL_INPUTS; pollIndex++) // Loop is run for max poll times
  {
 //   gDebugMode && DISPLAY_SERIAL.println("Lp");
    if((pollingIOs[pollIndex].pin == pin))  //checks whether already enabled or not
    {
      gDebugMode && DISPLAY_SERIAL.println("Pin is already Enabled");
      break;
    }
      if(pollingIOs[pollIndex].pin == 0)// If pin number is zero
      {
  //    gDebugMode && DISPLAY_SERIAL.println("PinCheck");
  //    gDebugMode && DISPLAY_SERIAL.println(state);
  //    gDebugMode && DISPLAY_SERIAL.println(pin);
      pollingIOs[pollIndex].pin = pin;                // Store the pin  
      pollingIOs[pollIndex].checkState = state;            // Store the state to be compared with
      gDebugMode && DISPLAY_SERIAL.println(state);
      //pollingIOs[pollIndex].curState = digitalRead(pollingIOs[pollIndex].pin);
      //pollingIOs[pollIndex].prevState = digitalRead(pollingIOs[pollIndex].pin);
       pollElements.push(1);                           // Push one input to input queue
       pollingIOs[pollIndex].flag=1;
       gDebugMode && DISPLAY_SERIAL.println("Polling Enabled");
      flag = true;                                    // If pin and state is updated
      break;
    }
    
  }
  if(flag == false )                                            // if flag is true
  {                             // flag is rest
    gDebugMode && DISPLAY_SERIAL.println("Poll Limit Reached! Failed!"); // Poll limit reached   
  }
}

void resetPins()
{
   int pollIndex;                                      // Poll index is declared
   for (pollIndex = 0; pollIndex < MAX_POLL_INPUTS; pollIndex++) // Loop is run for max poll times
   {
      pollingIOs[pollIndex].pin = 0;                // Store the pin  
      pollingIOs[pollIndex].checkState = 0;            // Store the state to be compared with
      pollingIOs[pollIndex].curState = 0;
      pollingIOs[pollIndex].prevState = 0;
   }
}
