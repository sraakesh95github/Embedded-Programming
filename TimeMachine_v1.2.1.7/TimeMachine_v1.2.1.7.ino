
 
/**************************************************HEADER***********************************************************
  PROJECT NAME :  TIME MACHINE MDUINO

  DESCRIPTION  :  Generic Code which can handle any kind of logical or event operations or sequence of operations.

  PRIVACY      :  Encrypted to use only for Zentron Projects.

  NOTES        :  This code has Interrupt Routines defined for all 44 available pins on arduino due.
                  Serial communication is made with LabViEW to achieve some functionalities.


  [ BELOW ARE THE PROTOCOLS TO PREVENT TIME MACHINE ]
  RECOMMENDED TO AVOID THE FOLLOWING,
**** Print statements inside ISR functions.
**** Not to use pin number 0 and 1 for interrupts.
**** Communication over the serialUSB getting overflown(basically when more than 64bytes).
**** Touching base board while machine is running.
**** Need to recheck PinNumber 45 of arduino as input interrupts.

  AUTHOR       :  SHASHIDHAR H N ACHARYA             START DATE :    19 Jan 2016

  CHANGES AND UPDATIONS:

   VERSION      DATE        WHO         DETAIL
   T.02         23FEB16     SHASHI      ISR execution happens in Loop to avoid default interrupts generated
*********************************************************************************************************************/

#include<QueueList.h>                                                         // Included Queue List

#include"TimerOne.h"                                                          // TimerOne included
//#include"TimerTwo.h"
#include"TimerThree.h"                                                        // TimerThree included    
#include"TimerFour.h"                                                         // TimerFour included
#include"TimerFive.h"                                                         // TimerFive included

// Function declarations
void patch(void);                                                             // All patches go in here
void EventHandler(void);                                                      // Events are handled here
void TurnOffTimer(void);                                                      // Turning off timer
void EventHandler2(void);                                                     // Events are handled here
void lastDelayConfigHandler(void);                                            // Last delay in the configuration is handled  
void SerialFlush(void);                                                       // Data at Serial port is cleared
void pulseOutput(void);                                                       // Pulsing output  
void WriteToLabViEW(char *);                                                  // Write to LabView  
void pullConnector(void);                                                     // Pulling connector for reel motor  
void pollInputHandler(int);                                                   // Poll inputs are handled
void IsrHandler_18(void);                                                     // ISR handler for IO 18
void IsrHandler_19(void);                                                     // ISR handler for IO 19
void IsrHandler_20(void);                                                     // ISR handler for IO 20
void IsrHandler_21(void);                                                     // ISR handler for IO 21
void ReInit(void);                                                            // Reinitialization of flags  
void WritePinStatus(int);                                                     // Pin status is written to LabView
void SetPinMode(int, int);                                                    // PinMode is set for the IOs    
void disablePollInputs(int);                                                  // Disabling all polling inputs    
void enablePollInputs(int, int);                                              // Enabling all polling inputs      
void flushPollQueue(void);
void getEncoderInitialPosition(void);
//void Inspection_Event_Handler(void);

#define   DATA_SERIAL             Serial                                      // Serial USB to communicate over                                          
#define   DISPLAY_SERIAL          Serial                                      // Serial USB to communicate over    
#define   IS_SERIAL_EMPTY         -1                                          // Check for data on Serial USB port     
#define   BUTTON_HANDLER          1000                                        // Button handler to configure index with  
#define   BUTTON_PRESS_DURATION   5000000                                     // How long to hold button pressed for powering off machine                                      
#define   POWER_OFF_IO            21                                          // Power off button IO number
#define   MAX_POLL_INPUTS         10                                           // Maximum number of poll inputs

