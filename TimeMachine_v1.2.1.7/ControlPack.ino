  
  /******************************************************************************************
    Name  :  ISR_fnPtr_t
    Type  :  function pointer
    Param@:  NoneArray
    Brief :  ISR for the 4 pins available to attach external interrupts are declared.
             Declared only for those which are recommended to be used as interrupts
    RetVal:  Base address of function
  *******************************************************************************************/
  void (*ISR_fnPtr_t[])() =                                                           // Function pointer containing all 4 possible ISR handlers
  { // 6 possible interrupts are 2,3,18,19,20,21
    IsrHandler_2,                                                                    // ISR for input IOs
    IsrHandler_18,                                                                    // ISR for input IOs
    IsrHandler_19,                                                                    // ISR for input IOs
    IsrHandler_21,                                                                     // ISR for input IOs
    IsrHandler_3,                                                                     // ISR for input IOs
    IsrHandler_20
  };
  
  /******************************************************************************************
    Name  :  ControlPack()
    Type  :  Function.
    Param@:  None
    Brief :  Interprets commands calls respective functions.
    RetVal:  Base address of function
  *******************************************************************************************/
  void ControlPack()
  {
    static unsigned short int intrIO;                                                   // Interrupt IO
    static unsigned short int pinStatus;                                                // Variable to hold pin status
    static char opCode[2];                                                              // Variable to store opcode
    static char readBuffer[2];                                                          // Pin Status buffer
    static unsigned short int flag1;                                                    // General flag1
    static unsigned short int flag2;                                                    // General flag2
    static unsigned short int pinNum;                                                   // Pin number being stored in this variable
    static unsigned int configIdx;                                                      // Config Index
    static unsigned int sizeBytes;                                                      // Size bytes used in command extraction from serial usb
    static unsigned long int pulseCounter;
    static char IoNum[4];
    
    // Reading Opcode from SerialUSB
    gDebugMode && DISPLAY_SERIAL.println("cntpk");                                      // Control pack
    DATA_SERIAL.readBytes(opCode, 1);                                                   // One byte of opcode is read
  
    pinNum = DATA_SERIAL.parseInt();                                                    // An integer read is pin number
    flag1 = pinNum / 10;                                                                // First digit of IO number
    flag2 = pinNum % 10;                                                                // Second digit of IO number
  
    gDebugMode && DISPLAY_SERIAL.println(opCode[0]);                                    // Opcode is printed
    gDebugMode && DISPLAY_SERIAL.println(pinNum);   // Pin number is printed
  //  if(opCode[0]==RUN_SKIP_TRIG)
  //  {
  //    runskip=1;
  //  }
  
    switch (opCode[0])                                                                  // Switch based on opcode
    {
      case  HANDSHAKE :             WriteToLabViEW("YES");                              // Reply back to LabView "YES"                                  
                                    ReInit(); break;                                    // Reinitialization of flags and Timer
      
      case  STOP_MOTOR :            gDebugMode && DISPLAY_SERIAL.println("stop motor"); // Display
                                    gMotor = false; break;                              // Stop motor
      
      case  STAMPING_IO_RCVD :      gDebugMode && DISPLAY_SERIAL.println("stamping IO");
                                    gStamping = pinNum; break;                          // Stamping output IO
      
      case  EXIT_APPLICATION :      gPowerOff = pinNum;                                 // Power Off IO updated and the status to compare
                                    gDebugMode && DISPLAY_SERIAL.println("Exit App");
                                    DATA_SERIAL.readBytes(readBuffer, 1);               // Read the separator out
                                    gPowerStatus = DATA_SERIAL.parseInt();              // gPowerStatus to compare in main loop
                                    flushPollQueue();
                                    gExitApp = true; break;                             // Exit application
      
      case  POWER_OUTPUT :          gDebugMode && DISPLAY_SERIAL.println("Pwr out");
                                    gPowerOut = pinNum; break;                          // Power output
      
      case  WINDING_OUTPUT :        gDebugMode && DISPLAY_SERIAL.println("Winding IO");
                                    gWinding = pinNum; break;                           // Winding output
      
      case  START_MOTOR :           gMotorPulse = pinNum; gMotor = true; break;         // Motor pulse IO number, Reeler motor is enabled
      
      case  PULSE_MODE :            gTrigPin= -1; intrIO= pinNum; pulseCounter= 0;      // gTrigger pin, IO number is updated                                                                   
                                    DATA_SERIAL.readBytes(readBuffer, 1);               // Read the separator out
                                    pinStatus = DATA_SERIAL.parseInt();                 // Read pin status which can either contain 0 or 1                                  
                                    gEncoderHandlerReady=true;
                                    while( pulseCounter < gDlyVal )                    // Motor steps for moving guide motor        
                                    {
                                      if(gEncoderHandlerReady)
                                      {
                                        gEventIndex = gIntrLinkHolder[gUserEventPin];                        // Index is got with pin number
                                        EventHandler();                                       // Event Handler is called    
                                        if((intrIO==gTrigPin) || digitalRead(intrIO)== pinStatus)  // Status of pin is checked as well as if trigger happend on the interrupt pin
                                        {
                                          pulseCounter = 0;  break;                        // Counter is reset
                                        }
                                        else                                              // If no trigger  
                                        {
                                          pulseCounter++;                                 // Counter increments(Keep pulsing)
                                        }
                                      }
                                    }                           
                                    gDebugMode && DISPLAY_SERIAL.println(pulseCounter);
                                    if(pulseCounter)                                    // If counter is non                                  
                                    {
                                      if(!intrIO)                                       // Interrupt IO will be zero for moving to ready to insert position which does mean no interrupts as motion terminator  
                                      {
                                        gDebugMode && DISPLAY_SERIAL.println("P");      // Display
                                        IoNum[0] = 'P';                                // 'P'
                                        IoNum[1] = '0';                                // '0'      
                                        IoNum[2] = '0';                                // '0'
                                      }
                                      else                                              // If intrIO is non-zero
                                      {
                                        gDebugMode && DISPLAY_SERIAL.println("W");      // Insufficient steps
                                        IoNum[0] = 'W';                                // 'W' - Insuffient steps indicator
                                        IoNum[1] = (intrIO / 10) + '0';                // IO number on which interrupt is expected
                                        IoNum[2] = (intrIO % 10) + '0';                // To generate common wrong home or wrong close command to LabViEW meaning insufficient steps
                                      }
                                      IoNum[3] = '\0';                                 // Null last character 
                                      WriteToLabViEW(IoNum);                           // Write to LabView
                                    }
                                    break;                                              // Break
  
      case  WRITE_IO_HIGH :         gDebugMode && DISPLAY_SERIAL.println("Wrt High");
                                    digitalWrite(pinNum,HIGH); break;                   // Function call to turn on output
  
      case  UPPER_SENSOR :          gDebugMode && DISPLAY_SERIAL.println("Upper Sensor");
                                    gUpperSensor = pinNum;  break;                      // Upper sensor IO number
  
      case  LOWER_SENSOR :          gDebugMode && DISPLAY_SERIAL.println("Lower Sensor");
                                    gLowerSensor = pinNum;  break;                      // Lower sensor IO number
  
      case  REELER_SENSOR :         gDebugMode && DISPLAY_SERIAL.println("Reeler Sensor");
                                    gReelSensor = pinNum; break;                        // Reeler sensor IO number  
  
      case  WRITE_IO_LOW :          gDebugMode && DISPLAY_SERIAL.println("Wrt Low");
                                    digitalWrite(pinNum,LOW); break;                    // Function call to turn off output
  
      case  WRITE_PIN_STATUS :      gDebugMode && DISPLAY_SERIAL.println("Write Pin Status");
                                    WritePinStatus(pinNum); break;                      // Function call to write IO status  
  
      case  PIN_MODE :              gDebugMode && DISPLAY_SERIAL.println("Pin mode");
                                    DATA_SERIAL.readBytes(readBuffer, 1);               // Read the separator out
                                    sizeBytes = DATA_SERIAL.parseInt();                 // Pin mode is set based on input
                                    SetPinMode(pinNum,sizeBytes);                       // Function which does set pin Mode is set 
                                    break;                                              // Break
  
      case  DEBUG_MODE :            gDebugMode && DISPLAY_SERIAL.println("Debug Mode");
                                    gDebugMode = pinNum; break;                         // Debug mode is set or reset
  
      case  USER_EVENT :            gEventIndex = gIntrLinkHolder[pinNum];              // Read the index of output action for the event
                                    gDebugMode && DISPLAY_SERIAL.println(gEventIndex);
                                    gUserEventPin = pinNum;                             // Update the IO for user event
                                    DATA_SERIAL.readBytes(readBuffer, 1);               // Read separator byte
                                    sizeBytes = DATA_SERIAL.parseInt();                 // Read mode selection byte 
                                    
                                    if(sizeBytes == CONTINUOUS_MODE)                    // If continuous mode is chosen in user event
                                    {
                                      gUserEventMode = CONTINUOUS_MODE;                 // Mode indicator gets continuous mode
                                      EventHandler();                                   // eventHandler() is called
                                      gDebugMode && DISPLAY_SERIAL.println("cont mode");// Display
                                    }
                                    else if(sizeBytes == ONE_SHOT)                      // If one shot mode is selected
                                    {
                                      gUserEventMode = ONE_SHOT;                        // Mode indicator gets one shot mode
                                      EventHandler();                                   // eventHandler() is called
                                      gDebugMode && DISPLAY_SERIAL.println("one shot"); // Display 
                                    }
                                    else                                                // Else  
                                    {
                                      gUserEventMode = TERMINATE_CONT_MODE;             // Terminate continuous mode which is being processed    
                                      gDebugMode && DISPLAY_SERIAL.println("stop");     // Display 
                                    }
                                    break;                                              // Break
                                    
      case  DISABLE_INTERRUPT :    
      {
                             switch(pinNum)
      
                                    {
                                      case INTR_1:    configIdx= pinNum; sizeBytes= 0;break; // Interrupt index for 20
                                      case INTR_2:    configIdx= pinNum; sizeBytes= 0;break; // Interrupt index for 2
                                      case INTR_3:    configIdx= pinNum; sizeBytes= 1;break; // Interrupt index for 21 
                                      case INTR_4:    configIdx= pinNum; sizeBytes= 2;break; // Interrupt index for 18
                                      case INTR_5:    configIdx= pinNum; sizeBytes= 3;break; // Interrupt index for 19
                                      case INTR_6:    configIdx= pinNum; sizeBytes= 4;break; // Interrupt index for 3
                                      default :       configIdx = pinNum; break;          // Input IO begins from 22
                                    }
                                    DATA_SERIAL.readBytes(readBuffer, 1);                   // Read the separator out
                                    if (readBuffer[0] == 'P')                               // 'P' for disabling polling inputs
                                    {
                                      disablePollInputs(pinNum); // Input pin number is disabled for polling interrupts 
                                      if(pollElements.count() >= MAX_POLL_INPUTS)
                                      {
                                        gDebugMode && DISPLAY_SERIAL.println("Poll i/p disabled");                                                                                         
                                                                      // Pop one element out from the polling queue    
                                      }
                                      else
                                      {
                                        gDebugMode && DISPLAY_SERIAL.println("Polling input limit reached");  
                                        gDebugMode && DISPLAY_SERIAL.println(pollElements.count()); 
                                      }
                                    }
                                    else
                                    {
                                      gDebugMode && DISPLAY_SERIAL.println("Interrupt disabled");
                                      detachInterrupt(digitalPinToInterrupt(pinNum));                          // ISR is being detached
                                     INTERRUPTPOLLING[sizeBytes].disabledPin=pinNum;  
                                     INTERRUPTPOLLING[sizeBytes].flagCompare=1;
                                    gDebugMode && DISPLAY_SERIAL.println("pIN IS sET");
                                    gDebugMode && DISPLAY_SERIAL.println(INTERRUPTPOLLING[sizeBytes].disabledPin);
                                    }
                                    break;                                              // Loop break
    }
  
      case  ENABLE_INTERRUPT :      
      {
                                      switch(pinNum)
      
                                    {
                                      case INTR_1:    configIdx= pinNum; sizeBytes= 5;break; // Interrupt index for 20
                                      case INTR_2:    configIdx= pinNum; sizeBytes= 0;break; // Interrupt index for 2
                                      case INTR_3:    configIdx= pinNum; sizeBytes= 1;break; // Interrupt index for 21 
                                      case INTR_4:    configIdx= pinNum; sizeBytes= 2;break; // Interrupt index for 18
                                      case INTR_5:    configIdx= pinNum; sizeBytes= 3;break; // Interrupt index for 19
                                      case INTR_6:    configIdx= pinNum; sizeBytes= 4;break; // Interrupt index for 3
                                      default :       configIdx = pinNum; break;          // Input IO begins from 22
                                    }
                                    DATA_SERIAL.readBytes(readBuffer, 1);               // Separator byte is read                                       
                                    InterruptInfo[configIdx].debounceVal = DATA_SERIAL.parseInt(); // Read debounce value                                  
                                    gDebugMode && DISPLAY_SERIAL.println(InterruptInfo[configIdx].debounceVal); // Display
                                    
                                    DATA_SERIAL.readBytes(readBuffer, 1) ;              // Separator byte is read
                                    InterruptInfo[configIdx].skipTrig = DATA_SERIAL.parseInt(); // Read skip trigger value
                                    gDebugMode && DISPLAY_SERIAL.println(InterruptInfo[configIdx].skipTrig); // Display
                                   
                                    DATA_SERIAL.readBytes(readBuffer, 1);               // A byte indicating Interrupts/Polling   
                                    INTERRUPTPOLLING[sizeBytes].flagCompare=0;
                                    INTERRUPTPOLLING[sizeBytes].flagLocal=0;   
                                    gDebugMode && DISPLAY_SERIAL.println("LOCAL");
                                    gDebugMode && DISPLAY_SERIAL.println(INTERRUPTPOLLING[sizeBytes].disabledPin);
                                    gDebugMode && DISPLAY_SERIAL.println(sizeBytes);
                                    switch(readBuffer[0])                               // 1 byte is read
                                    {
                                      case INTERRUPTS :                                 // Interrupts  
                                        
                                        DATA_SERIAL.readBytes(readBuffer, 1);           // A byte indicating interrupt configuration                                         
                                        switch(readBuffer[0])                             
                                        {  
                                          case RISING_EDGE :                            // AttachInterrupt for rising edge
                                            gDebugMode && DISPLAY_SERIAL.println("Rising Edge");
                                            attachInterrupt(digitalPinToInterrupt(pinNum),ISR_fnPtr_t[sizeBytes], RISING); 
                                            break;  
                                          
                                          case FALLING_EDGE :                           // AttachInterrupt for rising edge
                                            gDebugMode && DISPLAY_SERIAL.println("Falling Edge");
                                            attachInterrupt(digitalPinToInterrupt(pinNum),ISR_fnPtr_t[sizeBytes], FALLING); break; 
                                          
                                          case CHANGE_EDGE_TRANSITIONS :                // AttachInterrupt for rising edge
                                            gDebugMode && DISPLAY_SERIAL.println("Change");
                                            attachInterrupt(digitalPinToInterrupt(pinNum),ISR_fnPtr_t[sizeBytes], CHANGE); break;  
  
                                          default :
                                            gDebugMode && DISPLAY_SERIAL.println("Failed to attach Interrupt");   
                                            break;                            // Break                                            
                                        }
                                        break;
                                                   
                                      case  POLLING :                                   // Polling inputs
                                        gDebugMode && DISPLAY_SERIAL.println("Polling Input enabled");
                                        sizeBytes = DATA_SERIAL.parseInt();             // What state of input to compare with
                                        //if(pollElements.count() <= MAX_POLL_INPUTS)
                                        {
                                         enablePollInputs(pinNum,sizeBytes);           // Poll input is enabled with calling this function
                                        }
                                        //else
                                        {
                                          gDebugMode && DISPLAY_SERIAL.println("Polling input limit reached");   
                                        }
                                        break;
                                      default :                                         // Default do nothing
                                        gDebugMode && DISPLAY_SERIAL.println("Failed ENL");
                                        break;                                          // Break    
                                    }
                                    break;
      }
  //
            case BUTTON_SEQUENCE : {
                                   gPress=pinNum;
                                   Inspection_Event_Handler();
                                   gDebugMode && DISPLAY_SERIAL.println("bUTTON sEQUENCE");
                                   break;
                                   }
             case RUN_SKIP_TRIG : {
                                    //runskip=1;
                                    DATA_SERIAL.readBytes(readBuffer, 1);
                                    Run_Skip[pinNum] = DATA_SERIAL.parseInt();
                                    break;
                                   }
  
              case INIT_ENCODER_MOTOR :  
                                   {
                                    if(pinNum)
                                      encoderEnabled = true;
                                    else
                                      encoderEnabled = false;
                                    DATA_SERIAL.readBytes(readBuffer, 1); //separator
                                    encoderDebugEnabled = DATA_SERIAL.parseInt();
                                    if(encoderDebugEnabled)
                                      encoderDebugEnabled = true;
                                    else
                                      encoderDebugEnabled = false;
                                    DATA_SERIAL.readBytes(readBuffer, 1); //separator
                                    pinA = DATA_SERIAL.parseInt();
                                    DATA_SERIAL.readBytes(readBuffer, 1); //separator
                                    pinB = DATA_SERIAL.parseInt(); //Assign the channel B of encoder
                                    DATA_SERIAL.readBytes(readBuffer, 1); //separator
                                    gDebugMode && Serial.println(pinB);
                                    pinZ = DATA_SERIAL.parseInt(); //Assign the channel Z of the encoder
                                    gDebugMode && Serial.println(pinB);
                                    motSte = DATA_SERIAL.parseInt(); //Assign the motor steps pin
                                    DATA_SERIAL.readBytes(readBuffer, 1); //separator
                                    encoderResolution = DATA_SERIAL.parseInt();
                                    gDebugMode && Serial.println(encoderResolution);
                                    DATA_SERIAL.readBytes(readBuffer,1);
                                    encoderChannel = DATA_SERIAL.parseInt();
                                    pinMode(pinZ, INPUT_PULLUP);
                                    pinMode(motSte, OUTPUT); // Configure the motor steps as an output pin
                                    attachInterrupt(digitalPinToInterrupt(pinZ), ZChannelHandler, RISING);
                                    switch(encoderChannel)
                                    {
                                      case 0 : 
                                      {
                                        pinMode(pinA, INPUT_PULLUP);
                                        AState = digitalRead(pinA);
                                        attachInterrupt(digitalPinToInterrupt(pinA), AChannelHandlerDirect, RISING);
                                        break;
                                      }

                                      case 1 :
                                      {
                                        pinMode(pinA, INPUT_PULLUP);
                                        pinMode(pinB, INPUT_PULLUP);
                                        AState = digitalRead(pinA);
                                        BState = digitalRead(pinB);
                                        attachInterrupt(digitalPinToInterrupt(pinA), AChannelHandler, CHANGE);
                                        attachInterrupt(digitalPinToInterrupt(pinB), BChannelHandler, CHANGE);
                                        break;
                                      }
                                      default :
                                      {
                                        gDebugMode && Serial.println("Ecdr chnl dflt");
                                        pinMode(pinA, INPUT_PULLUP);
                                        AState = digitalRead(pinA);
                                        attachInterrupt(digitalPinToInterrupt(pinA), AChannelHandlerDirect, RISING);
                                      }
                                    }
                                    getEncoderInitialPosition();
                                    break;
                                   }
  
              case SET_TRIGGER_ANGLE :
                                   {
                                    init_angle = pinNum;
                                    init_angle = (float)init_angle / 360 * encoderResolution;
                                    DATA_SERIAL.readBytes(readBuffer, 1); //separator
                                    trigger_angle = DATA_SERIAL.parseInt();
                                    trigger_angle = (float)1 / trigger_angle * encoderResolution;
                                    DATA_SERIAL.readBytes(readBuffer, 1);//separator
                                    dummyPin = DATA_SERIAL.parseInt();
                                    Serial.println(trigger_angle);
                                    moving_target = init_angle;
                                    while(moving_target < gEncoderCounter)
                                    {
                                      moving_target += trigger_angle;
                                    }
                                    DATA_SERIAL.readBytes(readBuffer, 1); //separator
                                    acceptableZError = DATA_SERIAL.parseInt();
                                    break;
                                   }

                case GET_DEBUG_TIME :
                                   {
                                    int i = 0;
                                    /*for(i=0;i<NUMBER_OF_DEBUG_ELEMENTS;i++)
                                    {
                                      Serial.println(debugTimeArray[i]);
                                    }*/
                                    for(i=0;i<NUMBER_OF_DEBUG_ELEMENTS;i++)
                                    {
                                      Serial.println(debugSequenceTimeArray[i]);
                                      //Serial.println(debugDelayTimeArray[i]);
                                    }
                                    debugIndex = 0;
                                    debugFlag = false;
                                   }
                                   
              default :             gDebugMode && DISPLAY_SERIAL.println("Default");
                                    break;
      }                        
  }                                  
              
  
  
  
  
  

