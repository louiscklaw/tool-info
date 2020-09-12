/******************** (C) COPYRIGHT 2009 e-Design Co.,Ltd. *********************
* File Name: Function.c
     Author: bure
   Hardware: DS0201V1.1~1.6
    Version: V1.0
*******************************************************************************/

#include "Function.h"
#include "Lcd.h"
#include "Calculate.h"
#include "stm32f10x_lib.h"
#include "HW_V1_Config.h"
#include "ASM_Funtion.h"

//------------�����ѹ������ز�������------------

unsigned const char Item_V[20][11] = //����λ��ֱ��������ʾ�ַ�
//    0��        1��        2��        3��        4��       5��        6��        7��         8��        9,   ̽ͷ˥����1 ʱ
{"10mV/Div","20mV/Div","50mV/Div","0.1V/Div","0.2V/Div","0.5V/Div"," 1V/Div "," 2V/Div "," 5V/Div "," 10V/Div",
//    10��       11��      12��       13��       14��      15��       16��       17��        18��       19��  ̽ͷ˥����10ʱ
 "0.2V/Div","0.5V/Div"," 1V/Div "," 2V/Div "," 5V/Div "," 10V/Div"," 20V/Div"," 50V/Div","100V/Div"," -GND-  "};

unsigned const int V_Scale[20] =    //��ֱ�����ȵ�λ����ϵ��
// 0��  1��   2��   3��   4��    5��    6��    7��     8��     9,          ̽ͷ˥����1 ʱ
{400, 800, 2000, 4000, 8000, 20000, 40000, 80000, 200000, 400000,
// 10��   11��   12��   13��    14��    15��    16��     17��   18�� 19��  ̽ͷ˥����10ʱ
 8000, 20000, 40000, 80000, 200000, 400000, 800000, 2000000, 4000000, 0 };

unsigned short Km[20]=    //������ѹ����ϵ�� K = Km/4069
//  0,    1,   2,   3,    4,   5,   6,    7,   8,   9,   10,  11,  12,   13,  14,  15,   16,  17,  18, 19
{2956, 1478, 591, 296, 1114, 446, 223, 1157, 463, 231, 1452, 581, 290, 1082, 433, 216, 1048, 419, 210, 231};

//        Y_POSУ׼����1���壺10mV 20mV 50mV  .1V  .2V  .5V   1V   2V   5V  10V  .2V  .5V   1V   2V   5V  10V  20V  50V 100V default
unsigned short  Y_POSm[20]   ={ 20,  42, 104, 207,  55, 138, 275,  53, 132, 265,  42, 105, 212,  57, 142, 284,  59, 146, 294,   0};
//        Y_POSУ׼����2���壺10mV 20mV 50mV  .1V  .2V  .5V   1V   2V   5V  10V  .2V  .5V   1V   2V   5V  10V  20V  50V 100V default
         short  Y_POSn[20]   ={881, 799, 566, 180, 750, 439, -75, 758, 461, -37, 799, 563, 161, 743, 424,-109, 735, 409,-146, 956};

//------------ɨ��ʱ��������ز�������------------

unsigned const char Item_T[22][12] =  //����λˮƽɨ��ʱ����ʾ�ַ�
//    0           1           2            3           4           5           6           7           8          9           10
{" 1uS/Div "," 2uS/Div "," 5uS/Div "," 10uS/Div"," 20uS/Div"," 50uS/Div","100uS/Div","200uS/Div","500uS/Div"," 1mS/Div "," 2mS/Div ",
//    11          12          13           14          15          16          17         18          19          20          21
 " 5mS/Div "," 10mS/Div"," 20mS/Div"," 50mS/Div"," 0.1S/Div"," 0.2S/Div"," 0.5S/Div","  1S/Div ","  2S/Div ","  5S/Div "," 10S/Div "};

unsigned const int T_Scale[22] =    //ˮƽɨ��ʱ����λ����ϵ��
//    0           1           2            3           4           5           6           7           8          9           10
//{    1167,       1167,       1167,        1167,       1167,       2000,       4000,       8000,       20000,      40000,      80000,
{     40,         80,         200,        400,        800,        2000,       4000,      8000,       20000,     40000,      80000,
//    11          12          13           14          15          16          17         18          19          20          21
    200000,     400000,     800000,     2000000,    4000000,    8000000,    20000000,   40000000,   80000000,   200000000,  400000000};

unsigned const short Scan_PSC[22] =   //ˮƽɨ��ʱ�������Ԥ��Ƶ��ֵ-1
//    0           1           2            3           4           5           6           7           8          9           10
{11,/*40n*/  11,/*80n*/  11,/*200n*/  11,/*400n*/ 11,/*800n*/ 15,/*2u*/   15,/*4u*/   15,/*8u*/   15,/*20u*/  15,/*40u*/  15,/*80u*/
//    11          12          13           14          15          16          17         18          19          20          21
31,/*200u*/  63,/*400u*/ 63,/*800u*/  127,/*2m*/  255,/*4m*/  255,/*8m*/  255,/*20m*/ 511,/*40m*/ 511,/*80m*/511,/*0.2S*/1023,/*0.4S*/};

unsigned const short Scan_ARR[22] =   //ˮƽɨ��ʱ���������Ƶ��ֵ-1
//    0           1           2            3           4           5           6           7           8          9           10
{ 6,/*40n*/    6,/*80n*/    6,/*200n*/   6,/*400n*/  6,/*800n*/ 8 ,/*2u*/   17,/*4u*/   35,/*8u*/   89,/*20u*/  179,/*40u*/ 359,/*80u*/
//    11          12          13           14          15          16          17         18          19          20          21
449,/*200u*/ 449,/*400u*/ 899,/*800u*/ 1124,/*2m*/ 1124,/*4m*/ 2249,/*8m*/ 5624,/*20m*/ 5624,/*40m*/11249,/*80m*/28124,/*.2*/28124,/*.4*/};

unsigned const short Ks[22] =   //ˮƽɨ��ʱ���ֵϵ��
// 0     1     2    3    4    5    6    7    8    9    10   11   12   13   14   15   16   17   18   19   20   21
{ 29860,14930,5972,2986,1493,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024};

//------------��׼Ƶ�������ز�������------------

unsigned const char Item_F[16][7] =  //����λƵ�������ʾ�ַ�
//   0       1        2        3         4        5        6        7       8         9       10       11       12       13       14       15
{" 1MHz ","500KHz","200KHz","100KHz"," 50KHz"," 20KHz"," 10KHz"," 5KHZ "," 2KHz "," 1KHz "," 500Hz"," 200Hz"," 100Hz"," 50Hz "," 20 Hz"," 10Hz "};

unsigned const int Fout_PSC[16] =
//   0       1        2        3         4        5        6        7       8         9       10       11       12       13       14       15
{    0,      0,       0,       0,        0,       0,       0,       0,     4-1,      4-1,    16-1,    16-1,    64-1,    64-1,    128-1,   128-1};