#define   CONTROL_PACK            '0'                                         // Control pack >> 01
#define   DATA_PACK               '1'                                         // Data pack >> 10
#define   HANDSHAKE               'A'                                         // Handshake command >> QRY
#define   STOP_MOTOR              'B'                                         // Stop motor >> SMT
#define   STAMPING_IO_RCVD        'C'                                         // Stamping IO received >> STM
#define   EXIT_APPLICATION        'D'                                         // Exit Application >> EXT  
#define   POWER_OUTPUT            'E'                                         // Power output IO >> PWR  
#define   WINDING_OUTPUT          'F'                                         // Winding output IO >> WND  
#define   START_MOTOR             'G'                                         // Start motor >> SST   
#define   PULSE_MODE              'H'                                         // Loop and pulse output >> LPI
#define   WRITE_IO_HIGH           'I'                                         // Write IO high >> HGH
#define   WRITE_IO_LOW            'J'                                         // Write IO low >> LOW      
#define   UPPER_SENSOR            'K'                                         // Upper sensor >> S1U
#define   LOWER_SENSOR            'L'                                         // Lower sensor >> S1L
#define   REELER_SENSOR           'M'                                         // Reeler sensor >> S2U
#define   WRITE_PIN_STATUS        'N'                                         // Write pin status >> PST
#define   PIN_MODE                'O'                                         // Pin Mode configuration >> PMD
#define   DEBUG_MODE              'P'                                         // Debug mode to enable print >> DBG
#define   CLEAR_TIME_MACHINE      'Q'                                         // Clear Time Machine >> CLR
#define   USER_EVENT              'R'                                         // User event from LabView >> EVT
#define   DISABLE_INTERRUPT       'S'                                         // Disable interrupt >> DBL
#define   ENABLE_INTERRUPT        'T'                                         // Enable interrupt >> ENL
#define   EVENT_CONFIGURATION     'U'                                         // Event config turation >> ENT  
#define   LINK_INDEX_IO           'V'                                         // Link index >> SET
#define   LOOP_COUNT              'W'                                         // Value for the loop count >> DLY
#define   CONTINUOUS_MODE         2                                           // Continuous mode for user event
#define   ONE_SHOT                1                                           // One shot mode for user event
#define   TERMINATE_CONT_MODE     0                                           // Turn off continuous pulsing
#define   INTR_1                  20   
#define   INTR_2                  2                                           // Interrupt 2 is available on 2  
#define   INTR_3                  18                                          // Interrupt 18 is available on 3
#define   INTR_4                  19                                          // Interrupt 19 is available on 4  
#define   INTR_5                  21                                          // Interrupt 21 is available on 5
#define   INTR_6                  3                                           // Interrupt 3 is available on 6
#define   RISING_EDGE             'R'                                         // Rising edge for interrupts        
#define   FALLING_EDGE            'F'                                         // Falling edge for interrupts
#define   CHANGE_EDGE_TRANSITIONS 'C'                                         // Change configuration for interrupts
#define   INTERRUPTS              'I'                                         // Interrupt indicator
#define   POLLING                 'P'                                         // Polling input configurations
#define   ENABLE                  'E'
#define   DISABLE                 'D'
#define   RUN_SKIP_TRIG           'X'                                         // To skip first trigger
#define   INSPECTION_CONFIGURATION 'Y'                                        // To store functions which is to be done after pushbutton press
#define   BUTTON_SEQUENCE         'Z'                                         // To give index of Inspection Sequence
#define   INIT_ENCODER_MOTOR      'i'                                         //Move the reeler motor to the index position
#define   SET_TRIGGER_ANGLE       'a'                                        //Disable encoder trigger
#define   GET_DEBUG_TIME          'g'                                         //Get debug time
#define   NUMBER_OF_DEBUG_ELEMENTS 500
/******************************************************************************************
  Name  :  Configurations
  Type  :  Union
  Param@:  None
  Brief :  Contains three types of structures: (1) logics (2) event (3) conditions.
  RetVal:  None
*******************************************************************************************/
struct interrupt
{
  unsigned long int debounceVal;                                              // Debounce time for an interrupt
  long int intCount;                                                          // Interrupt count
  unsigned int skipTrig;                                                      // Skip trigger for event handler
} 
InterruptInfo[53];                                                            // InterruptInfo

