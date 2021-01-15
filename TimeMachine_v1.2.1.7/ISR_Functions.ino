
// Operation indicators

#define WRITE_LOW       0                                           // Write Low action is 1  
#define WRITE_HIGH      1                                           // Write High action is 1
#define Input           0
#define Output          1
#define Skip_Mode       2
// Range of indices allocated in the array 
// Thousand is indexed to say isr attached to BUTTON(Recommended to not to use this index for anything else).  

void IsrHandler_2()                                                // Isr handler for interrupt on IO-18 
{
  gCurrentTick = micros();                                          // current micros()  
  gDeltaTick = gCurrentTick - gPastTick[2];                        // Find delta of micros()    
  
  if( gDeltaTick > InterruptInfo[2].debounceVal)                   // Checking for debounce
  {
    if(Run_Skip[2]>0)
    {
      --Run_Skip[2];
      gDebugMode && DISPLAY_SERIAL.println("sKIP iNTERRUPT");
    }
    else{
    if((InterruptInfo[2].intCount % InterruptInfo[2].skipTrig)==0)// Modulus operation for skip trigger operation
    {
      if(INTERRUPTPOLLING[0].flagPin==0)
      {
        INTERRUPTPOLLING[0].flagCompare=0;
        ++INTERRUPTPOLLING[0].flagPin;
        gDebugMode && DISPLAY_SERIAL.println("sKIP iNTERRUPT"); 
      }
      else{
        gTrigPin = 2;
      }
      //gTrigPin = 2;                                                // Trigger variable becomes 18
      gPastTick[2] = gCurrentTick;                                 // Current Tick becomes past tick
      gInputStartTime = gCurrentTick;                               // Start time variable is updated used for power off IO  
      gDebugMode && DISPLAY_SERIAL.println("ISR-2");               // Display    
    }
  }
     InterruptInfo[2].intCount++;                                  // Event Counter increments
  }
}

void IsrHandler_3()                                                // Isr handler for interrupt on IO-18 
{
  gCurrentTick = micros();                                          // current micros()  
  gDeltaTick = gCurrentTick - gPastTick[3];                        // Find delta of micros()    
  
  if( gDeltaTick > InterruptInfo[2].debounceVal)                   // Checking for debounce
  {
    if(Run_Skip[3]>0)
    {
      --Run_Skip[3];
      gDebugMode && DISPLAY_SERIAL.println("sKIP iNTERRUPT");
    }
    else{
    if((InterruptInfo[3].intCount % InterruptInfo[3].skipTrig)==0)// Modulus operation for skip trigger operation
    {
      if(INTERRUPTPOLLING[4].flagPin==0)
      {
        INTERRUPTPOLLING[4].flagCompare=0;
        ++INTERRUPTPOLLING[4].flagPin;
        gDebugMode && DISPLAY_SERIAL.println("sKIP iNTERRUPT"); 
      }
      else{
        gTrigPin = 3;
      }
      gPastTick[3] = gCurrentTick;                                 // Current Tick becomes past tick
      gInputStartTime = gCurrentTick;                               // Start time variable is updated used for power off IO  
      gDebugMode && DISPLAY_SERIAL.println("ISR-3");               // Display    
    }
  }
     InterruptInfo[3].intCount++;                                  // Event Counter increments
  }
}

void IsrHandler_20()                                                // Isr handler for interrupt on IO-18 
{
  gCurrentTick = micros();                                          // current micros()  
  gDeltaTick = gCurrentTick - gPastTick[20];                        // Find delta of micros()    
  
  if( gDeltaTick > InterruptInfo[20].debounceVal)                   // Checking for debounce
  {
    if(Run_Skip[20]>0)
    {
      --Run_Skip[20];
      gDebugMode && DISPLAY_SERIAL.println("sKIP iNTERRUPT");
    }
    else{
    if((InterruptInfo[20].intCount % InterruptInfo[20].skipTrig)==0)// Modulus operation for skip trigger operation
    {
      if(INTERRUPTPOLLING[5].flagPin==0)
      {
        INTERRUPTPOLLING[5].flagCompare=0;
        ++INTERRUPTPOLLING[5].flagPin;
        gDebugMode && DISPLAY_SERIAL.println("sKIP iNTERRUPT"); 
      }
      else{
        gTrigPin = 20;
      }
      gPastTick[3] = gCurrentTick;                                 // Current Tick becomes past tick
      gInputStartTime = gCurrentTick;                               // Start time variable is updated used for power off IO  
      gDebugMode && DISPLAY_SERIAL.println("ISR-20");               // Display    
    }
  }
     InterruptInfo[3].intCount++;                                  // Event Counter increments
  }
}

void IsrHandler_19()                                                // Isr handler for interrupt on IO-19 
{
  gCurrentTick = micros();                                          // current micros()  
  gDeltaTick = gCurrentTick - gPastTick[19];                        // Find delta of micros()    
  
  if( gDeltaTick > InterruptInfo[19].debounceVal)                   // Checking for debounce
  {
    if(Run_Skip[19]>0)
    {
      --Run_Skip[19];
      gDebugMode && DISPLAY_SERIAL.println("sKIP iNTERRUPT");
    }
    else{
    if((InterruptInfo[19].intCount % InterruptInfo[19].skipTrig)==0)// Modulus operation for skip trigger operation
    {
      if(INTERRUPTPOLLING[2].flagPin==0)
      {
        INTERRUPTPOLLING[2].flagCompare=0;
        ++INTERRUPTPOLLING[2].flagPin;
        gDebugMode && DISPLAY_SERIAL.println("sKIP iNTERRUPT"); 
      }
      else{
        gTrigPin = 19;
        //Run_Skip[2]=2;
      }      
      gPastTick[19] = gCurrentTick;                                 // Current Tick becomes past tick
      //flag19=true;
      gInputStartTime = gCurrentTick;                               // Start time variable is updated used for power off IO  
      gDebugMode && DISPLAY_SERIAL.println("ISR-19");               // Display    
     }
    }
     InterruptInfo[19].intCount++;                                  // Event Counter increments
  }
}