unsigned const int Fout_ARR[16] =
//   0       1        2        3         4        5        6        7       8         9       10       11       12       13       14       15
{  72-1,   144-1,   360-1,   720-1,   1440-1,   3600-1,  7200-1,  14400-1, 9000-1,  18000-1, 9000-1,  22500-1, 11250-1, 22500-1, 28125-1, 56250-1};

//-----------------------------------------------------------------------------

unsigned short Scan_Buffer[0x1000]; //ɨ�����������
unsigned char  Signal_Buffer[300];  //�������ݻ�������0-299Ϊ����������300ΪY���̡�301ΪX����
unsigned char  View_Buffer[300];    //��ʾ���λ�������0-299Ϊ����������300ΪY���̡�301ΪX����
unsigned char  Ref_Buffer [304];
/*=   //�ο����建������0-299Ϊ����������300ΪY���̡�301ΪX����
  {100,116,130,144,157,167,175,181,185,185,184,179,173,164,153,141,128,114,100,86, 73, 60, 49, 40, 33, 27, 24, 24, 25, 29,
   35, 43, 52, 63, 75, 87, 100,112,124,135,145,153,160,164,167,167,166,163,157,150,142,133,122,111,100,89, 79, 70, 61, 54,
   48, 44, 42, 42, 43, 46, 50, 57, 64, 72, 81, 90, 100,109,118,126,133,139,144,147,149,149,148,146,142,137,130,124,116,108,
   100,93, 85, 79, 73, 68, 64, 61, 60, 60, 61, 63, 66, 70, 75, 81, 87, 93, 100,106,112,117,122,125,128,130,131,131,131,129,
   126,123,119,115,110,105,100,96, 91, 88, 84, 81, 79, 78, 77, 78, 78, 80, 82, 84, 87, 90, 93, 97, 100,103,106,108,110,112,
   113,114,114,113,113,112,110,109,107,105,104,102,100,99, 97, 96, 96, 95, 95, 95, 95, 96, 96, 97, 97, 98, 99, 99, 100,100,
   100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,
   100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,115,129,141,151,160,165,169,170,169,167,162,156,148,139,130,
   120,110,100,91, 82, 75, 68, 63, 59, 57, 56, 56, 58, 61, 64, 69, 75, 81, 87, 94, 100,106,112,117,121,125,127,129,130,130,
   128,127,124,121,117,113,109,104,100,96, 92, 88, 85, 83, 81, 80, 79, 79, 80, 81, 83, 85, 88, 91, 94, 97, 100,103,106,108,
   108,108,6,7};
*/
//------------------------------------------����ʱ�ĳ�ʼֵ����------------------------------------------------

//        Item_Index���鶨�壺ģʽ Y�� X��  V0  ���� ��Դ TR ���� ˥�� ���� ��ȡ Fout  T2  T1  X.POS  -- Item V2   V1   VT
unsigned short Item_Index[23]={0,   6,  7,  80,   0,   4,  8,  0,   0,   1,   1,   9,  233, 68, 4096,  0, 0,  40, 199, 140, 0, 0, 1};
//        Hide_Index���鶨�壺ģʽ REF X��  V0  ���� ��Դ VS ���� ˥�� ���� ��ȡ Fout  T2  T1   Tp    --  --  V2   V1   VT
unsigned char  Hide_Index[23]={1,   0,  1,   1,   1,  1,  1,  1,   1,   1,   1,   1,    0,  0,   1,    1,  1,  0,  0,   0, 0, 0, 1};

//unsigned short Item_Index[20]={0,  6,  7,  0,  0,   4,  8,  0,   0,  1,   10,  9,  0, 0,  4096, 0, 0, 0, 0, 0};
//unsigned short vt=140, v0=69, v1=199, v2=40, t0=0, t1=68, t2=233;
unsigned short t0=0, Tp;
//unsigned char  Hide_v2=0;
//unsigned char  Item=4;
//------------------------------------------------------------------------------------------------------------
int  Frequency, Cycle, Duty, Vram, Vavg, Vpp, DCV;

unsigned char Update[23]   ={1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1};//��Item��ˢ�±�־,��ֵ=1ʱ��ʾ��Ҫˢ��
unsigned const char V_Unit[4][3]={"uV","mV","V ","kV"};
unsigned const char T_Unit[4][3]={"nS","uS","mS","S "};
unsigned const char F_Unit[4][4]={"Hz","Hz","KC","MC"};
unsigned const char Battery_Status[5][4]={"~`'","~`}","~|}","{|}","USB"};
unsigned const short Battery_Color [5]  ={RED,  YEL,  GRN,  GRN,  GRN  };
unsigned const char MODE_Unit[6][6]={"AUTO","NORM","SING","NONE","SCAN","!FIT!"};
unsigned const char TEST_Unit[7][8]={" FREQN "," CYCLE ","  DUTY "," V p-p "," V rms "," V avg ","  DC.V "};
unsigned char  FileNum[4]="000";

unsigned short Delay_Counter=0, X_Counter=0, Edge=0, First_Edge, Last_Edge, Wait_CNT;
unsigned char  Counter_20mS=0, Key_Repeat_Counter=0, Key_Wait_Counter=0, Cursor_Counter=0, Type, Key_Repeat_X10;
unsigned char  Key_Buffer=0, Toggle=0, Sync=0, Erase_Note=0, Frame=0, Stop=0;
unsigned int   vb_Sum=360*32, Battery=400;


I32STR_RES Num;

/*******************************************************************************
 delayms: ���루mS����ʱ���� ����: ��ʱ�ȴ��ĺ�����ֵ����72MHz��Ƶ����£�
*******************************************************************************/
void Delayms(u16 mS)
{
	Delay_Counter=mS;
	while(Delay_Counter)
	{
	};
}

/*******************************************************************************
 Sync_Trig: ��ɨ�軺������Ѱ�ҵ�һ������ͬ�����������ĵ�
*******************************************************************************/
void Sync_Trig(void)
{
  int Vs;
  for(t0=150; t0<(0x1000-X_SIZE);++t0){   //�б𴥷�ͬ��λ��
    if(t0>=(0x1000-DMA_CNDTR1)) break;    //����һ�����ڣ��ȴ�A/Dת������
    Vs=120+(Km[Item_Index[Y_SENSITIVITY]]*(2048-Scan_Buffer[t0]))/4096;   //ת����ǰ���ε��ֵ
    if(Item_Index[TRIG_SLOPE]==0) {       //��������Ϊ�����ش���
      if(Vs<(Item_Index[VT]-Item_Index[TRIG_SENSITIVITY])&&(Sync==0)) Sync=1; //���ڴ�����ֵ����
      if(Vs>(Item_Index[VT]+Item_Index[TRIG_SENSITIVITY])&&(Sync==1)) Sync=2; //���ڴ�����ֵ����
    } else {                              //��������Ϊ�½��ش���
      if(Vs>(Item_Index[VT]+Item_Index[TRIG_SENSITIVITY])&&(Sync==0)) Sync=1; //���ڴ�����ֵ����
      if(Vs<(Item_Index[VT]-Item_Index[TRIG_SENSITIVITY])&&(Sync==1)) Sync=2; //���ڴ�����ֵ����
    }
    if(Sync==2) break;
  }
  X_Counter=0;
  if(t0>=(0x1000-X_SIZE)) Sync=3;//����ͬ�����ɹ���־

}