struct event
{
  unsigned short int pinNumber;                                               // Pin number
  unsigned short int action;                                                  // Action
  unsigned short int nextIndex;                                               // Next Action
  unsigned long int nextDelay;                                                // Delay after which next sequence begins
}
Event_Operations[35];                                                         // Excel config number of rows

struct pollInputs
{
  unsigned int pin;                                                           // Input PinNumber
  unsigned int prevState;                                                     // previous state of IO number  
  unsigned int curState;                                                      // current state of IO number
  unsigned int checkState;
  unsigned short int flag;
}
pollingIOs[MAX_POLL_INPUTS];                                                  // Number of polling inputs

struct Functions
{
  unsigned short int pinNumber;
  unsigned short int io;
  char ED[2];
  unsigned int Debounce_Delay;
  unsigned short int Skip_Trigger;
  unsigned short int Output_Mode;
  unsigned short int Initial_Skip;
  unsigned short int Next_Index;
  char IP[2];
  char RF[2];
}
Functions_Operations[60];

boolean gMotor = false;                                                       // Flag to enable or disable reeler motor
volatile boolean gExitApp = false;                                                     // App exit flag
boolean gDebugMode = false;                                                    // Debug mode used for data to write to PC
volatile boolean gTimerMode = false;                                                   // Timer Mode used for data to write to PC
volatile boolean gEncoderHandlerReady = true;
volatile boolean gEncoderHandler2Ready = true;
volatile int gTrigPin = 0;                                                             // Interrupt flag
volatile int gUserEventMode = 0;                                                       // User event is set to zero
volatile int gPowerOff = -1;                                                           // Power Off IO              
unsigned int gStamping = 0;                                                   // Stamping pin
unsigned int gWinding = 0;                                                    // Winding pin
unsigned int gUpperSensor = 0;                                                // Upper sag sensor
unsigned int gLowerSensor = 0;                                                // Lower sag sensor
unsigned int gReelSensor = 0;                                                 // Reel sensor IO number
unsigned int gPowerOut = 0;                                                   // Power output IO
unsigned int gReadUpper = 0;                                                  // Read upper sensor
unsigned int gReadLower = 0;                                                  // Read lower sensor
unsigned int gReadReeler = 0;                                                 // Read reeler sensor
unsigned int gMotorPulse = 0;                                                 // Read motor pulse
unsigned short int gMotorFlag=0;
volatile unsigned long int gInputStartTime = 0;                                             // Input start time is captured in this variable
volatile unsigned int gUserEventPin = 0;                                               // User event on IO
volatile unsigned int gIndex2 = 0;                                                     // Index used in event handler2
unsigned int gPowerStatus = 0;                                                // Power status being used for comparison    
volatile unsigned int gEventIndex = 0;                                                 // Event index is captured with it
volatile unsigned int gIntrLinkHolder[60];                                             // Interrupt link holder
unsigned long int gCurrentTick = 0;                                           // To capture current tick
unsigned long int gDeltaTick = 0;                                             // To capture delta tick
volatile unsigned long int gPastTick[53] = {0};                                        // To capture past tick
unsigned long int gDlyVal = 0;   // Number of steps being given is stored and used in pulsing in loop
int runskip = 0;
//int runskip = 0;
int gPress=0;
int Run_Skip[50]= {'\0'};
int Run_Skip_Compare[50] = {'0'};
int flag45 = 1;
int flagcompare=0;
int flaglocal=0;
volatile bool AState = LOW;
volatile bool BState = LOW;
volatile bool motFlag = false;
int motSte = 5;
long int stepResolution = 800;
long int encoderResolution = 1440;
int pinA = 18;
int pinB = 19;
int pinZ = 20;
long int stpCnt = 0L;
long int tmpCnt = 0L;
long int moving_target = 0L; //This is defined as Cm in the documentation
volatile long int gEncoderCounter = 11L; //This is defined as Cg in the documentation
long int init_angle = 0L; //This is defined as Co in the documentation
long int trigger_angle = 0L; //This is defined as Cp in the documentation
int dummyPin = 0;
bool encoderEnabled = false;
unsigned int encoderChannel = 1; //0 - Channel A only ; 1 - Both channels
unsigned long int debugTimeStart = 0;
unsigned long int debugTimeEnd = 0;
unsigned long int debugTime = 0;
unsigned long int debugTimeArray[NUMBER_OF_DEBUG_ELEMENTS];
bool debugFlag = false;
unsigned int debugIndex = 0;
unsigned long int debugSequenceTimeStart = 0L;
unsigned long int debugSequenceTime = 0L;
unsigned long int debugSequenceTimeArray[NUMBER_OF_DEBUG_ELEMENTS];
unsigned long int debugSequenceTimeIndex = 0L;
//unsigned long int debugDelayTimeStart = 0L;
//unsigned long int debugDelayTime = 0L;
unsigned long int debugDelayTimeArray[NUMBER_OF_DEBUG_ELEMENTS];
//unsigned long int debugDelayTimeIndex = 0L;*/
unsigned int acceptableZError = 0;
bool encoderDebugEnabled = false;

