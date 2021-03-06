/********************* (C) COPYRIGHT 2017 e-Design Co.,Ltd. ********************
 Brief   : �ײ�Ӳ������                                            Author : bure
*******************************************************************************/
#ifndef __DRIVE_H
#define __DRIVE_H

#include "STM32F30x.h"

//============================================================================//

extern vu32 CntSec;
extern u8   StateA, StateB, CouplA, CouplB, GainA, GainB, KindA, KindB, Twink;
extern u16  KeyIn, Vbat, Ichg, Key_Exit;
extern s16  PopCnt, MenuCnt;
extern u8   Key_flag, Key_UPD;
extern u8   StdBy_Flag;
extern u8   StdBy_Key;
extern u16  Key_S_Time, Vb_Sum, Vbattrey;;

#define DONE            0
#define BLINK           1
#define OFF             0
#define ON              1
#define WAIT_TIMES      100000

//=====+========+========+========+========+========+========+========+========+
//Item:|��Ʒ�ͺ�|Ӳ���汾| MCU�ͺ�| LCD�ͺ�| LCD�ͺ�|FPGA�ͺ�| DFU�汾| OEM�汾|
//-----+--------+--------+--------+--------+--------+--------+--------+--------+
// RET:|   u8*  |   u8*  |   u8*  |   u8*  |   u8*  |   u8*  |   u8*  |   u8*  |
//-----+--------+--------+--------+--------+--------+--------+--------+--------+
//enum   {  PROD,     SCH,     MCU,     LCD,     ADC,    FPGA,     DFU,     OEM,
//=====+========+========+========+========+========+========+========+========+


//----------------------------- key code define ------------------------------//
#define K_RUN           0x02     /* RUN��             */
#define K_M             0x04     /* Bit2           M��*/
#define K_UP            0x08     /* Bit3           U��*/
#define K_DOWN          0x10     /* Bit4           D��*/
#define K_LEFT          0x20     /* Bit5           L��*/
#define K_RIGHT         0x40     /* Bit6           R��*/
#define K_S             0x80     /* Bit7           S��*/

#define KEY_DOUBLE_M    0x4000   /*M˫��*/
#define KEY_DOUBLE_S    0x8000   /*S˫��*/

#define R_HOLD          0x0200   /*RUN����*/
#define S_HOLD          0x2000   /*M����  */
#define M_HOLD          0x1000   /*S����  */

//----------------------------------------------------------------------------//
void USB_MSD_Config(void);
void Drive(u8 Device, u32 Value);
void SysTick_ISP(void);
void Wait_mS(u16 mS);
void AiPosi(u8 Val);
void BiPosi(u8 Val);
void Beep(u16 mS);
void Delay_mS(vu32 mS);
void Keys_Detect(void);
void ADC_Start(void);
void __USB_Port(u8 NewState);
void TIM6_ISP(void);
void Key_Init(void);
void GPIO_SWD_NormalMode(void);

u16  Bat_Vol(void);

#endif
/********************************* END OF FILE ********************************/
