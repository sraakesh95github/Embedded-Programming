
/*****************************************************************************************************
* Name  :  DataPack()
* Type  :  Function
* Param@:  None
* Brief :  Gets index and Configuration event operations and stores them in right places
* RetVal:  None
*****************************************************************************************************/

void DataPack()
{
    static char opCode[2];                                                                  // Opcode                                        
    static unsigned int configIdx;                                                          // Config index
    static char readBuffer[2];
    
    gDebugMode && DISPLAY_SERIAL.println("Dtpk");                                   // Display
   //Reading Opcode from DATA_SERIAL
    DATA_SERIAL.readBytes(opCode,1);                                                        // Read a byte of opcode
    
    gDebugMode && DISPLAY_SERIAL.println(opCode);                                   // Display
    configIdx = DATA_SERIAL.parseInt();                                                     // Index to which we are writing configuration
    gDebugMode && DISPLAY_SERIAL.println(configIdx);
/**********************************************************************************************************
* Name  :  ENT
* Type  :  If condition. 
* Param@:  None
* Brief :  Gets configurations of event operations and stores it in array of events.
* RetVal:  None
************************************************************************************************************/    
/*************************************************************************************************************
* Name  :  SET
* Type  :  If condition. 
* Param@:  None
* Brief :  Sets the index of event sequence operations.
           (Recommended to send this exactly after first operation of sequence is sent)
* RetVal:  None
*************************************************************************************************************/      
/*************************************************************************************************************
* Name  :  DLY
* Type  :  If condition. 
* Param@:  None
* Brief :  Delay to be stored in this variable
* RetVal:  None
*************************************************************************************************************/      
    switch(opCode[0])
    {
      case  EVENT_CONFIGURATION :         DATA_SERIAL.readBytes(readBuffer, 1);             // Read separator byte
                                          Event_Operations[configIdx].pinNumber = DATA_SERIAL.parseInt(); // Pin Number is read
                                          gDebugMode && DISPLAY_SERIAL.print("Pin: ");      // Display
                                          gDebugMode && DISPLAY_SERIAL.println(Event_Operations[configIdx].pinNumber); // Display pin number
                                          
                                          DATA_SERIAL.readBytes(readBuffer, 1);             // Read separator byte
                                          Event_Operations[configIdx].action = DATA_SERIAL.parseInt(); // Read Action
                                          gDebugMode && DISPLAY_SERIAL.print("Action: ");   // Display
                                          gDebugMode && DISPLAY_SERIAL.println(Event_Operations[configIdx].action); // Display
                                          
                                          DATA_SERIAL.readBytes(readBuffer, 1);             // Read Separator byte
                                          Event_Operations[configIdx].nextIndex = DATA_SERIAL.parseInt(); // Read next index
                                          gDebugMode && DISPLAY_SERIAL.print("Next Idx: ");   // Display
                                          gDebugMode && DISPLAY_SERIAL.println(Event_Operations[configIdx].nextIndex); // Display next index
                                          
                                          DATA_SERIAL.readBytes(readBuffer, 1);             // Read a byte
                                          Event_Operations[configIdx].nextDelay = DATA_SERIAL.parseInt(); // Delay before next action
                                          gDebugMode && DISPLAY_SERIAL.print("Delay: ");      // Display
                                          gDebugMode && DISPLAY_SERIAL.println(Event_Operations[configIdx].nextDelay); // Display delay
                                          break;                                            // Break
                                                  
      case  LINK_INDEX_IO :               gDebugMode && DISPLAY_SERIAL.print("Link ");      // Display
                                          DATA_SERIAL.readBytes(readBuffer, 1);             // Read separator byte
                                          gDebugMode && DISPLAY_SERIAL.print(configIdx);
                                          gDebugMode && DISPLAY_SERIAL.print(" ");
                                          gIntrLinkHolder[configIdx] = DATA_SERIAL.parseInt(); // Read and link pin number to action list
                                          gDebugMode && DISPLAY_SERIAL.println(gIntrLinkHolder[configIdx]);
                                          break;                                            // Break   
                                                  
      case  LOOP_COUNT :                  gDebugMode && DISPLAY_SERIAL.println("Loop count ");
                                          gDlyVal = configIdx; break;                       // Loop count is gotten   


      case INSPECTION_CONFIGURATION :     {
                                          gDebugMode && DISPLAY_SERIAL.println("Inspection");
                                          DATA_SERIAL.readBytes(readBuffer, 1);             // Read separator byte
                                          Functions_Operations[configIdx].pinNumber = DATA_SERIAL.parseInt(); // Pin Number is read
                                          gDebugMode && DISPLAY_SERIAL.print("Pin: ");      // Display
                                          gDebugMode && DISPLAY_SERIAL.println(Functions_Operations[configIdx].pinNumber); // Display pin number
                                          
                                          DATA_SERIAL.readBytes(readBuffer, 1);             // Read separator byte
                                          Functions_Operations[configIdx].io = DATA_SERIAL.parseInt(); // Read Action
                                          gDebugMode && DISPLAY_SERIAL.print("io: ");   // Display
                                          gDebugMode && DISPLAY_SERIAL.println(Functions_Operations[configIdx].io); // Display

                                          if(Functions_Operations[configIdx].io==0)
                                          {
                                                    DATA_SERIAL.readBytes(readBuffer, 1);             // Read separator byte
                                                    Functions_Operations[configIdx].ED[0]= readBuffer[0];            
                                                    gDebugMode && DISPLAY_SERIAL.print("Enable or Disable: ");   // Display
                                                    gDebugMode && DISPLAY_SERIAL.println(Functions_Operations[configIdx].ED[0]); // Display
                                          
                                                    Functions_Operations[configIdx].Debounce_Delay = DATA_SERIAL.parseInt(); // Read Action
                                                    gDebugMode && DISPLAY_SERIAL.print("Debouce Delay: ");   // Display
                                                    gDebugMode && DISPLAY_SERIAL.println(Functions_Operations[configIdx].Debounce_Delay); // Display
          
                                                    DATA_SERIAL.readBytes(readBuffer, 1);             // Read separator byte
                                                    Functions_Operations[configIdx].Skip_Trigger = DATA_SERIAL.parseInt(); // Read Action
                                                    gDebugMode && DISPLAY_SERIAL.print("Skip Trigger: ");   // Display
                                                    gDebugMode && DISPLAY_SERIAL.println(Functions_Operations[configIdx].Skip_Trigger); // Display
         
                                                    DATA_SERIAL.readBytes(readBuffer,1);
                                                    Functions_Operations[configIdx].IP[0]=readBuffer[0]; 
                                                    Functions_Operations[configIdx].IP[1]='\0';
                                                    gDebugMode && DISPLAY_SERIAL.println(Functions_Operations[configIdx].IP[0]);
          
                                                    DATA_SERIAL.readBytes(readBuffer,1);
                                                    Functions_Operations[configIdx].RF[0]=readBuffer[0];
                                                    Functions_Operations[configIdx].RF[1]='\0';
                                                    gDebugMode && DISPLAY_SERIAL.println(Functions_Operations[configIdx].RF[0]);
                                            
                                          }

                                          if(Functions_Operations[configIdx].io==1)
                                          {
                                          DATA_SERIAL.readBytes(readBuffer, 1);             // Read separator byte
                                          Functions_Operations[configIdx].Output_Mode = DATA_SERIAL.parseInt(); // Read Action
                                          gDebugMode && DISPLAY_SERIAL.print("Output Mode: ");   // Display
                                          gDebugMode && DISPLAY_SERIAL.println(Functions_Operations[configIdx].Output_Mode); // Displa
                                          }

                                          if(Functions_Operations[configIdx].io==2)
                                          {
                                          DATA_SERIAL.readBytes(readBuffer, 1);             // Read separator byte
                                          Functions_Operations[configIdx].Initial_Skip = DATA_SERIAL.parseInt(); // Read Action
                                          gDebugMode && DISPLAY_SERIAL.print("Initial Skip: ");   // Display
                                          gDebugMode && DISPLAY_SERIAL.println(Functions_Operations[configIdx].Initial_Skip); // Displa
                                          }

                                          //DATA_SERIAL.readBytes(readBuffer, 1);             // Read separator byte
                                          Functions_Operations[configIdx].Next_Index = DATA_SERIAL.parseInt(); // Read Action
                                          gDebugMode && DISPLAY_SERIAL.print("Next Index: ");   // Display
                                          gDebugMode && DISPLAY_SERIAL.println(Functions_Operations[configIdx].Next_Index); // Displa

                                          
                                          break;                                           // Break
    }
      default :                           gDebugMode && DISPLAY_SERIAL.println("Default");
                                          break;    
    }    
}  