int MAX_INTERRUPTS=6;
struct POLLInterrupts
{
  int disabledPin;
  short int flagLocal;
  short int flagCompare;
  short int flagPin;
}
INTERRUPTPOLLING[5];

QueueList <unsigned long int> pollElements;                                   // Poll elements being the queue of polling inputs active at present

/***********************************************************************************************************************
    Some Predefined flags     Indices pre-allocated for them

    StartApplication          0
    StopApplication           1
    ClearApplication          2
    DebugInfo                 3
    DebugError                4
    IncomingBytes@Serial      5
    Skip triggers             6(PinNumber of sensor triggers makes one camera click)
    CurrentResultFlag         7
    Delay / Flag values       10-53
************************************************************************************************************************/

void setup()
{
  /******************************************************************************************
    Name  :  Serial Init
    Type  :  Init
    Param@:  Baudrate
    Brief :  Initializes Serial with the inputted baudrate.
    RetVal:  None
  *******************************************************************************************/

  DATA_SERIAL.begin(9600);                                                  // Serial port initialization with baudrate 115200
  DATA_SERIAL.setTimeout(5);                                                  // Serial port timeout being set
  patch(); 
  Timer1.attachInterrupt(EventHandler);                                       // Event Handler is attached for Timer1
  Timer4.attachInterrupt(EventHandler2);                                      // Event Handler is attached for Timer 4
  Timer5.attachInterrupt(lastDelayConfigHandler);                             // Last delay handler
  Timer3.attachInterrupt(lastDelayConfigHandler2);                                       // Turn off timer is attached for Timer 3
  delay(500);
  flushPollQueue();
  // Reading if there is some junk data serial port
  gEventIndex = DATA_SERIAL.available();                                            // If there is some data on serial port
  if(gEventIndex)                                                                   // If data present
  {
    SerialFlush();                                                             // Read it out from port
    gEventIndex = 0;
  }
  gTimerMode = true;
  delay(100); //init time for the incremental encoder
}