void IsrHandler_18()                                                // Isr handler for interrupt on IO-20 
{
  gCurrentTick = micros();                                          // current micros()  
  gDeltaTick = gCurrentTick - gPastTick[18];                        // Find delta of micros()    
  
  if( gDeltaTick > InterruptInfo[18].debounceVal)                   // Checking for debounce
  {
    if(Run_Skip[18]>0)
    {
      --Run_Skip[18];
      gDebugMode && DISPLAY_SERIAL.println("sKIP iNTERRUPT");
    }
    else{
    if((InterruptInfo[18].intCount % InterruptInfo[18].skipTrig)==0)// Modulus operation for skip trigger operation
    {
      if(INTERRUPTPOLLING[1].flagPin==0)
      {
        INTERRUPTPOLLING[1].flagCompare=0;
        ++INTERRUPTPOLLING[1].flagPin;
        gDebugMode && DISPLAY_SERIAL.println("sKIP iNTERRUPT"); 
      }
      else{
        gTrigPin = 18;
      }
      //gTrigPin = 18;                                                // Trigger variable becomes 20
      gPastTick[18] = gCurrentTick;                                 // Current Tick becomes past tick
      gInputStartTime = gCurrentTick;                               // Start time variable is updated used for power off IO  
      gDebugMode && DISPLAY_SERIAL.println("ISR-18");               // Display    
    }
    }
     InterruptInfo[18].intCount++;                                  // Event Counter increments
  }
}

void IsrHandler_21()                                                // Isr handler for interrupt on IO-21 
{
  gCurrentTick = micros();                                          // current micros()  
  gDeltaTick = gCurrentTick - gPastTick[21];                        // Find delta of micros()    
  
  if( gDeltaTick > InterruptInfo[21].debounceVal)                   // Checking for debounce
  {
    if(Run_Skip[21]>0)
    {
      --Run_Skip[21];
      gDebugMode && DISPLAY_SERIAL.println("sKIP iNTERRUPT");
    }
    else{
    if((InterruptInfo[21].intCount % InterruptInfo[21].skipTrig)==0)// Modulus operation for skip trigger operation
    {
     if(INTERRUPTPOLLING[3].flagPin==0)
      {
        INTERRUPTPOLLING[3].flagCompare=0;
        ++INTERRUPTPOLLING[3].flagPin;
        gDebugMode && DISPLAY_SERIAL.println("sKIP iNTERRUPT"); 
      }
      else{
        gTrigPin = 21;
      } 
      //gTrigPin = 21;                                                // Trigger variable becomes 21
      gPastTick[21] = gCurrentTick;                                 // Current Tick becomes past tick
      gInputStartTime = gCurrentTick;                               // Start time variable is updated used for power off IO  
      gDebugMode && DISPLAY_SERIAL.println("ISR-21");               // Display    
    }
    }
     InterruptInfo[21].intCount++;                                  // Event Counter increments
  }
}

void pollInputHandler(int pin)                                      // poll Input handler takes pin number as argument    
{
  gCurrentTick = micros();                                          // current micros()  
  gDeltaTick = gCurrentTick - gPastTick[pin];                       // Find delta of micros() 
  if( gDeltaTick > InterruptInfo[pin].debounceVal)                  // Checking for debounce
  {
    if(Run_Skip[pin]>0)
    {
      --Run_Skip[pin];
      gDebugMode && DISPLAY_SERIAL.println("sKIP iNTERRUPT");
    }
    else{
     //if((InterruptInfo[pin].intCount%InterruptInfo[pin].skipTrig)==0)// Modulus operation for skip trigger operation
    //{
      if(pin == gPowerOff)  { gExitApp = true;  }                   // Power off IO check    
      gTrigPin = pin;                                               // Trigger variable becomes 21
      gDebugMode && DISPLAY_SERIAL.println("pOLLING");
      gDebugMode && DISPLAY_SERIAL.println(pin);
      gPastTick[pin] = gCurrentTick;                                // Current Tick becomes past tick
      gInputStartTime = gCurrentTick;                               // Start time variable is updated used for power off IO  
    //}
    }
    InterruptInfo[pin].intCount++;                                  // Event Counter increments
  }
}

  void AChannelHandlerDirect()  // Called when A has changed
  {
    //(AState ^ BState) ? gEncoderCounter++ : gEncoderCounter--; //if A XOR B is true and A has changed, increment counter. If false ,decrement it.
    //AState = digitalRead(pinA);            // No need to read channel B because it's state has not changed since the last call to BChannelHandler().
    gEncoderCounter++;
  }
  void AChannelHandler()  // Called when A has changed
  {
    (AState ^ BState) ? gEncoderCounter++ : gEncoderCounter--; //if A XOR B is true and A has changed, increment counter. If false ,decrement it.
    AState = digitalRead(pinA);            // No need to read channel B because it's state has not changed since the last call to BChannelHandler().
  }

  void BChannelHandler()  // Called when B has changed
  {
    (AState ^ BState) ? gEncoderCounter-- : gEncoderCounter++;  //if A XOR B is true and B has changed, decrement counter. If false ,increment it.
    BState = digitalRead(pinB);            // No need to read channel A because it's state has not changed since the last call to AChannelHandler().
  }

  void ZChannelHandler()
  {
    gEncoderCounter = 0L;
  }