/*******************************************************************************
 Signal_Process: ���㴦�����ݻ�����
*******************************************************************************/
void Signal_Process(void)
{
  int i, p, q; int Vs, Vr;
  if(Sync==3) t0=150; //��ͬ�����ɹ����趨Ĭ����ʼλ��
  p=(Frame*(1024*X_SIZE)/Ks[Item_Index[X_SENSITIVITY]])+t0+Item_Index[X_POSITION]-(4096+150);
  for(i=X_Counter; i<(X_SIZE); ++i){
    Sync=5;           //����ת����ɺ�תȥ��ʾɨ�貨��
    q=p+(i*1024)/Ks[Item_Index[X_SENSITIVITY]];
    if(q<0) {
      q=0;
      Item_Index[X_POSITION]++;
    }
    if(q>=(0x1000-DMA_CNDTR1)) break;  //����һ�����ڣ��ȴ�A/Dת������
    X_Counter=i+1;
    Vr=Km[Item_Index[Y_SENSITIVITY]]*(Scan_Buffer[q+1]-Scan_Buffer[q])/4096;
    Vs=(Km[Item_Index[Y_SENSITIVITY]]*(2048-Scan_Buffer[q]))/4096+120               //��ǰ���ε����ֵ
      -(((i*1024)%Ks[Item_Index[X_SENSITIVITY]])*Vr)/Ks[Item_Index[X_SENSITIVITY]]; //��ǰ���ε�Ĳ�ֵ
    if(Vs>MAX_Y)  Vs=MAX_Y;
    else if(Vs<MIN_Y) Vs=MIN_Y;
    Signal_Buffer[i]=Vs;
    Sync=4;        //ȫ��ת����ɺ�תȥ��ʾɨ�貨��
  }
  if(DMA_CNDTR1==0) {
    Measure_Wave();//��������ɣ����㲨�εĸ�������ֵ
     if(Item_Index[RUNNING_STATUS]==RUN) ADC_Start(); //����"RUN"ģʽ�£����²���
  }
}

/*******************************************************************************
 Erase_Draw: �Ȳ�������ʾɨ�貨��
*******************************************************************************/
void Erase_Draw(void)
{
  unsigned short i;
  unsigned char  y1, y2, y3, y4,y5, y6;

  y1=View_Buffer[0];
  y3=Signal_Buffer[0];
  y5=Ref_Buffer[0];

  for(i=0; i<X_Counter; ++i){
    y2=View_Buffer[i];
    y4=Signal_Buffer[i];
    View_Buffer[i]=y4;
    y6=Ref_Buffer[i];
    Erase_SEG(i,y1,y2,CH1_COLOR);
    Erase_SEG(i,y5,y6,CH3_COLOR);
    Draw_CH1_SEG(i,y3,y4);
    if(Hide_Index[REF]==0) Draw_CH3_SEG(i,y5,y6);
    y1 = y2;
    y3 = y4;
    y5 = y6;
  }

  if(X_Counter>=X_SIZE-1) {//һ֡��ʾ���
    Stop=1;
    X_Counter=0; //����ָ�븴��
    Battery_Detect();//����ص�ѹ
    if((((Frame+2)*X_SIZE)+t0+Item_Index[X_POSITION]-4096)<0x1000) Frame++;//ָ����һ֡
    else {
      Frame=0;
      ADC_Start();
      Item_Index[X_POSITION]=4096;
    }
    Delay_Counter=100;        //ά�ֲ�����ʾ100mS
    if(Item_Index[0]!=4) Sync=0;//��"SCAN"ģʽ�£����¿�ʼ������ʾ����
    else Erase_Wave();//��"SCAN"ģʽ�£������Ѿ���ʾ�Ĳ��Σ�׼����һ֡����ʾ
  } else {
    Sync=2;
    Stop=0;//ɨ��������ʾδ��ɣ��������Ӧ֡������ת���������ʾ
  }
}
/*******************************************************************************
 Scan_Wave: ɨ��ͬ���������趨ģʽ��ʾ����
*******************************************************************************/

