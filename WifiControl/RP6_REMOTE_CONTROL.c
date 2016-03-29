#include "RP6M256Lib.h" 		// M256 Lib

#include "RP6I2CmasterTWI.h"	// I2C Master Lib

/*****************************************************************************/
/*****************************************************************************/
// Include the "RP6 Control M256 I2C Master library".
// This allows I2C control of the Robot base in a nearly identical API as
// on the Robot Base. 

#include "RP6M256_I2CMasterLib.h"  
/****************************************************************************/
/**
 * WIFI receiver. 
 * This checks if a whole input line has been received.
 *  
 */
uint8_t getInputLine_WIFI(void)
{ 
	if(getBufferLength_WIFI()) 						
		return parseLine_WIFI(readChar_WIFI());
	return 0;
}


/*****************************************************************************/

// Behaviour command type:

#define IDLE  0
#define RANGE 15  // used for the light sensors

// The behaviour command data type:
typedef struct {
	uint8_t  speed_left;  // left speed *** both used for 
	uint8_t  speed_right; // right speed *** turning commands
	unsigned dir:2;       // direction (FWD, BWD, LEFT, RIGHT)
	unsigned move:1;      // move flag // if it move
	unsigned rotate:1;    // rotate flag // or if it will rotate and move
	uint16_t move_value;  // move value is used for distance and angle values
	uint8_t  state;       // state of the behaviour
} behaviour_command_t;

behaviour_command_t STOP = {0, 0, FWD, false, false, 0, IDLE};   
//The STOP behaviour//

/*****************************************************************************/
/*****************************************************************************/
// Behaviour WIFI:

#define WIFI_ACTIVE 1
#define WIFI_EXECUTE_CMD 2

behaviour_command_t wifiControl = {0, 0, FWD, 
									false, false, 0, IDLE};


char* message_beginning = "*CLOS**OPEN*";

/**
 * This function allows the remote controll according the received commands from the WLAN 
 * module
 * This behaviour first waits to be activated, then it surpresses all other behaviours 
 * (except for the low battery behaviour).
 * 
 */
void wifi_Control(void)
{	
	/*writeString("wifi_Control()\n");
	while(true)
	{
	if(getInputLine_WIFI())
         {
			writeString("getInputLine_WIFI()\n");
			if(strcmp(receiveBuffer_WIFI,"*CLOS**OPEN*cmd")==0)
            {
			   writeString("cmd received !\n");
            }
            else
            {
			   writeString("other string received\n");
			   writeString(receiveBuffer_WIFI);
			   writeString("\n");
            }
         }
	}*/
	
	static uint8_t speedl = 40;
	static uint8_t speedr = 40;
	
	switch(wifiControl.state)
	{
		case IDLE: 
			if(getInputLine_WIFI())
			{
				writeString("receiveBuffer_WIFI = ");
				writeString(receiveBuffer_WIFI);
				writeString("\n");
				if(strcmp(receiveBuffer_WIFI, strcat(message_beginning,"connection"))==0)
				{
					writeString("Connection established\n");
					wifiControl.state = WIFI_ACTIVE;
					break;
				}else{
					writeString("Connection failed\n");
				}
			}
		break;
		case WIFI_ACTIVE: 
			if(getInputLine_WIFI())
			{
				writeString("receiveBuffer_WIFI = ");
				writeString(receiveBuffer_WIFI);
				writeString("\n");
				if(strcmp(receiveBuffer_WIFI, "fwd")==0)
				{
					wifiControl.speed_left = speedl;
					wifiControl.speed_right = speedr;
					writeString("Forward command received\n"); 
					wifiControl.dir = FWD;
				}
				else if(strcmp(receiveBuffer_WIFI, "bwd")==0)
				{
					wifiControl.speed_left = speedl;
					wifiControl.speed_right = speedr;
					writeString("Backward command received\n"); 
					wifiControl.dir = BWD;
				}
				else if(strcmp(receiveBuffer_WIFI, "left")==0)
				{
					if(speedl/2 >= 30) 
					{
						wifiControl.speed_left = speedl / 2;   // reduce speed for rotation otherwise
						wifiControl.speed_right = speedr / 2;  // it can be quite fast
					}
					else if(speedl > 10)
					{
						wifiControl.speed_left = 30;
						wifiControl.speed_right = 30;
					}
					writeString("Left command received\n"); 
					wifiControl.dir = LEFT;
				}
				else if(strcmp(receiveBuffer_WIFI, "right")==0)
				{
					if(speedl/2 >= 30) 
					{
						wifiControl.speed_left = speedl / 2;
						wifiControl.speed_right = speedr / 2;
					}
					else if(speedl > 10)
					{
						wifiControl.speed_left = 30;
						wifiControl.speed_right = 30;
					}
					writeString("Right command received\n"); 
					wifiControl.dir = RIGHT;
				}
				else if(strcmp(receiveBuffer_WIFI, "deconnection")==0)
				{
					writeString("Deconnection\n");
					wifiControl.state = IDLE;
				}
			}
		break;
		default:
			wifiControl.state = IDLE;
			writeString("Back to IDLE state\n");
		break;
	}
}

