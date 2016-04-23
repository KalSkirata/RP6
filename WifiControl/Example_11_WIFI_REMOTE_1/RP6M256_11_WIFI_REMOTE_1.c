/* 
 * ****************************************************************************
 * RP6 ROBOT - Remote Control
 * ****************************************************************************
 * Example: WIFI_REMOTE 1.1 - WIFI Remote Control From a Android device
 * Author: Aikaterini Minardou
 * ****************************************************************************
 * Description:  
 *	In this program the WLAN Module receives the commands that are sent from the 
 *  the user's Android device and it is also programs the microcontroller 
 *	to continious update the user about sensor values (Light, Battery level,
 *  Bumpers state and  )
 *
*/
// Includes:

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
#define STOP_STATE 3

behaviour_command_t wifiControl = {0, 0, FWD, 
									false, false, 0, IDLE};


char* message_beginning = "*CLOS**OPEN*";
char* last_command = "";

#define SPEED_RIGHT 40
#define SPEED_LEFT 40

void move_right(void){
	wifiControl.speed_left = SPEED_LEFT;
	wifiControl.speed_right = SPEED_RIGHT/2;
	writeString("Right command received\n"); 
	wifiControl.dir = RIGHT;
	last_command = "right";
}

void move_left(void){
	wifiControl.speed_left = SPEED_LEFT / 2;
	wifiControl.speed_right = SPEED_RIGHT; 
	writeString("Left command received\n"); 
	wifiControl.dir = LEFT;
	last_command = "left";
}

void move_fwd(void){
	wifiControl.speed_left = SPEED_LEFT;
	wifiControl.speed_right = SPEED_RIGHT;
	writeString("Forward command received\n"); 
	wifiControl.dir = FWD;
	last_command = "fwd";
}

void move_bwd(void){
	wifiControl.speed_left = SPEED_LEFT;
	wifiControl.speed_right = SPEED_RIGHT;
	writeString("Backward command received\n"); 
	wifiControl.dir = BWD;
	last_command = "bwd";
}

/**
 * This function allows the remote controll according the received commands from the WLAN 
 * module
 * This behaviour first waits to be activated, then it surpresses all other behaviours 
 * (except for the low battery behaviour).
 * 
 */
void wifi_Control(void)
{	
	switch(wifiControl.state)
	{
		case IDLE: 
			/*if(getInputLine_WIFI())
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
			}*/
			wifiControl.state = WIFI_ACTIVE;
		break;
		case WIFI_ACTIVE:
			if(getInputLine_WIFI())
			{
				writeString("receiveBuffer_WIFI = ");
				writeString(receiveBuffer_WIFI);
				writeString("\n");
				if(strcmp(receiveBuffer_WIFI, "stop")==0)
				{
					wifiControl.state = STOP_STATE;
				}
				else if(strcmp(receiveBuffer_WIFI, "disconnection")==0)
				{
					writeString("Disconnection\n");
					wifiControl.speed_left = 0;
					wifiControl.speed_right = 0;
					wifiControl.state = IDLE;
					last_command = "";
				}
				else if(strcmp(receiveBuffer_WIFI, "fwd")==0)
				{
					move_fwd();
				}
				else if(strcmp(receiveBuffer_WIFI, "bwd")==0 || strcmp(receiveBuffer_WIFI, "*CLOS**OPEN*bwd")==0 || strcmp(receiveBuffer_WIFI, "*CLOS**OPEN**CLOS**OPEN*bwd")==0)
				{
					move_bwd();
				}
				else if(strcmp(receiveBuffer_WIFI, "left")==0  || strcmp(receiveBuffer_WIFI, "*CLOS**OPEN*left")==0 || strcmp(receiveBuffer_WIFI, "*CLOS**OPEN**CLOS**OPEN*left")==0)
				{
					move_left();
				}
				else if(strcmp(receiveBuffer_WIFI, "right")==0)
				{
					move_right();
				}
			}else{
				if(strcmp(last_command, "fwd")==0){
					move_fwd();
				}else if(strcmp(last_command, "bwd")==0){
					move_bwd();
				}else if(strcmp(last_command, "left")==0){
					move_left();
				}else if(strcmp(last_command, "right")==0){
					move_right();
				}
			}
		break;
		case STOP_STATE:
			writeString("Stop\n");
			
			wifiControl.speed_left = 0;
			wifiControl.speed_right = 0;
			
			if(getInputLine_WIFI()){
				writeString("receiveBuffer_WIFI = ");
				writeString(receiveBuffer_WIFI);
				writeString("\n");
				if(strcmp(receiveBuffer_WIFI,"start")==0)
				{
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
	}
	else if(!(cmd->move || cmd->rotate)) // just move at speed? 
	{
		changeDirection(cmd->dir);
		moveAtSpeed(cmd->speed_left,cmd->speed_right);	
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
		moveCommand(&wifiControl);
	}
	else                     // Lowest priority - 0
	{
		//writeString("command STOP\n");
		moveCommand(&STOP);  // Default command - do nothing!
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
		/*
		mSleep(600);
		warnForObs();
		mSleep(600);
		task_checkINT();
		mSleep(600);
		light_detection();
		mSleep(600);
	    task_I2CTWI();
		mSleep(600);*/
		writeIntegerLength_WIFI(adcBat,DEC,4);
		behaviourController();
	}
	return 0;
}

