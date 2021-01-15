
void TurnOffTimer()                                     // Turn off timer
{
  Timer3.detachInterrupt();
  Timer3.stop();                                        // Timer3 is stopped
  if (gTimerMode)
  {
    digitalWrite(gPowerOut, LOW);                         // Power out is made LOW
  }
}

void pulseOutput()                                      // Pulse output
{
  Timer3.detachInterrupt();
  digitalWrite(gPowerOut, HIGH);                        // Power out is made HIGH
  Timer3.attachInterrupt(TurnOffTimer);
  Timer3.initialize(30000000);                           // Timer 3 is initialized
}


void Inspection_Event_Handler()
{
  static unsigned short int pinNum;                                                   // Pin number being stored in this variable
  // static unsigned int configIdx;                                                      // Config Index
  static unsigned int sizeBytes;                                                      // Size bytes used in command extraction from serial usb
  switch (Functions_Operations[gPress].io)
  {
    case Input   : {
        switch (Functions_Operations[gPress].pinNumber)
        {
          case INTR_1:     sizeBytes = 5; break; // Interrupt index for 20
          case INTR_2:     sizeBytes = 0; break; // Interrupt index for 2
          case INTR_3:     sizeBytes = 1; break; // Interrupt index for 21
          case INTR_4:     sizeBytes = 2; break; // Interrupt index for 18
          case INTR_5:     sizeBytes = 3; break; // Interrupt index for 19
          case INTR_6:     sizeBytes = 4; break; // Interrupt index for 3
          default :        break;          // Input IO begins from 22
        }

        switch (Functions_Operations[gPress].ED[0])                              // 1 byte is read
        {
          case ENABLE :   {                              // Interrupts
              if (Functions_Operations[gPress].IP[0] == INTERRUPTS)
              {
                InterruptInfo[Functions_Operations[gPress].pinNumber].debounceVal = Functions_Operations[gPress].Debounce_Delay ;
                InterruptInfo[Functions_Operations[gPress].pinNumber].skipTrig  = Functions_Operations[gPress].Skip_Trigger;
                INTERRUPTPOLLING[sizeBytes].flagCompare = 0;
                INTERRUPTPOLLING[sizeBytes].flagLocal = 0;
                switch (Functions_Operations[gPress].RF[0])
                {
                  case RISING_EDGE :                            // AttachInterrupt for rising edge
                    gDebugMode && DISPLAY_SERIAL.println("Rising Edge");
                    attachInterrupt(digitalPinToInterrupt(Functions_Operations[gPress].pinNumber), ISR_fnPtr_t[sizeBytes], RISING); break;

                  case FALLING_EDGE :                           // AttachInterrupt for rising edge
                    gDebugMode && DISPLAY_SERIAL.println("Falling Edge");
                    attachInterrupt(digitalPinToInterrupt(Functions_Operations[gPress].pinNumber), ISR_fnPtr_t[sizeBytes], FALLING); break;

                  case CHANGE_EDGE_TRANSITIONS :                // AttachInterrupt for rising edge
                    gDebugMode && DISPLAY_SERIAL.println("Change");
                    attachInterrupt(digitalPinToInterrupt(Functions_Operations[gPress].pinNumber), ISR_fnPtr_t[sizeBytes], CHANGE); break;

                  default :
                    gDebugMode && DISPLAY_SERIAL.println("Failed to attach Interrupt");
                    break;                            // Break
                }
              }
              else  {                                   // Polling inputs
                gDebugMode && DISPLAY_SERIAL.println("Polling Input enabled");
                sizeBytes = atoi(Functions_Operations[gPress].RF[0]);
                gDebugMode && DISPLAY_SERIAL.println(sizeBytes);
                enablePollInputs(Functions_Operations[gPress].pinNumber, sizeBytes);          // Poll input is enabled with calling this function
                if (pollElements.count() <= MAX_POLL_INPUTS)
                {
                  //pollElements.push(1);                           // Push one input to input queue
                  //gPollCount=gPollCount+1;

                }
                else
                {
                  gDebugMode && DISPLAY_SERIAL.println("Polling input limit reached");
                }
                break;
              }
            default :                                         // Default do nothing
              gDebugMode && DISPLAY_SERIAL.println("Failed ENL");
              break;                                          // Break
            }
          case DISABLE      :       {
              if (Functions_Operations[gPress].IP[0] == POLLING)                            // 'P' for disabling polling inputs
              {
                //if(pollElements.count() >= MAX_POLL_INPUTS)
                {
                  gDebugMode && DISPLAY_SERIAL.println("Poll i/p disabled");
                  disablePollInputs(Functions_Operations[gPress].pinNumber);                        // Input pin number is disabled for polling interrupts
                  gDebugMode && DISPLAY_SERIAL.println(Functions_Operations[gPress].pinNumber);
                  //pollElements.pop();                               // Pop one element out from the polling queue
                  //gPollCount=gPollCount-1;
                }
                //else
                {
                  //gDebugMode && DISPLAY_SERIAL.println("Polling input limit reached");
                }
              }
              else
              {
                gDebugMode && DISPLAY_SERIAL.println("Interrupt disabled");
                detachInterrupt(digitalPinToInterrupt(Functions_Operations[gPress].pinNumber));                          // ISR is being detached
                INTERRUPTPOLLING[sizeBytes].disabledPin = Functions_Operations[gPress].pinNumber;
                INTERRUPTPOLLING[sizeBytes].flagCompare = 1;
              }
              break;
            }
        }
        break;
      }
    case Output       :          {
        if (Functions_Operations[gPress].Output_Mode == 1)
        {
          digitalWrite(Functions_Operations[gPress].pinNumber, HIGH);
          gDebugMode && DISPLAY_SERIAL.println("HIGH");
        }
        else
        {
          digitalWrite(Functions_Operations[gPress].pinNumber, LOW);
          gDebugMode && DISPLAY_SERIAL.println("LOW");
        }
        break;
      }
    case Skip_Mode    :    {
        Run_Skip[Functions_Operations[gPress].pinNumber] = Functions_Operations[gPress].Initial_Skip;
        gDebugMode && DISPLAY_SERIAL.println("Run Skip");
        break;
      }
  }
  if (Functions_Operations[gPress].Next_Index)
  {
    gPress = Functions_Operations[gPress].Next_Index;
    Inspection_Event_Handler();
  }
  else
  {
    gPress = 0;
  }
}
/*****************************************************************************************************
    Name  :  EventHandler()
    Type  :  Function
    Param@:  None
    Brief :  A generic function to handle all kind of sequence of executions
             Does operations in a sequence stored in configuration.
             Knows the type of operation it has to do based on Index Range.
             Kinds of operations may be logical, event or conditional event operations
             It knows when to stop event execution by operation indicator
    RetVal:  None
*****************************************************************************************************/
void EventHandler()                                      // Event handler
{
  //debugSequenceTimeStart = micros();
  Timer1.stop();                                         // Timer 1 is stopped
  if (gTimerMode)
  {
    gEncoderHandlerReady = false;
    static int procIndex;                                  // Declare variable
    procIndex = gEventIndex;                               // Event index is assigned to Processing index
    /*debugSequenceTimeStart = micros();
    debugSequenceTimeArray[debugSequenceTimeIndex] = debugSequenceTimeStart;
    debugDelayTimeArray[debugSequenceTimeIndex] = Event_Operations[procIndex].pinNumber;
    if (debugSequenceTimeIndex < NUMBER_OF_DEBUG_ELEMENTS)
    debugSequenceTimeIndex++;*/
    //debugDelayTimeArray[debugDelayTimeIndex] = Event_Operations[procIndex].pinNumber;
    //if (debugDelayTimeIndex < NUMBER_OF_DEBUG_ELEMENTS)
       //debugDelayTimeIndex++;
    switch (Event_Operations[procIndex].action)            // Switch to action
    {
      case WRITE_LOW:                                      // Case write output low
        digitalWrite(Event_Operations[procIndex].pinNumber, LOW); // Write pin low
        //gDebugMode && DISPLAY_SERIAL.println(Event_Operations[procIndex].pinNumber);
        //gDebugMode && DISPLAY_SERIAL.println("L");
        break;                                             // Break

      case WRITE_HIGH:                                     // Case write output high
        digitalWrite(Event_Operations[procIndex].pinNumber, HIGH); // Write pin high
        //gDebugMode && DISPLAY_SERIAL.println(Event_Operations[procIndex].pinNumber);
        //gDebugMode && DISPLAY_SERIAL.println("H");
        break;                                             // Break

      default:                                             // Case default
        //gDebugMode && DISPLAY_SERIAL.println("DFLT");              // Display
        break;                                             // Break
    }
    gEventIndex = Event_Operations[procIndex].nextIndex;   // Next action index is assigned to global index
    if (gEventIndex)                                       // If next sequence exists
    {
      //debugSequenceTimePrevious = micros() - debugSequenceTimePrevious;
      //debugSequenceTimeArray[debugSequenceTimeIndex] =  debugSequenceTimePrevious;
      //debugSequenceTimeIndex ++;
      if (debugFlag == false)
        debugTimeStart = micros();
      debugFlag = true;
      if (Event_Operations[procIndex].nextDelay)           // If there is a delay before next action
      {
        Timer1.initialize(Event_Operations[procIndex].nextDelay); // Start Timer1 for the eventHandler execution
        //Timer1.start();
      }
      else                                                 // If end of sequence is hit
      {
        EventHandler();                                    // EventHandler function is called
      }
    }
    else                                                   // If last index
    {
      debugFlag == false;
      debugTimeEnd = micros();
      debugTimeArray[debugIndex] = debugTimeEnd - debugTimeStart;
      if (debugIndex < NUMBER_OF_DEBUG_ELEMENTS)
        debugIndex++;
      if (Event_Operations[procIndex].nextDelay)           // There is a next delay
      {
        Timer5.initialize(Event_Operations[procIndex].nextDelay); // Start Timer5 for causing just to cause delay
        //gDebugMode && DISPLAY_SERIAL.println("NxtDly");
      }
      else
      {
        //gDebugMode && DISPLAY_SERIAL.println("Imd call");
        lastDelayConfigHandler();
      }
    }
  }
}