/**
Checks to see if any obstacle is found 
if not it will continue
else it will warn the user
**/
void warnForObs(void){

if(obstacle_right && obstacle_left)
	writeString("Obsahead");
else if(obstacle_left) 
	writeString("Obsleft");
else if(obstacle_right)
	writeString("Obsright");

}


/**
 * Timed Watchdog display - the other heartbeat function
 * does not work in this example as we use blocked moving functions here.
 */
void watchDogRequest(void)
{
	static uint8_t heartbeat2 = false;
	if(heartbeat2)
	{
		heartbeat2 = false;
	}
	else
	{
		setCursorPosLCD(1, 14); 
		heartbeat2 = true;
	}
}

/*****************************************************************************/
// Behaviour control:

/**
 * This function processes the movement commands that the behaviours generate. 
 * Depending on the values in the behaviour_command_t struct, it sets motor
 * speed, moves a given distance or rotates.
 */
void moveCommand(behaviour_command_t * cmd)
{
	if(cmd->move_value > 0)  // move or rotate?
	{
		writeString("cmd->move_value > 0\n");
		if(cmd->rotate){
			rotate(cmd->speed_left, cmd->dir, cmd->move_value, false);
			writeString("rotate\n");
		}else if(cmd->move){
			move(cmd->speed_left, cmd->dir, DIST_MM(cmd->move_value), false);
			writeString("move\n");
		}
		cmd->move_value = 0; // clear move value - the move commands are only
		                     // given once and then runs in background.
		/*writeString("Left speed : ");
		writeInteger(cmd->speed_left,DEC);
		writeString("\n");
		
		writeString("Right speed : ");
		writeInteger(cmd->speed_right,DEC);
		writeString("\n");*/
	}
	else if(!(cmd->move || cmd->rotate)) // just move at speed? 
	{
		//writeString("!(cmd->move || cmd->rotate)\n");
		changeDirection(cmd->dir);
		moveAtSpeed(cmd->speed_left,cmd->speed_right);
		
		/*writeString("Direction : ");
		writeInteger(cmd->dir,DEC);
		writeString("\n");
		
		writeString("Left speed : ");
		writeInteger(cmd->speed_left,DEC);
		writeString("\n");
		
		writeString("Right speed : ");
		writeInteger(cmd->speed_right,DEC);
		writeString("\n");*/
	}
	else if(isMovementComplete()) // movement complete? --> clear flags!
	{
		writeString("mouvement complete\n");
		cmd->rotate = false;
		cmd->move = false;
	}
}
#define BATTERY_LOW 1
behaviour_command_t checkLowBattery = {0, 0, FWD, 
									false, false, 0, IDLE};

/*This function sends a string to the control interface about the levels of 
brightness in the area*/

void light_detection(void){

	if(	adcLSL <RANGE && adcLSR < RANGE){
		writeString(" DARK");
	} else{
		writeString("BRIGHT");
	}

}

void behaviour_checkLowBattery(void)
{
}

/**
 * This is the Event Handler and it gets called when the Battery Voltage
 * is getting low! The Parameter isVoltageLow is true, when the voltage
 * is low and false, when the voltage is OK.
 */
 