void loop()
{
  /******************************************************************************************
    Name  :  SerialUSB.available()
    Type  :  Function
    Param@:  None
    Brief :  Checks for serial data and calls functions accordingly.s
             Wrong data is getting flushed off and does interrupt service routine execution.
    RetVal:  Number of Bytes at SerialUSB.
  *******************************************************************************************/
  static int pollIndex;                                                       // Poll index is being taken
  static char readTail[2];                                                    // Read tail in this buffer
  static char readHead[2];
  static char IoNum[4];                                                       // IO number of 4 bytes size
  static int InterruptIndex;

  if(DATA_SERIAL.available())                                                 // If any data available
  {
    DATA_SERIAL.readBytes(readHead, 1);                                       // Read first byte
    gDebugMode && DISPLAY_SERIAL.println("AVL");
    switch(readHead[0])                                                       // Switch based on first byte
    {
      case DATA_PACK:     DataPack(); break;                                  // Data pack
      case CONTROL_PACK:  ControlPack();  break;                              // Control pack
      default :           gDebugMode && DISPLAY_SERIAL.println("dflt");       // Default case
                          SerialFlush(); break;                               // Flush serial port data
    }

    DATA_SERIAL.readBytes(readTail, 1);                                       // Read footer
    readTail[1] = '\0';                                                       // Last character is made null
  }
  else if (gExitApp && digitalRead(gPowerOff) == gPowerStatus)                // Power Off IO is checked to be zero
  {
    gDebugMode && DISPLAY_SERIAL.println("PWR");
    gCurrentTick = micros();                                                  // Current time is captured
    if (((gCurrentTick - gInputStartTime) > BUTTON_PRESS_DURATION ))          // Button press prolong duration is checked
    {
      pulseOutput();                                                          // Pulse output IO
      gExitApp = false;                                                       // Exit application flag is made false
    }
  }

  //Serial.println(micros());
  /*debugSequenceTimeArray[debugSequenceTimeIndex] = micros();
  if (debugSequenceTimeIndex < NUMBER_OF_DEBUG_ELEMENTS)
    debugSequenceTimeIndex++;*/
  /******************************************************************
    Handling ISR functionalities where gTrigPin = pin number
  ******************************************************************/
  if (gTrigPin > 1)                                                          // If a trigger happened
  {
    //Serial.println(gTrigPin);
  //  gDebugMode && DISPLAY_SERIAL.println("TRG");
  //  gDebugMode && DISPLAY_SERIAL.println(gTrigPin);
    gEventIndex = gIntrLinkHolder[gTrigPin];                                      // Array of indices of trigger pin
  //  gDebugMode && DISPLAY_SERIAL.println(gEventIndex);
    /*****************************************************************
      If Button Hanlder, write pin Number to LabViEW.
      Else get into execution of configurations .
    *****************************************************************/
    if (gEventIndex == BUTTON_HANDLER)                                            // If the input is a button
    {
      /** Extracting Pin Number as string **/
      IoNum[0] = 'P';                                                        // 'P' is sent with IO got triggered
      IoNum[1] = (gTrigPin / 10) + '0';                                      // IO number is converted to character
      IoNum[2] = (gTrigPin % 10) + '0';                                      // IO number is converted to character
      IoNum[3] = '\0';                                                       // Last character is made null

      WriteToLabViEW(IoNum);                                                 // Triggered IO number is written to Serial Port
    }
    else
    {
      EventHandler();                                                         // Event Handler is called
    }
    gTrigPin = 1;                                                             // Reset the flag back to original value
  }
  if(gMotor)                                                                 // If reeler motor is enabled
  {
    gReadUpper = digitalRead(gUpperSensor);                                   // Read upper sensor
    gReadLower = digitalRead(gLowerSensor);                                   // Read lower sensor
  //  gReadReeler = digitalRead(gReelSensor);                                   // Read reeler sensor
    if(gReadLower==1)
    {
      gMotorFlag=1;
    }
    if(gReadUpper==0)
    {
      gMotorFlag=0;
    }
    if(gMotorFlag)                              // Any sag is enabled
    {
      pullConnector(); // Pull connector
    }
  }

  if (!pollElements.isEmpty())                                                // If poll elements queue is non-empty
  {   
    for (pollIndex = 0; pollIndex < MAX_POLL_INPUTS; pollIndex++)             // Check for all inputs to be polled
    {
      if(pollingIOs[pollIndex].pin != 0)
      { 
//        pollingIOs[pollIndex].curState = digitalRead(pollingIOs[pollIndex].pin);                      // Current state is updated
//        if((pollingIOs[pollIndex].curState != pollingIOs[pollIndex].prevState))  // Read digital IO to be the decided state// (pollingIOs[pollIndex].curState == pollingIOs[pollIndex].checkState
//        {                                                // Trigger pin is updated
//        //  gDebugMode && DISPLAY_SERIAL.println(pollingIOs[pollIndex].checkState);
//           if(pollingIOs[pollIndex].curState == pollingIOs[pollIndex].checkState)
//           {
//              pollInputHandler(pollingIOs[pollIndex].pin);
//      //        gDebugMode && DISPLAY_SERIAL.println(pollingIOs[pollIndex].pin);
//           }
//            pollingIOs[pollIndex].prevState = digitalRead(pollingIOs[pollIndex].pin);  
//        }
if(pollingIOs[pollIndex].flag==1)
{ 
if(digitalRead(pollingIOs[pollIndex].pin)==1)
{
  pollInputHandler(pollingIOs[pollIndex].pin);
  gDebugMode && DISPLAY_SERIAL.println("DigitalRead");
  pollingIOs[pollIndex].flag=0;
}
}
else{
  if(digitalRead(pollingIOs[pollIndex].pin)==0)
{
  gDebugMode && DISPLAY_SERIAL.println("DigitalRead 0");
  pollingIOs[pollIndex].flag=1;
}
}
      }

    }
  }
for(InterruptIndex=0;InterruptIndex<MAX_INTERRUPTS;InterruptIndex++)
{
  if(INTERRUPTPOLLING[InterruptIndex].flagLocal==0)
  {
    
    if(INTERRUPTPOLLING[InterruptIndex].flagCompare==1)
    {
      if(digitalRead(INTERRUPTPOLLING[InterruptIndex].disabledPin)==1)
  {
  gCurrentTick = micros();                                          // current micros()  
  gDeltaTick = gCurrentTick - gPastTick[INTERRUPTPOLLING[InterruptIndex].disabledPin];                        // Find delta of micros()    
  
  if( gDeltaTick > InterruptInfo[INTERRUPTPOLLING[InterruptIndex].disabledPin].debounceVal)                   // Checking for debounce
  {
  gDebugMode && DISPLAY_SERIAL.println("dIGITALrEAD");
  INTERRUPTPOLLING[InterruptIndex].flagPin=0;
  gDebugMode && DISPLAY_SERIAL.println(INTERRUPTPOLLING[InterruptIndex].disabledPin);
  INTERRUPTPOLLING[InterruptIndex].flagLocal=1;
  }
      else
      {
        INTERRUPTPOLLING[InterruptIndex].flagPin=1;
      }
    }
    }
  }
}
if(encoderEnabled)
{
 /* debugDelayTimeArray[debugDelayTimeIndex] = gEncoderCounter;
  if (debugDelayTimeIndex < NUMBER_OF_DEBUG_ELEMENTS)
    debugDelayTimeIndex++;*/
   if(gEncoderCounter == 0) //Added logic to cancel the effect of initial zero setting during the Z trigger pulse.
    {
      if(moving_target>init_angle)
        moving_target=init_angle;
    }
  if (gEncoderCounter == moving_target)
  {
    //Serial.println(gEncoderCounter);
    gTrigPin = dummyPin;
    moving_target = moving_target + trigger_angle;
    if(moving_target > encoderResolution)
      moving_target = init_angle;
      //moving_target = moving_target - encoderResolution;
    //Serial.print("E : ");
    //Serial.print("M : ");
    //Serial.println(moving_target);
    if(encoderDebugEnabled)
    {
      if(gEncoderCounter > encoderResolution + acceptableZError || gEncoderCounter < encoderResolution - acceptableZError)
        WriteToLabViEW("ZNT");
    }
  }
}

}