/*****************************************************************************************************
    Name  :  PrintEventConfig()
    Type  :  Function
    Param@:  Index has event configuration stored
    Brief :  Prints event configured data. We get the index from the datum of SET command.(Arr[indx]=val)
    RetVal:  None
*****************************************************************************************************/
void printEventConfig(unsigned int index)                 // Print config index function
{
  //gDebugMode && DISPLAY_SERIAL.println("ENT");            // Event

  //gDebugMode && DISPLAY_SERIAL.println("PIN");            // Pin
  //gDebugMode && DISPLAY_SERIAL.println(Event_Operations[index].pinNumber); // Pin Number

  //gDebugMode && DISPLAY_SERIAL.println("ACTION[0]");      // Action on IO
  //gDebugMode && DISPLAY_SERIAL.println(Event_Operations[index].action); // Current action

  //gDebugMode && DISPLAY_SERIAL.println("ACTION[1]");      // Next action index
  // gDebugMode && DISPLAY_SERIAL.println(Event_Operations[index].nextIndex); // Next action index

  //gDebugMode && DISPLAY_SERIAL.println("NEXTDLY");        // Next delay
  // gDebugMode && DISPLAY_SERIAL.println(Event_Operations[index].nextDelay);  // Next delay
}

/******************************************************************************************************
    Name  :  pullConnector()
    Type  :  Function
    Param@:  None
    Brief :  Pulses motor with reading sensor as well as the command from LabVIEW to start motor.
    RetVal:  None
******************************************************************************************************/
void pullConnector()                                      // pull connector function
{
  /* if(gReadLower)                                          // Read lower sensor
    {
     if(digitalRead(gStamping) == HIGH)                    // Stamping is High
     {
       digitalWrite(gStamping, LOW);                       // Stamping is low
       gDebugMode && DISPLAY_SERIAL.println("SOff");       // Display
     }
    }
    else                                                    // If no reeler
    {
     if(digitalRead(gStamping) == LOW)                     // If stamping is low
     {
       digitalWrite(gStamping, HIGH);                      // stamping HIGH
       gDebugMode && DISPLAY_SERIAL.println("SOn");        // On
     }
    }

    if(gReadReeler)                                         // If reeler is on
    {
     if (digitalRead(gWinding) == LOW)                     // If Winding is low
     {
       digitalWrite(gWinding, HIGH);                       // Write winding high
       gDebugMode && DISPLAY_SERIAL.println("WOn");        // Winding on display
     }
    }
    else                                                    // if reeler is not on
    {
     if (digitalRead(gWinding) == HIGH)                    // If Winding is high
     {
       digitalWrite(gWinding, LOW);                        // Write Winding LOW
       gDebugMode && DISPLAY_SERIAL.println("WOff");       // Winding is turned off
     }
    }*/

  if (gReadUpper == 1)                                    // If upper switch is active
  {

    //gDebugMode && DISPLAY_SERIAL.println("HI");
    if (gEncoderHandler2Ready)
    {
      //  gDebugMode && DISPLAY_SERIAL.println("ST");
      gIndex2 = gIntrLinkHolder[gMotorPulse];
      EventHandler2();
    }                                         // Move motor function is called
  }
}

