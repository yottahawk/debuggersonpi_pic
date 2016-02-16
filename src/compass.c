/* 
 * File:   compass.c
 * Author: Harry
 *
 * Created on February 13, 2016, 11:21 PM
 */

/////////////////////////////////////INCLUDES///////////////////////////////////

#include "compass.h"

#include "math.h"

/////////////////////////////////////DEFINES////////////////////////////////////

#define PI 3.14

//////////////////////////////TYPEDEFS,ENUMS,STRUCTS////////////////////////////

//////////////////////////////////GLOBAL VARIABLES//////////////////////////////

// Externals
int currentheading;             // Value of current heading
int prevheadings[100];          // Buffer containing previous headings

// Internals
signed int X_axis;
signed int Y_axis;
signed int Z_axis;

int m_scale = 1;

///////////////////////////////FUNCTION DEFINITIONS/////////////////////////////

/* 
 * Initializes the compass over an i2c comms channel. 
 * The comms channel is opened within the init method.
 * The HMC5883L is set up as desired for the given application.
 */
void initCompass()
{
    periph_OpenI2C();
    
    periph_writeCompass(Config_Reg_A, Config_A_Data); 
    periph_writeCompass(Config_Reg_B, Config_B_Data); 
    periph_writeCompass(Mode_Reg, Mode_Data_Continuous);  
}

void selfTestCompass(); // Run self test and verify pass

void scaleAxis(); // Scale the output of each axis.
void setGain(set_gauss_scale value);     // Use an enum to set the gauss scale

int createIntFromChars(unsigned char MSB, unsigned char LSB)
{
    // Create int from two chars
    
    signed int val = 0; 
                               
    val = MSB; 
    val <<= 8;                          
    val |= LSB;          
    return val;    
}; 

void readCompassData()
{
    // Reads the relevant data to a buffer
    UINT8 data[6] = {0};
    
    UINT8 * data_ptr;
    data_ptr = &data[0];
    
    periph_StartI2C(); 
    periph_WriteByte(HMC5883L_ADDR | SLAVE_WRITE_TO);        // Write    
    periph_WriteByte(X_MSB_Reg);  
    periph_StartI2C(); 
    periph_WriteByte(HMC5883L_ADDR | SLAVE_READ_FROM);       // Read
    periph_ReadBytesAck( data_ptr , 6 );          
    periph_StopI2C();
    
    X_axis = createIntFromChars(data[0], data[1]);
    Z_axis = createIntFromChars(data[2], data[3]);
    Y_axis = createIntFromChars(data[4], data[5]); 
};     

void calculateHeading()
{
    // Takes the buffer data and calculates a heading
    readCompassData();
    
    int X_scaled = X_axis * m_scale;
    int Y_scaled = Y_axis * m_scale;
    int Z_scaled = Z_axis * m_scale;
    
    currentheading = atan2(Y_scaled , X_scaled);
    
    if(currentheading < 0.0) 
    { 
      currentheading += (2.0 * PI); 
    } 
    
    if(currentheading > (2.0 * PI))                
    {                            
      currentheading -= (2.0 * PI); 
    }                    
                    
   currentheading *= (180.0 / PI);
    
   // currentheading = currentheading + (int)declination_angle;
}

unsigned char periph_readCompass(unsigned char reg_address)
{
    // Read the value of a certain register
    UINT8 val; 
    
    periph_StartI2C();
    periph_WriteByte(HMC5883L_ADDR | SLAVE_WRITE_TO);
    periph_WriteByte(reg_address);

    periph_StartI2C();
    periph_WriteByte(HMC5883L_ADDR | SLAVE_READ_FROM);
    periph_ReadByte(&val);
    
    periph_StopI2C();
    
    return val;
} 

void periph_writeCompass(unsigned char reg_address, unsigned char value)
{
    // Writes to a register  
    periph_StartI2C();
    
    periph_WriteByte(HMC5883L_ADDR | SLAVE_WRITE_TO);
    periph_WriteByte(reg_address);
    periph_WriteByte(value);
    
    periph_StopI2C();
}