void Scan_Wave(void)//6 kinds of SYNC MODE:  AUTO��NORM��SIGN��NONE��SCAN��FIT
{

  //---------------------ɨ��ͬ����ʽΪ AUTO or FIT --------------------------
  if((Item_Index[SYNC_MODE]==0)||(Item_Index[SYNC_MODE]>4)) {
    Frame=0;
    if(Item_Index[RUNNING_STATUS]==RUN){
      Stop=0;
      if((Sync==0)||(Sync==1)) Sync_Trig();            //ͬ������
      if((Sync==2)||(Sync==3)) Signal_Process();       //���㴦�����ݻ�����
      if((Sync>3)&&(Delay_Counter==0)) Erase_Draw();   //ˢ����ʾɨ�貨��
    } else {                //��'HOLD'״̬��
      Sync=2;
      X_Counter=0;
    }
  }

//--------------------ɨ��ͬ����ʽΪ NORM-----------------------------
  if(Item_Index[SYNC_MODE]==1){
    Frame=0;
    if(Item_Index[RUNNING_STATUS]==RUN){
      Stop=0;
      if((Sync==0)||(Sync==1)) Sync_Trig();            //ͬ������
      if((Sync==2)||(Sync==3)) Signal_Process();       //���㴦�����ݻ�����
      if((Sync>3)&&(Delay_Counter==0)) Erase_Draw();   //ˢ����ʾɨ�貨��
    } else {                //��'HOLD'״̬��
      Sync=2;
      X_Counter=0;
    }
    if(Sync==3) {
      Erase_Wave();                        //�����ɵ�ɨ�貨��
      if(DMA_CNDTR1==0) {//��������ɣ����¿�ʼ����
        ADC_Start();
        Item_Index[RUNNING_STATUS]=RUN;
        Sync=0;
      }
    }
  }
//--------------------ɨ��ͬ����ʽΪ SING--------------------------
  if(Item_Index[SYNC_MODE]==2){
    Frame=0;
    if(Item_Index[RUNNING_STATUS]==RUN){  //��'RUN'״̬��
      Stop=0;
      if((Sync==0)||(Sync==1)) Sync_Trig();            //ͬ������
    } else {                //��'HOLD'״̬��
      Sync=0;
      X_Counter=0;
    }
    if(Sync==3) {
      Erase_Wave();                        //�����ɵ�ɨ�貨��
      if(DMA_CNDTR1==0) {//��������ɣ����¿�ʼ����
        ADC_Start();
        Item_Index[RUNNING_STATUS]=RUN;
        Sync=0;
      }
    }
    if(Sync==2) Signal_Process();       //���㴦�����ݻ�����
    if((Sync>3)&&(Delay_Counter==0)){
      Erase_Draw();                     //ˢ����ʾɨ�貨��
      Item_Index[RUNNING_STATUS]=HOLD;
      Update[RUNNING_STATUS]=1;
    }
  }
//-----------------------ɨ��ͬ����ʽΪ NONE------------------------
  if(Item_Index[SYNC_MODE]==3){
    Frame=0; X_Counter=0;
	if(Item_Index[RUNNING_STATUS]==RUN){  //��'RUN'״̬��
      Sync=3; Stop=0;
	} else {                //��'HOLD'״̬��
      Sync=2;
	}
    if((Sync==2)||(Sync==3)) Signal_Process();                    //���㴦�����ݻ�����

    if((Sync>3)&&(Delay_Counter==0)) Erase_Draw(); //ˢ����ʾɨ�貨��
  }
//-----------------------ɨ��ͬ����ʽΪ SCAN------------------------
  if(Item_Index[SYNC_MODE]==4){
    X_Counter=0;
    if(Item_Index[RUNNING_STATUS]==RUN){  //��'RUN'״̬��
      Stop=0;
      if(Sync==0) Erase_Wave();
      Sync=3;
      if((Sync==2)||(Sync==3)) Signal_Process();       //���㴦�����ݻ�����
      if((Sync>3)&&(Delay_Counter==0)) Erase_Draw();   //ˢ����ʾɨ�貨��
    } else {                //��'HOLD'״̬��
      Sync=2;
      X_Counter=0;
    }
  }
}
/*******************************************************************************
 Measure_Wave: ���㲨�ε�Ƶ�ʡ����ڡ����ֵ��ƽ��ֵ����Чֵ��ռ�ձ�
*******************************************************************************/
void Measure_Wave(void)
{
  unsigned short i=0, Vmax=0xFFFF, Vmin=0, Trig=0;
  unsigned int Threshold0, Threshold1, Threshold2, Threshold3;
  int  Vk=0, Vn=0, Vm, Vp, Vq, Tmp1, Tmp2;
  Edge=0,
  First_Edge=0;
  Last_Edge=0;
  Threshold0=2048-((Item_Index[V0]-120)*4096)/Km[Item_Index[Y_SENSITIVITY]];
  Threshold1=2048-((Item_Index[VT]-Item_Index[TRIG_SENSITIVITY]-120)*4096)
    /Km[Item_Index[Y_SENSITIVITY]];
  Threshold2=2048-((Item_Index[VT]+Item_Index[TRIG_SENSITIVITY]-120)*4096)
    /Km[Item_Index[Y_SENSITIVITY]];
  Threshold3=2048-((Item_Index[VT]-120)*4096)/Km[Item_Index[Y_SENSITIVITY]];
  for(i=0;i<0x1000;++i){
    Vk += Scan_Buffer[i];
    if((i>t0)&&(i<t0+300)){
      if(Scan_Buffer[i]<Vmax) Vmax=Scan_Buffer[i];
      if(Scan_Buffer[i]>Vmin) Vmin=Scan_Buffer[i];
    }
    if((Scan_Buffer[i]>Threshold1)&&(Trig==0))  Trig=1;//���ڴ�����ֵ����
    if((Scan_Buffer[i]<Threshold2)&&(Trig==1)) {
      Trig=0;
      if(First_Edge==0) {
        First_Edge=i;
        Last_Edge=i;
        Edge=0;
      } else {
        Last_Edge=i;
        Edge++;
      }
    }
  }
  Vk >>= 12;

  if(Edge!=0){

    Vm = 0;
    Vq = 0;
    for(i=First_Edge; i<Last_Edge; ++i){
      if(Scan_Buffer[i]<Threshold3) Vm++;
      Vp=(4096-Scan_Buffer[i])-Threshold0;
      Vn +=(Vp*Vp)/8;                       //Ϊ�˷�ֹ������Ԥ��8
      if(Scan_Buffer[i]<Threshold0) Vq+=(Threshold0-Scan_Buffer[i]);
      else                          Vq+=(Scan_Buffer[i]-Threshold0);
    }
    if(Type==PRN) {
      if(Item_Index[X_SENSITIVITY]<5) Frequency =(Edge*(1000000000/1167)/(Last_Edge-First_Edge))*1000;
      else Frequency = (Edge*(1000000000/T_Scale[Item_Index[X_SENSITIVITY]])/(Last_Edge-First_Edge))*1000;
      Cycle = ((Last_Edge-First_Edge)*T_Scale[Item_Index[X_SENSITIVITY]])/Edge;
      Duty = 100000*Vm/(Last_Edge-First_Edge);
      Vram=(((Km[Item_Index[Y_SENSITIVITY]])*__Int_sqrt((Vn/(Last_Edge-First_Edge))*8))>>12)
        *V_Scale[Item_Index[Y_SENSITIVITY]];
      Vavg=(((Km[Item_Index[Y_SENSITIVITY]])*(Vq/(Last_Edge-First_Edge)))>>12)
        *V_Scale[Item_Index[Y_SENSITIVITY]];
    }
  } else {
      Frequency = 0;
      Cycle     = 0;
      Duty      = 0;
      Vram      = 0;
      Vavg      = 0;
  }
  if(Vmin<Vmax) Vmin=Vmax;
  Tmp1 =((Km[Item_Index[Y_SENSITIVITY]]*(Vmin-Vmax))>>12);
  Vpp = Tmp1 * V_Scale[Item_Index[Y_SENSITIVITY]];

  Tmp2 = 120+(Km[Item_Index[Y_SENSITIVITY]]*(2048-Vk))/4096;
  DCV  =(Tmp2-Item_Index[V0]) * V_Scale[Item_Index[Y_SENSITIVITY]];
  if((Item_Index[SYNC_MODE]==5)&&(Wait_CNT==0)){
    if((Edge<20)&&(Item_Index[X_SENSITIVITY]<15)) {
      Item_Index[X_SENSITIVITY]++;
      Update[X_SENSITIVITY]=1;
    }
    if((Edge>60)&&(Item_Index[X_SENSITIVITY]>0 )) {
      Item_Index[X_SENSITIVITY]--;
      Update[X_SENSITIVITY]=1;

    }
    if((Tmp1 <50)&&(Item_Index[INPUT_ATTENUATOR]==0)&&(Item_Index[Y_SENSITIVITY]>0)){
      Item_Index[Y_SENSITIVITY]--;
      Update[Y_SENSITIVITY]=1;
    }
    if((Tmp1 <50)&&(Item_Index[INPUT_ATTENUATOR]==1)&&(Item_Index[Y_SENSITIVITY]>11)){
      Item_Index[Y_SENSITIVITY]--;
      Update[Y_SENSITIVITY]=1;
    }
    if((Tmp1 >150)&&(Item_Index[INPUT_ATTENUATOR]==0)&&(Item_Index[Y_SENSITIVITY]<9 )){
      Item_Index[Y_SENSITIVITY]++;
      Update[Y_SENSITIVITY]=1;
    }
    if((Tmp1 >150)&&(Item_Index[INPUT_ATTENUATOR]==1)&&(Item_Index[Y_SENSITIVITY]<18)){
      Item_Index[Y_SENSITIVITY]++;
      Update[Y_SENSITIVITY]=1;
    }
      Draw_Dot_Vn(Item_Index[VT]+Item_Index[VS], ERASE, LN1_COLOR);
      Draw_Dot_Vn(Item_Index[VT]-Item_Index[VS], ERASE, LN1_COLOR);
      Item_Index[VT]=Tmp2;
      Update[VERNIERS]=1;
  }
  if(Wait_CNT>5) Wait_CNT=0;
  else Wait_CNT++;
}
/*******************************************************************************
 Display_Item: ��ʾ����Ŀ������
*******************************************************************************/
void Display_Item(void)
{
  Display_Str(3,224,GRN,PRN,MODE_Unit[Item_Index[SYNC_MODE]]);//��ʾɨ��ͬ����ʽ
  Display_Str(40,224,YEL,PRN,Item_V[Item_Index[Y_SENSITIVITY]]);//��ʾ(�����ѹ)����
  Display_Str(109,224,YEL,PRN,Item_T[Item_Index[X_SENSITIVITY]]); //��ʾ(ɨ������)����
  Display_Str(186,224,CH1_COLOR,PRN,"Y.POS");//��ʾ(��ֱλ��)����ѡ��
  Display_Str(231,224,WHITE,PRN,TEST_Unit[Item_Index[MEASUR_KIND]]);//��ʾ(��������
  if(Test_USB_ON())
    Display_Str(292,224,Battery_Color[4],PRN,Battery_Status[4]);//�����USB��Դʱ����ʾ'USB'����״̬
  else
    Display_Str(292,224,Battery_Color[Item_Index[5]],PRN,
                Battery_Status[Item_Index[POWER_INFOMATION]]);//��ʾ(���״̬)
  Display_Str(303, 205, YEL,PRN, "TR");//��ʾ(����������ѡ��
  if(Item_Index[TRIG_SLOPE]==RISING) Display_Str(303, 185, YEL,PRN, "^S");//��ʾ�����ش�������
  else                               Display_Str(303, 185, YEL,PRN, "_S");//��ʾ�½��ش�������
  if(Item_Index[INPUT_ATTENUATOR]==0) Display_Str(303, 165, YEL,PRN, "*1");//��ʾ̽ͷ˥����1
  else                                Display_Str(303, 165, YEL,PRN, "10");//��ʾ̽ͷ˥����0.1
  Display_Str(303, 145, YEL,PRN, "SI");//��ʾ(ͼ�񱣴�)
  Display_Str(303, 125, YEL,PRN, "FS");//��ʾ(�ļ�����)
  Display_Str(303, 105, YEL, PRN,"FL");//��ʾ(�ļ�װ��)
  Display_Str(303, 85, YEL,PRN, "Fo");//��ʾ(��׼Ƶ�����)
  Display_Str(303, 65,  YEL,PRN, "T2");//��ʾ(�����α�T2)
  Display_Str(303, 45,  YEL,PRN, "T1");//��ʾ(�����α�T1)
  Display_Str(303, 25,  YEL,PRN, "T0");//��ʾ(ˮƽλ��)
  Display_Str(284,2,GRN,PRN,"!RUN!");//��ʾ(ɨ��״̬)
  Int32String_sign(&Num, (Item_Index[T2]-Item_Index[T1])*T_Scale[Item_Index[X_SENSITIVITY]], 3);//
  Display_Str(199,2,YEL,PRN,"[T=");//��ʾ(ʱ�����)
  Display_Str(224,2,YEL,PRN,(unsigned const char *)Num.str);
  Display_Str(224+Num.len*8,2,YEL,PRN," ");
  Display_Str(224+40,2,YEL,PRN,T_Unit[Num.decPos]);
  Int32String_sign(&Num, (Item_Index[V1]-Item_Index[V2])*V_Scale[Item_Index[Y_SENSITIVITY]], 3);//
  Display_Str(88,2, WHITE,PRN,"V1");
  Display_Str(105,2,WHITE,PRN,"-");
  Display_Str(114,2,WHITE,PRN,"V2");
  Display_Str(131,2,WHITE,PRN,"=");
  Display_Str(139,2,WHITE,PRN,(unsigned const char *)Num.str);//��ʾ(��ѹ�����α�1~2)
  Display_Str(139+40,2,WHITE,PRN,V_Unit[Num.decPos]);
  Int32String_sign(&Num, (Item_Index[VT]-Item_Index[V0])*V_Scale[Item_Index[Y_SENSITIVITY]], 3);//
  Display_Str(3,2,YEL,PRN,"Vt"); //��ʾ(������ѹ)
  Display_Str(20,2,YEL,PRN,"=");
  Display_Str(28,2,YEL,PRN,(unsigned const char *)Num.str);
  Display_Str(28+40,2,YEL,PRN,V_Unit[Num.decPos]);
  if(Hide_Index[T1]) Draw_Ti(Item_Index[T1],ERASE,LN2_COLOR);
  else        Draw_Ti(Item_Index[T1],ADD,LN2_COLOR);
  if(Hide_Index[T2]) Draw_Ti(Item_Index[T2],ERASE,LN2_COLOR);
  else        Draw_Ti(Item_Index[T2],ADD,LN2_COLOR);
  if(Hide_Index[V1]) Draw_Vi(Item_Index[V1],ERASE,LN2_COLOR);
  else        Draw_Vi(Item_Index[V1],ADD,LN2_COLOR);
  if(Hide_Index[V2]) Draw_Vi(Item_Index[V2],ERASE,LN2_COLOR);
  else        Draw_Vi(Item_Index[V2],ADD,LN2_COLOR);
  if(Hide_Index[VT]){
    Draw_Vn(Item_Index[VT]+Item_Index[VS],ERASE,LN1_COLOR);
    Draw_Vn(Item_Index[VT]-Item_Index[VS],ERASE,LN1_COLOR);
  }else{
    Draw_Vn(Item_Index[VT]+Item_Index[VS],ADD,LN1_COLOR);
    Draw_Vn(Item_Index[VT]-Item_Index[VS],ADD,LN1_COLOR);
  }
  Draw_Dot_Vn(Item_Index[VT]+Item_Index[VS],ADD,LN1_COLOR);
  Draw_Dot_Vn(Item_Index[VT]-Item_Index[VS],ADD,LN1_COLOR);
  Draw_Dot_Vn(Item_Index[V1],ADD,LN2_COLOR);
  Draw_Dot_Vn(Item_Index[V2],ADD,LN2_COLOR);
  Draw_Dot_Ti(Item_Index[T1],ADD,LN2_COLOR);
  Draw_Dot_Ti(Item_Index[T2],ADD,LN2_COLOR);
  if((Tp!=MIN_X)&&(Tp!=MAX_X))  Draw_Dot_Ti(Tp,ADD,CH2_COLOR);            //Draw_Dot_Tp
}
/*******************************************************************************
 Update_Item: ����Update[x]��Erase[x]�ı�־��ˢ����ʾ����Ŀ������
*******************************************************************************/
void Update_Item(void)
{
  short Tmp;
  if( Update[SYNC_MODE]!=0){                 //ˢ��(ɨ��ͬ����ʽѡ��)ģʽ
    Update[SYNC_MODE]=Type;
    Display_Str(3,224,GRN,Type,MODE_Unit[Item_Index[SYNC_MODE]]);
  }
  if( Update[Y_SENSITIVITY]!=0){   //ˢ��(�����ѹ)���̵�����ʾ������
    Update[Y_SENSITIVITY]=Type;
    Display_Str(40,224,YEL,Type,Item_V[Item_Index[Y_SENSITIVITY]]);
    Update[TRIG_LEVEL]=1;
    Update[Y_VERNIER_1]=1;
    Set_Range(Item_Index[Y_SENSITIVITY]);
    Set_Y_Pos(Item_Index[Y_SENSITIVITY], Item_Index[V0]);
  }
  if( Update[X_SENSITIVITY]!=0){   //ˢ��(ɨ������)���̵�����ʾ
    Update[X_SENSITIVITY]=Type;
    Display_Str(109,224,YEL,Type,Item_T[Item_Index[X_SENSITIVITY]]);
    Set_Base(Item_Index[X_SENSITIVITY]);
    Update[DELTA_T]=1;
  }
  if( Update[Y_POSITION]!=0){   //ˢ��(��ֱλ��)����ѡ�� Y.Position
    Update[Y_POSITION]=Type;
    Display_Str(186,224,CH1_COLOR,Type,"Y.POS");
    Draw_Dot_Vn(Item_Index[V0],ADD,CH1_COLOR);
    Set_Y_Pos(Item_Index[Y_SENSITIVITY], Item_Index[V0]);
    Update[VERNIERS]=1;
  }
  if( Update[MEASUR_KIND]!=0){   //ˢ��(��������)��ʾ
    Update[MEASUR_KIND]=Type;
    if(Type==0) Display_Str(231,224,WHITE,PRN,TEST_Unit[Item_Index[MEASUR_KIND]]);
    else {
      switch (Item_Index[4]){
        case 0://��ʾƵ��
          if(Frequency==0){
            if(Item_Index[CURRENT_ITEM]==MEASUR_KIND){
              Display_Str(231,224,WHITE,INV,TEST_Unit[Item_Index[MEASUR_KIND]]);
            } else {
              Display_Str(231,224,WHITE,PRN,TEST_Unit[Item_Index[MEASUR_KIND]]);
            }
          } else {
            Int32String(&Num, Frequency, 4);
            if(Item_Index[CURRENT_ITEM]==MEASUR_KIND){
              Display_Str(231,224,WHITE,Type,(unsigned const char *)Num.str);
              Display_Str(231+40,224,WHITE,Type,F_Unit[Num.decPos]);
            } else{
              Display_Str(231,224,WHITE,PRN,(unsigned const char *)Num.str);
              Display_Str(231+40,224,WHITE,PRN,F_Unit[Num.decPos]);
            }
          }
          break;
        case 1://��ʾ����
          if(Cycle==0)
            if(Item_Index[CURRENT_ITEM]==MEASUR_KIND){
              Display_Str(231,224,WHITE,INV,TEST_Unit[Item_Index[MEASUR_KIND]]);
            } else {
              Display_Str(231,224,WHITE,PRN,TEST_Unit[Item_Index[MEASUR_KIND]]);
            }
          else {
            Int32String(&Num, Cycle, 4);
            if(Item_Index[CURRENT_ITEM]==MEASUR_KIND){
              Display_Str(231,224,WHITE,Type,(unsigned const char *)Num.str);
              Display_Str(231+40,224,WHITE,Type,T_Unit[Num.decPos]);
            } else{
              Display_Str(231,224,WHITE,PRN,(unsigned const char *)Num.str);
              Display_Str(231+40,224,WHITE,PRN,T_Unit[Num.decPos]);
            }
          }
          break;
        case 2://��ʾռ�ձ�
          if(Duty==0)
            if(Item_Index[CURRENT_ITEM]==MEASUR_KIND){
              Display_Str(231,224,WHITE,INV,TEST_Unit[Item_Index[MEASUR_KIND]]);
            } else {
              Display_Str(231,224,WHITE,PRN,TEST_Unit[Item_Index[MEASUR_KIND]]);
            }
          else {
            Int32String(&Num, Duty, 4);
            if(Item_Index[CURRENT_ITEM]==MEASUR_KIND){
              Display_Str(231,224,WHITE,Type,(unsigned const char *)Num.str);
              Display_Str(231+40,224,WHITE,Type,"% ");
            } else{
              Display_Str(231,224,WHITE,PRN,(unsigned const char *)Num.str);
              Display_Str(231+40,224,WHITE,PRN,"% ");
            }
          }
          break;
        case 3://��ʾ�������ֵ
          Int32String(&Num, Vpp, 4);
          if(Item_Index[CURRENT_ITEM]==MEASUR_KIND){
            Display_Str(231,224,WHITE,Type,(unsigned const char *)Num.str);
            Display_Str(231+40,224,WHITE,Type,V_Unit[Num.decPos]);
          } else{
            Display_Str(231,224,WHITE,PRN,(unsigned const char *)Num.str);
            Display_Str(231+40,224,WHITE,PRN,V_Unit[Num.decPos]);
          }
          break;
        case 4://��ʾ������Чֵ
          if(Vram==0)
            if(Item_Index[CURRENT_ITEM]==MEASUR_KIND){
              Display_Str(231,224,WHITE,INV,TEST_Unit[Item_Index[MEASUR_KIND]]);
            } else {
              Display_Str(231,224,WHITE,PRN,TEST_Unit[Item_Index[MEASUR_KIND]]);
            }
          else {
            Int32String(&Num, Vram, 4);
            if(Item_Index[CURRENT_ITEM]==MEASUR_KIND){
              Display_Str(231,224,WHITE,Type,(unsigned const char *)Num.str);
              Display_Str(231+40,224,WHITE,Type,V_Unit[Num.decPos]);
            } else{
              Display_Str(231,224,WHITE,PRN,(unsigned const char *)Num.str);
              Display_Str(231+40,224,WHITE,PRN,V_Unit[Num.decPos]);
            }
          }
          break;
        case 5://��ʾ����ƽ��ֵ
          if(Vavg==0)
            if(Item_Index[CURRENT_ITEM]==MEASUR_KIND){
              Display_Str(231,224,WHITE,INV,TEST_Unit[Item_Index[MEASUR_KIND]]);
            } else {
              Display_Str(231,224,WHITE,PRN,TEST_Unit[Item_Index[MEASUR_KIND]]);
            }
          else {
            Int32String(&Num, Vavg, 4);
            if(Item_Index[CURRENT_ITEM]==MEASUR_KIND){
              Display_Str(231,224,WHITE,Type,(unsigned const char *)Num.str);
              Display_Str(231+40,224,WHITE,Type,V_Unit[Num.decPos]);
            } else{
              Display_Str(231,224,WHITE,PRN,(unsigned const char *)Num.str);
              Display_Str(231+40,224,WHITE,PRN,V_Unit[Num.decPos]);
            }
          }
          break;
        case 6://��ʾֱ��ƽ��ֵ
          Int32String_sign(&Num, DCV, 3);
          if(Item_Index[CURRENT_ITEM]==MEASUR_KIND){
            Display_Str(231,224,WHITE,Type,(unsigned const char *)Num.str);
            Display_Str(231+40,224,WHITE,Type,V_Unit[Num.decPos]);
          } else{
            Display_Str(231,224,WHITE,PRN,(unsigned const char *)Num.str);
            Display_Str(231+40,224,WHITE,PRN,V_Unit[Num.decPos]);
          }
          break;
      }
    }
  }

  if( Update[POWER_INFOMATION]!=0){   //ˢ��(���״̬)��ʾ
    Update[POWER_INFOMATION]=Type;
    if(Test_USB_ON())
      Display_Str(292,224,Battery_Color[4],PRN,Battery_Status[4]);//�����USB��Դʱ������״̬����ʾ'USB'
    else
      Display_Str(292,224,Battery_Color[Item_Index[5]],PRN,
                  Battery_Status[Item_Index[POWER_INFOMATION]]);
  }

  if( Update[TRIG_SENSITIVITY]!=0){   //ˢ��(����������ѡ��)��ʾ
    Update[TRIG_SENSITIVITY]=Type;
    Display_Str(303, 205, YEL,Type, "TR");
    if(Item_Index[CURRENT_ITEM]==TRIG_SENSITIVITY){
      Display_Str(88,2,WHITE,Type,"Trig!Sensitive");
    }
  }

  if( Update[TRIG_SLOPE]!=0){   //ˢ��(��������)��ʾ
    Update[TRIG_SLOPE]=Type;
    if(Item_Index[TRIG_SLOPE]==RISING) Display_Str(303, 185, YEL,Type, "^S");//�����ش���
    else                               Display_Str(303, 185, YEL,Type, "_S");//�½��ش���
    if(Item_Index[CURRENT_ITEM]==TRIG_SLOPE){
      if(Item_Index[TRIG_SLOPE]==0) Display_Str(88,2,WHITE,Type,"!Trig.Slope=!^!");
      else                          Display_Str(88,2,WHITE,Type,"!Trig.Slope=!_!");
    }
  }

  if( Update[INPUT_ATTENUATOR]!=0){   //ˢ��(����̽ͷ˥��)��ʾ
    Update[INPUT_ATTENUATOR]=Type;
    if(Item_Index[INPUT_ATTENUATOR]==0) Display_Str(303, 165, YEL,Type, "*1");//�����1
    else                                Display_Str(303, 165, YEL,Type, "10");//�����0.1
    if(Item_Index[CURRENT_ITEM]==INPUT_ATTENUATOR){
      if(Item_Index[INPUT_ATTENUATOR]==0) Display_Str(88,2,WHITE,Type,"!INP.Scale!=*1!");
      else                                Display_Str(88,2,WHITE,Type,"!INP.Scale=*10");
    }
    Update[Y_SENSITIVITY]=1;
  }
  if( Update[SAVE_WAVE_IMAGE]!=0){   //ˢ��(ͼ�񱣴�)��ʾ
    Update[SAVE_WAVE_IMAGE]=Type;
    Display_Str(303, 145, YEL,Type, "SI");
    if(Item_Index[CURRENT_ITEM]==SAVE_WAVE_IMAGE){
      __Char_to_Str(FileNum,Item_Index[SAVE_WAVE_IMAGE]);
      Display_Str(88,2,WHITE,Type,"!Save Image");
      Display_Str(172,2,WHITE,Type,FileNum);
    }
  }
  if( Update[SAVE_WAVE_CURVE]!=0){   //ˢ��(�ļ�����)��ʾ
    Update[SAVE_WAVE_CURVE]=Type;
    Display_Str(303, 125, YEL,Type, "FS");
    if(Item_Index[CURRENT_ITEM]==SAVE_WAVE_CURVE){
      __Char_to_Str(FileNum,Item_Index[SAVE_WAVE_CURVE]);
      Display_Str(88,2,WHITE,Type,"!Save File");
      Display_Str(164,2,WHITE,Type,FileNum);
      Display_Str(188,2,WHITE,Type," ");
    }
  }
  if( Update[LOAD_WAVE_CURVE]!=0){   //ˢ��(�ļ�װ��)��ʾ
    Update[LOAD_WAVE_CURVE]=Type;
    Display_Str(303, 105, YEL, Type,"FL");
    if(Item_Index[CURRENT_ITEM]==LOAD_WAVE_CURVE){
      __Char_to_Str(FileNum,Item_Index[LOAD_WAVE_CURVE]);
      Display_Str(88,2,WHITE,Type,"!Load File");
      Display_Str(164,2,WHITE,Type,FileNum);
      Display_Str(188,2,WHITE,Type," ");
    }
  }

  if( Update[OUTPUT_FREQUENCY]!=0){   //ˢ��(��׼Ƶ�����)��ʾ
    Update[OUTPUT_FREQUENCY]=Type;
    TIM4_PSC = Fout_PSC[Item_Index[OUTPUT_FREQUENCY]];
    TIM4_ARR = Fout_ARR[Item_Index[OUTPUT_FREQUENCY]];
    TIM4_CCR1 = (Fout_ARR[Item_Index[OUTPUT_FREQUENCY]]+1)/2;
    Display_Str(303, 85, YEL,Type, "Fo");
    if(Item_Index[CURRENT_ITEM]==11){
      Display_Str(88,2,WHITE,Type," Fout!=");
      Display_Str(140,2,WHITE,Type,Item_F[Item_Index[OUTPUT_FREQUENCY]]);
      Display_Str(188,2,WHITE,Type," ");
    }
  }

  if( Update[X_VERNIER_2]!=0){   //ˢ��(�����α�T2)��ʾ
    Update[X_VERNIER_2]=Type;
    Display_Str(303, 65,  YEL,Type, "T2");
    if(Item_Index[CURRENT_ITEM]==X_VERNIER_2){
      Display_Str(88,2,WHITE,Type,"!Time Cursor2 ");
      Draw_Dot_Ti(Item_Index[T1],ADD,LN2_COLOR);
    }
  }

  if( Update[X_VERNIER_1]!=0){   //ˢ��(�����α�T1)��ʾ
    Update[X_VERNIER_1]=Type;
    Display_Str(303, 45,  YEL,Type, "T1");
    if(Item_Index[CURRENT_ITEM]==X_VERNIER_1){
      Display_Str(88,2,WHITE,Type,"!Time Cursor1 ");
      Draw_Dot_Ti(Item_Index[T1],ADD,LN2_COLOR);
    }
  }

  if( Update[X_POSITION]!=0){   //ˢ��(ˮƽλ�Ƶ���)��ʾ
    Update[X_POSITION]=Type;
    Display_Str(303, 25,  YEL,Type, "T0");
    if(Item_Index[CURRENT_ITEM]==X_POSITION){
      Display_Str(88,2,WHITE,Type,"!X.POS Adjust ");
    }
  }

  if( Update[RUNNING_STATUS]!=0){   //ˢ��(ɨ��״̬)��ʾ
    Update[RUNNING_STATUS]=0;
      if(Item_Index[RUNNING_STATUS]==0) Display_Str(284,2,GRN,PRN,"!RUN!");
      else  Display_Str(284,2,RED,PRN,"HOLD");
  }

  if( Update[DELTA_T]!=0){   //ˢ��(ʱ�����)��ʾ
    Update[DELTA_T]=0;
    Int32String_sign(&Num, (Item_Index[T2]-Item_Index[T1])*T_Scale[Item_Index[X_SENSITIVITY]], 3);//
    Display_Str(199,2,YEL,PRN,"[T=");
    Display_Str(224,2,YEL,PRN,(unsigned const char *)Num.str);
    Display_Str(224+Num.len*8,2,YEL,PRN," ");
    Display_Str(224+40,2,YEL,PRN,T_Unit[Num.decPos]);
  }

  if( Update[Y_VERNIER_2]!=0){   //ˢ��(��ѹ�����α�2)��ʾ
    Update[Y_VERNIER_2]=Type;
    if(Item_Index[CURRENT_ITEM]!=Y_VERNIER_2) Tmp=PRN;
    else                  Tmp=Type;
    if((Item_Index[CURRENT_ITEM]<6)||(Item_Index[CURRENT_ITEM]>14)){
      Int32String_sign(&Num, (Item_Index[V1]-Item_Index[V2])*V_Scale[Item_Index[Y_SENSITIVITY]], 3);//
      Display_Str(88,2, WHITE,PRN,"V1");
      Display_Str(105,2,WHITE,PRN,"-");
      Display_Str(114,2,WHITE,Tmp,"V2");
      Display_Str(131,2,WHITE,PRN,"=");
      Display_Str(139,2,WHITE,PRN,(unsigned const char *)Num.str);
      Display_Str(139+40,2,WHITE,PRN,V_Unit[Num.decPos]);
    }
  }

  if( Update[Y_VERNIER_1]!=0){   //ˢ��(��ѹ�����α�1)��ʾ
    Update[Y_VERNIER_1]=Type;
    if(Item_Index[CURRENT_ITEM]!=Y_VERNIER_1) Tmp=PRN;
    else                  Tmp=Type;
    if((Item_Index[CURRENT_ITEM]<6)||(Item_Index[CURRENT_ITEM]>14)){
      Int32String_sign(&Num, (Item_Index[V1]-Item_Index[V2])*V_Scale[Item_Index[Y_SENSITIVITY]], 3);//
      Display_Str(88,2,WHITE,Tmp,"V1");
      Display_Str(105,2,WHITE,PRN,"-");
      Display_Str(139,2,WHITE,PRN,(unsigned const char *)Num.str);
      Display_Str(139+40,2,WHITE,PRN,V_Unit[Num.decPos]);
    }
  }

  if( Update[TRIG_LEVEL]!=0){   //ˢ��(������ѹ)��ʾ
    Update[TRIG_LEVEL]=Type;
    Update[VERNIERS]=1;
    if(Item_Index[CURRENT_ITEM]!=TRIG_LEVEL) Tmp=PRN;
    else                 Tmp=Type;
    Int32String_sign(&Num, (Item_Index[VT]-Item_Index[V0])*V_Scale[Item_Index[Y_SENSITIVITY]], 3);//
    Display_Str(3,2,YEL,Tmp,"Vt");
    Display_Str(20,2,YEL,PRN,"=");
    Display_Str(28,2,YEL,PRN,(unsigned const char *)Num.str);
    Display_Str(28+40,2,YEL,PRN,V_Unit[Num.decPos]);
  }

  if(Hide_Index[VS]){
    Hide_Index[VS]=0;
  }

  if(Update[VERNIERS]){
    Update[VERNIERS]=0;
    Tp = MIN_X+150+Item_Index[X_POSITION]-4096;
    if(Tp > MIN_X+X_SIZE) Tp = MAX_X;
    if(Tp < MIN_X)        Tp = MIN_X;
    if(Hide_Index[TP]) Draw_Ti(Tp,ERASE,CH2_COLOR);
    else {
      if((Tp!=MIN_X)&&(Tp!=MAX_X))  Draw_Ti(Tp,ADD,CH2_COLOR);
    }
    if(Hide_Index[T1]) Draw_Ti(Item_Index[T1],ERASE,LN2_COLOR);
    else        Draw_Ti(Item_Index[T1],ADD,LN2_COLOR);
    if(Hide_Index[T2]) Draw_Ti(Item_Index[T2],ERASE,LN2_COLOR);
    else        Draw_Ti(Item_Index[T2],ADD,LN2_COLOR);
    if(Hide_Index[V1]) Draw_Vi(Item_Index[V1],ERASE,LN2_COLOR);
    else        Draw_Vi(Item_Index[V1],ADD,LN2_COLOR);
    if(Hide_Index[V2]) Draw_Vi(Item_Index[V2],ERASE,LN2_COLOR);
    else        Draw_Vi(Item_Index[V2],ADD,LN2_COLOR);
    if(Hide_Index[VT]){
      Draw_Vn(Item_Index[VT]+Item_Index[VS],ERASE,LN1_COLOR);
      Draw_Vn(Item_Index[VT]-Item_Index[VS],ERASE,LN1_COLOR);
    }else{
      Draw_Vn(Item_Index[VT]+Item_Index[VS],ADD,LN1_COLOR);
      Draw_Vn(Item_Index[VT]-Item_Index[VS],ADD,LN1_COLOR);
    }
    Draw_Dot_Vn(Item_Index[VT]+Item_Index[VS],ADD,LN1_COLOR);
    Draw_Dot_Vn(Item_Index[VT]-Item_Index[VS],ADD,LN1_COLOR);
    Draw_Dot_Vn(Item_Index[V1],ADD,LN2_COLOR);
    Draw_Dot_Vn(Item_Index[V2],ADD,LN2_COLOR);
    Draw_Dot_Ti(Item_Index[T1],ADD,LN2_COLOR);
    Draw_Dot_Ti(Item_Index[T2],ADD,LN2_COLOR);
    if((Tp!=MIN_X)&&(Tp!=MAX_X))  Draw_Dot_Ti(Tp,ADD,CH2_COLOR);            //Draw_Dot_Tp
  }
}

void Test_Display(int a, int b)
{
	__Char_to_Str(FileNum,a);
	Display_Str(232,87,WHITE,0,FileNum);
	Int32String_sign(&Num, b, 4);
	Display_Str(262,87,WHITE,0,(unsigned const char *)Num.str);
}

/******************************** END OF FILE *********************************/
