/**
  Generated main.c file from MPLAB Code Configurator

  @Company
    Microchip Technology Inc.

  @File Name
    main.c

  @Summary
    This is the generated main.c using PIC24 / dsPIC33 / PIC32MM MCUs.

  @Description
    This source file provides main entry point for system initialization and application code development.
    Generation Information :
        Product Revision  :  PIC24 / dsPIC33 / PIC32MM MCUs - 1.171.1
        Device            :  dsPIC33CK256MP508
    The generated drivers are tested against the following:
        Compiler          :  XC16 v1.70
        MPLAB 	          :  MPLAB X v5.50
 */

/*
    (c) 2020 Microchip Technology Inc. and its subsidiaries. You may use this
    software and any derivatives exclusively with Microchip products.

    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
    WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
    PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION
    WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION.

    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
    BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
    FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
    ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
    THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.

    MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE
    TERMS.
 */

/**
  Section: Included Files
 */

#define BIT_TIME 562
#define IR_START_HIGH_COUNT  16
#define IR_START_LOW_COUNT  8
#define LED_ADDR 0x4008

#define ADDR_SIZE 16


#define FCY 4000000
#include "mcc_generated_files/system.h"
#include "mcc_generated_files/tmr1.h"
#include "mcc_generated_files/pwm.h"

#include <libpic30.h>
#include "ir_remote.h"
#include "mcc_generated_files/pin_manager.h"
#include "mcc_generated_files/pwm_module_features.h"

typedef struct IR_Struct_t {
    IR_STATES_t IR_STATES;
    uint8_t pause_train;
    uint8_t TMR_Elapsed_Count;
    uint16_t addr;
    uint8_t data;
    uint8_t red;
    uint8_t blue;
    uint8_t green;
   

} IR_Struct_t;

IR_Struct_t IR_Struct;

void TMR1_BitTime_Callback() {

    IR_Struct.TMR_Elapsed_Count++;

}

void Init_IR_System();
void CARRIER_ON();
void CARRIER_OFF();
void send_bit(uint32_t address);
/*
                         Main application
 */
int main(void) {
    // initialize the device
    SYSTEM_Initialize();

    Init_IR_System();
    
    uint32_t local_addr = 0;
    uint8_t bit_index = 0;
    
    while (1) {
        
        switch (IR_Struct.IR_STATES) {
            case IR_STATE_IDLE:
                if (BTN_GetValue() == 0)
                    IR_Struct.IR_STATES = IR_STATE_START_HIGH;
                break;
                
            case IR_STATE_START_HIGH:
                CARRIER_ON();
                if(IR_Struct.TMR_Elapsed_Count == IR_START_HIGH_COUNT)
                    {
                    CARRIER_OFF();
                    IR_Struct.TMR_Elapsed_Count = 0;
                    IR_Struct.IR_STATES = IR_STATE_START_LOW;
                    }
                break;
                
            case IR_STATE_START_LOW:
                TMR1_Start();
                if(IR_Struct.TMR_Elapsed_Count == IR_START_LOW_COUNT)
                    {
                    TMR1_Stop();
                    IR_Struct.TMR_Elapsed_Count = 0;
                    IR_Struct.IR_STATES = IR_STATE_TRANSMIT_ADDR;
                    
                    }
                break;
            case IR_STATE_TRANSMIT_ADDR:
               
                local_addr = IR_Struct.addr;
                while( bit_index < ADDR_SIZE) {
                    local_addr = local_addr << 1;
                    send_bit(local_addr);
                    bit_index++;
                }
                IR_Struct.TMR_Elapsed_Count  = 0;
                bit_index = 0;
                IR_Struct.IR_STATES = IR_STATE_TRANSMIT_DATA;
                
//            case IR_STATE_TRANSMIT_ADDR_REVERSED:
//                local_addr = ~IR_Struct.addr;
//                while( bit_index < ADDR_SIZE) {
//                   
//                    local_addr = local_addr << 1;
//                    send_bit(local_addr);
//                    bit_index++;
//                }
//                IR_Struct.TMR_Elapsed_Count  = 0;
//                bit_index = 0;
//                IR_Struct.IR_STATES = IR_STATE_TRANSMIT_DATA;
//                break;
            case IR_STATE_TRANSMIT_DATA:  
                Nop();
                break;
            default:
                break;
        }// Add your application code
    }
    return 1;
}

void Init_IR_System() {
    
    TMR1_SetInterruptHandler(&TMR1_BitTime_Callback);
    CARRIER_OFF();    
    IR_Struct.pause_train = 0;
    IR_Struct.TMR_Elapsed_Count = 0;
    IR_Struct.IR_STATES = IR_STATE_IDLE;
    IR_Struct.addr = LED_ADDR;

}

void CARRIER_OFF(){
    TMR1_Stop();
    PWM_GeneratorDisable(PWM_GENERATOR_1);
}

void CARRIER_ON(){
    TMR1_Start();
    PWM_GeneratorEnable(PWM_GENERATOR_1);
}
void send_bit(uint32_t address){

                    if(address & 0x0010000) //if it's a 1
                        {   
                            CARRIER_ON();
                            while(IR_Struct.TMR_Elapsed_Count != 1); //1 high
                            PWM_GeneratorDisable(PWM_GENERATOR_1);
                            while(IR_Struct.TMR_Elapsed_Count != 4); //3 low 
                            IR_Struct.TMR_Elapsed_Count  = 0;
                            TMR1_Stop();
                        }
                    else
                        {
                            CARRIER_ON();
                            while(IR_Struct.TMR_Elapsed_Count != 1); //1 high
                            PWM_GeneratorDisable(PWM_GENERATOR_1);
                            while(IR_Struct.TMR_Elapsed_Count != 2); //1 low 
                            IR_Struct.TMR_Elapsed_Count  = 0;
                            TMR1_Stop();
                        }
}
void SEND_1(){

}

void SEND_0(){

}
/**
 End of File
 */