void EventHandler2()                                      // Event handler
{
  Timer4.stop();                                         // Timer 1 is stopped
  if (gTimerMode)
  {
    gEncoderHandler2Ready = false;
    static int procIndex;                                  // Declare variable
    procIndex = gIndex2;                               // Event index is assigned to Processing index

    switch (Event_Operations[procIndex].action)            // Switch to action
    {
      case WRITE_LOW:                                      // Case write output low
        digitalWrite(Event_Operations[procIndex].pinNumber, LOW); // Write pin low
        //gDebugMode && DISPLAY_SERIAL.println(Event_Operations[procIndex].pinNumber);
        //gDebugMode && DISPLAY_SERIAL.println("L");
        break;                                             // Break

      case WRITE_HIGH:                                     // Case write output high
        digitalWrite(Event_Operations[procIndex].pinNumber, HIGH); // Write pin high
        // gDebugMode && DISPLAY_SERIAL.println(Event_Operations[procIndex].pinNumber);
        // gDebugMode && DISPLAY_SERIAL.println("H");
        break;                                             // Break

      default:                                             // Case default
        //gDebugMode && DISPLAY_SERIAL.println("DFLT");              // Display
        break;                                             // Break
    }
    gIndex2 = Event_Operations[procIndex].nextIndex;   // Next action index is assigned to global index
    if (gIndex2)                                       // If next sequence exists
    {
      if (Event_Operations[procIndex].nextDelay)           // If there is a delay before next action
      {
        Timer4.initialize(Event_Operations[procIndex].nextDelay); // Start Timer1 for the eventHandler execution
      }
      else                                                 // If end of sequence is hit
      {
        EventHandler2();                                    // EventHandler function is called
      }
    }
    else                                                   // If last index
    {
      if (Event_Operations[procIndex].nextDelay)           // There is a next delay
      {
        Timer3.initialize(Event_Operations[procIndex].nextDelay); // Start Timer5 for causing just to cause delay
        // gDebugMode && DISPLAY_SERIAL.println("NxtDly");
      }
      else
      {
        // gDebugMode && DISPLAY_SERIAL.println("Imd call");
        lastDelayConfigHandler2();
      }
    }
  }
}
/*****************************************************************************************************
    Name  :  EventHandler()
    Type  :  Function
    Param@:  None
    Brief :  A generic function to handle all kind of sequence of executions
             Does operations in a sequence stored in configuration.
             Knows the type of operation it has to do based on Index Range.
             Kinds of operations may be logical, event or conditional event operations
             It knows when to stop event execution by operation indicator
    RetVal:  None
*****************************************************************************************************/
void patch()                                              // Patch function
{
  pinMode(41, OUTPUT);                                    // Output
  pinMode(42, OUTPUT);                                    // Output
  pinMode(43, OUTPUT);                                    // Output
  pinMode(7, OUTPUT);                                    // Output
  pinMode(12, OUTPUT);                                    // Output
  pinMode(13, OUTPUT);                                    // Output
  pinMode(6, OUTPUT);                                    // Output

  digitalWrite(41, LOW);                                 // Reinit IO
  digitalWrite(42, LOW);                                 // Reinit IO
  digitalWrite(43, LOW);                                 // Reinit IO
  digitalWrite(06, LOW);                                 // Reinit IO
  digitalWrite(7, LOW);                                 // Reinit IO
  digitalWrite(12, LOW);                                 // Reinit IO
  digitalWrite(13, LOW);                                 // Reinit IO
  INTERRUPTPOLLING[0].flagPin = 1;
  INTERRUPTPOLLING[1].flagPin = 1;
  INTERRUPTPOLLING[2].flagPin = 1;
  INTERRUPTPOLLING[3].flagPin = 1;
  INTERRUPTPOLLING[4].flagPin = 1;
}