void batteryVoltageLow(uint8_t isVoltageLow)
{

    if(isVoltageLow)
	{

		writeString("LOW Battery");
		writeIntegerLength_WIFI(adcBat,DEC,4);
	}
	else
	{
		writeString("Battery Ok!");
		writeIntegerLength_WIFI(adcBat,DEC,4);
	}
}

/**
 * The behaviourController task controls the subsumption architechture. 
 * It implements the priority levels of the different behaviours. 
 *
 *
 */
void behaviourController(void)
{
    // Call all the behaviour tasks:
	wifi_Control();

    // Execute the commands depending on priority levels:
	if(checkLowBattery.state != IDLE) // Highest priority - 2
	{
		//writeString("Priority 2\n");
		
	}else if(wifiControl.state != IDLE) // Priority - 1
	{
		//while(strcmp(receiveBuffer_WIFI, "stop")!=0){
		writeString("speed_left = ");
		writeInteger(wifiControl.speed_left,DEC);
		writeString(" speed_right = ");
		writeInteger(wifiControl.speed_right,DEC);
		writeString("\n");
		
		writeString("dir = ");
		writeInteger(wifiControl.dir,DEC);
		writeString(" move = ");
		writeInteger(wifiControl.move,DEC);
		writeString(" rotate = ");
		writeInteger(wifiControl.rotate,DEC);
		writeString(" move value = ");
		writeInteger(wifiControl.move_value,DEC);
		writeString(" state = ");
		writeInteger(wifiControl.state,DEC);
		writeString("\n");
		
		moveCommand(&wifiControl);
		//	wifi_Control();
		//}
		//writeString("Priority 1\n");
	}
	else                     // Lowest priority - 0
	{
		//writeString("command STOP\n");
		moveCommand(&STOP);  // Default command - do nothing!
		//writeString("Priority 0\n");
	}
}



// I2C Requests: 

/**
 * The I2C_requestedDataReady Event Handler
 */
void I2C_requestedDataReady(uint8_t dataRequestID)
{
	checkRP6Status(dataRequestID);
}

/*****************************************************************************/
// I2C Error handler

/**
 * This function gets called automatically if there was an I2C Error like
 * the slave sent a "not acknowledge" (NACK, error codes e.g. 0x20 or 0x30).
 */
void I2C_transmissionError(uint8_t errorState)
{
	writeString("\nI2C ERROR - TWI STATE: 0x");
	writeInteger(errorState, HEX);
	writeChar('\n');
}

/*****************************************************************************/
// Main function - The program starts here:

int main(void)
{
	initRP6M256(); 
	initLCD();

	// ---------------------------------------
	WDT_setRequestHandler(watchDogRequest); 

	// ---------------------------------------
	I2CTWI_initMaster(100);  
	I2CTWI_setRequestedDataReadyHandler(I2C_requestedDataReady);
	I2CTWI_setTransmissionErrorHandler(I2C_transmissionError);


	setLEDs(0b1111);
	mSleep(1000);
	setLEDs(0b0000);
	
	// ---------------------------------------
	// Setup ACS power:
	I2CTWI_transmit3Bytes(I2C_RP6_BASE_ADR, 0, CMD_SET_ACS_POWER, ACS_PWR_OFF);
	// Enable Watchdog for Interrupt requests:
	I2CTWI_transmit3Bytes(I2C_RP6_BASE_ADR, 0, CMD_SET_WDT, true);
	// Enable timed watchdog requests:
	I2CTWI_transmit3Bytes(I2C_RP6_BASE_ADR, 0, CMD_SET_WDT_RQ, true);

	startStopwatch1();
	startStopwatch2();
	startStopwatch3();
	
	while(true) 
	{ 
		/*writeIntegerLength_WIFI(adcBat,DEC,4);
		mSleep(600);
		warnForObs();
		mSleep(600);
		task_checkINT();
		mSleep(600);
		light_detection();
		mSleep(600);
	    task_I2CTWI();
		mSleep(600);*/
		behaviourController();
	}
	return 0;
}