void ReInit()
{
  gPowerOff = -1;                                         // Power Off IO is reset back to -1
  gUserEventMode = TERMINATE_CONT_MODE;                   // User event mode is reset
  Timer3.attachInterrupt(lastDelayConfigHandler2);
  flushPollQueue();
}

void lastDelayConfigHandler()                             // Last config delay Handler
{
  gEncoderHandlerReady = true;
  Timer5.stop();
  if (gTimerMode)                                         // Stop timer 5
  {
    if (gUserEventMode == CONTINUOUS_MODE)                  // If continous mode is enabled,
    {
      gEventIndex = gIntrLinkHolder[gUserEventPin];         // Event Index is got
      EventHandler();                                       // Call event Handler
    }
  }
}

void lastDelayConfigHandler2()                             // Last config delay Handler
{
  gEncoderHandler2Ready = true;
  Timer3.stop();
  if (gTimerMode)                                         // Stop timer 5
  {
    if (gUserEventMode == CONTINUOUS_MODE)                  // If continous mode is enabled,
    {
      gIndex2 = gIntrLinkHolder[gMotorPulse];         // Event Index is got
      EventHandler();                                       // Call event Handler
    }
  }
}

void flushPollQueue()
{
  while (1)
  {
    if (!pollElements.isEmpty())
    {
      pollElements.pop();
    }
    else
    {
      break;
    }
  }
}
/*
  void motor_run_revolution()
  {
  if (tmpCnt < stepResolution)
  {
    digitalWrite(motSte, HIGH);
    delayMicroseconds(1000);
    digitalWrite(motSte, LOW);
    delayMicroseconds(1000);
    tmpCnt++;
  }
  }

  void motor_run_angle()
  {
  if (tmpCnt < stpCnt)
  {
    digitalWrite(motSte, HIGH);
    delayMicroseconds(1000);
    digitalWrite(motSte, LOW);
    delayMicroseconds(1000);
    tmpCnt++;
  }
  }

  void motor_set_index()
  {
  while (index_flag == false)
  {
    if (tmpCnt < stepResolution)
    {
      digitalWrite(motSte, HIGH);
      delayMicroseconds(1000);
      digitalWrite(motSte, LOW);
      delayMicroseconds(1000);
      tmpCnt++;
    }
  }
  index_flag = false;
  }*/

  void getEncoderInitialPosition()
  {
    Timer5.attachInterrupt(runMotor);
    Timer5.initialize(500); //The speed of the motor is 150 RPM for a steps per revolutions of 400
  }

  void runMotor()
  {
    digitalWrite(motSte,!motFlag);
    motFlag=!motFlag;
    if(gEncoderCounter < 10) //Stop the encoder at Z position
    {
      Timer5.detachInterrupt();
      Timer5.attachInterrupt(lastDelayConfigHandler);
    }
  }

