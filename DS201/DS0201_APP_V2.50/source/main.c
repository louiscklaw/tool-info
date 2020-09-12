/******************** (C) COPYRIGHT 2009 e-Design Co.,Ltd. *********************
  File Name: main.c
     Author: bure
   Hardware: DS0201V1.1~1.6
    Version: Ver 2.01
********************************************************************************
 Ver 2.01 �汾�޸�˵��
 1.�޸���SCANģʽ���ڵ���ʱ��ʾ������BUG
 2.�����˱����ֳ����ò����Ĺ��ܣ�����������CONFIG.DAT�ļ���
 3.�����˴������ʶ��ʾ
 4.�޸���Vp-p���������BUG
 5.�޸��˵��������������ʾ�����BUG
 6.�޸���2G��SD����д�����BUG
********************************************************************************
 Ver 2.02 �汾�޸�˵��
 1.�޸ĵ������ΪFULLʱ����ѹΪ3.9V
********************************************************************************
 Ver 2.20 �汾�޸�˵��
 1. ��Ӧ�ó����ΪLIB��APP���󲿷֣�ÿһ���ִ����С��32KB������ʹ�������IAR��32K���ư棩�������
 2. �����������������SD���ϸĵ����ڵ�FLASH�ϣ���û��SD��ʱ��Ȼ���Ա��湤������
 3. ����LCD��Ļ��д�빦�ܣ������ʾЧ��
 4. �޸��˲ο����ε���ʾ��������ʽ����M���л�����ʾ/������
 5. �޸��˵������������ȣ��زʱ�ı�ʾ��ʽ
********************************************************************************
 Ver 2.21 �汾�޸�˵��
 1. �����ò������ȱ�ʾ�źű仯���ʵĹ���
********************************************************************************
 Ver 2.30 �汾�޸�˵��
 1. �޸ı������������ʽ��
 2. �޸��˿�����ʾ��Ϣ�е��﷨����
 3. UI������ʽ�Ķ���ͳһ�����Ҽ�ѡ��˵������¼��ı�˵�ѡ��
 4. ��ɨ��ͬ����ʽ�У������ˡ�FIT������Ӧ���ܵ������Ը������������ź�Ƶ�ʣ��Զ�ѡ����ʵ�ɨ��ʱʱ��
********************************************************************************
 Ver 2.31 �汾�޸�˵��
 1. �ڡ�FIT������Ӧ���ܵ��У������˿��Ը������������źŷ��ȣ�
    �Զ�ѡ����ʵ�Y�������ȵ��Ĺ���
********************************************************************************
 Ver 2.40 �汾�޸�˵��
 1. SIGNģʽ����ΪSING
 2. �޸���SINGLE Mode�£���׽����������BUG
 3. �޸���NORM Mode�£���������������BUG
 4. ���Ʋ�����Ӧ���л��˵����ʱ���ȷ�ɫ���ӳٰ�����ٿ�ʼ��˸
 5. ���ƶ�X.POSʱ��ÿ�ΰ����ƶ�1��Ϊ1��25�㣩
 6. ȡ���ò������ȱ�ʾ�źű仯���ʵĹ���
 7. ������ͣ/���С����ָ�����ʱ����ˢ����Ļ������żȻ�����Ĳ�������
********************************************************************************
 Ver 2.50 �汾�޸�˵��
 1. ��������Ļ��ǰ��ʾͼ���BMP�ļ����湦��
 2. �޸���RUN/HOLDģʽת��ʱ��ͬ������ƫ�Ƶ�BUG
 3. ��������ͬ��ģʽ״̬�£�����M�������ԡ���ʾ/�������ο����ι���
********************************************************************************
У�ԣ�yijian 2010.08.26
*******************************************************************************/

#include "Function.h"
#include "Lcd.h"
#include "Calculate.h"
#include "stm32f10x_lib.h"
#include "HW_V1_Config.h"
#include "File.h"
#include "string.h"
#include "ASM_Funtion.h"

int main(void)
{
  int i,j;
/*--------------Ӳ����·��ʼ��-----------*/
  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0xC000);
  Timer_Configuration();

/*----------��ʾ������ʾ��Ϣҳ��----------*/
  Display_Str(8, 71, GRN,PRN, "        System Initializing...       ");
  Display_Str(8, 23, YEL,PRN, "   DS0201 Oscilloscope APP Ver2.50   ");
  for(i=0; i<300; ++i)  Signal_Buffer[i]=120;
  for(j=0; j<0x1000; ++j) Scan_Buffer[j]=1940;
  for(i=0; i<304; ++i)  Ref_Buffer [i]=__Get_Ref_Wave(i);
  Read_Parameter();
  Delayms(2000);
  Battery_Detect();

/*--------ʾ��������ҳ����ʾ��ʼ��--------*/
  Clear_Screen(BACKGROUND);       //����Ļ
  Display_Grid();                 //��ʾ��������
  Display_Frame();

/*------------ʾ����������ѭ��------------*/
  while (1){
    Update_Item();
    Scan_Wave();
    if(Key_Buffer!=0) {
      Cursor_Counter=25;
      Type=0;
      Update[Item_Index[CURRENT_ITEM]]=1;
      Update[VERNIERS]=1;
      if(Key_Buffer==KEYCODE_PLAY){
        Update[15]=1;
        if(Item_Index[RUNNING_STATUS]==RUN)  Item_Index[RUNNING_STATUS]=HOLD;
        else {
          ADC_Start();
	  Item_Index[RUNNING_STATUS]=RUN;
	  Sync=0;
          Clear_Screen(BACKGROUND);       //����Ļ
          Display_Grid();                 //��ʾ��������
          Display_Frame();
          Display_Item();
        }
        Key_Buffer=0;
      }
      if(((~GPIOA_IDR) & 0x0010)&&(Key_Buffer==KEYCODE_MANU)){ // KEY_PLAY & KEY_MANU is push on
        Item_Index[RUNNING_STATUS]=RUN;                        // RUNNING_STATUS always RUN
        if(Write_Parameter()!=FLASH_COMPLETE) Display_Str(89,2,WHITE,PRN,"!Write Error !");
        else                                  Display_Str(89,2,WHITE,PRN,"!Write Finish!");
        Delayms(500);
        Update[Y_VERNIER_1]=Update[Y_VERNIER_2]=1;
        Key_Buffer=0;
      }
      if(((~GPIOA_IDR) & 0x0008)&&(Key_Buffer==KEYCODE_MANU)){ // KEY_B & KEY_MANU is push on
        if(Write_Parameter()!=FLASH_COMPLETE) Display_Str(89,2,WHITE,PRN,"!Write Error !");
        else                                  Display_Str(89,2,WHITE,PRN,"!Write Finish!");
        Delayms(500);
        Update[Y_VERNIER_1]=Update[Y_VERNIER_2]=1;
        Key_Buffer=0;
      }
      if(((~GPIOA_IDR) & 0x0008)&&(Key_Buffer==KEYCODE_LEFT)){ // KEY_B & KEY_LEFT is push on
        Item_Index[CURRENT_ITEM]=X_SENSITIVITY;
        Update[Item_Index[CURRENT_ITEM]]=1;
        if(Item_Index[X_SENSITIVITY]>0){
          Item_Index[X_SENSITIVITY]--;
          ADC_Start();
          Frame=0;
          X_Counter=0;
          Item_Index[X_POSITION]=4096;
          Draw_Dot_Ti(Tp,ERASE,CH2_COLOR);
          Sync=0;
        }
        Key_Buffer=0;
      }
      if(((~GPIOA_IDR) & 0x0008)&&(Key_Buffer==KEYCODE_RIGHT)){ // KEY_B & KEY_RIGHT is push on
        Item_Index[CURRENT_ITEM]=X_SENSITIVITY;
        Update[Item_Index[CURRENT_ITEM]]=1;
        if(Item_Index[X_SENSITIVITY]<21){
          Item_Index[X_SENSITIVITY]++;
          ADC_Start();
          Frame=0;
          X_Counter=0;
          Item_Index[X_POSITION]=4096;
          Draw_Dot_Ti(Tp,ERASE,CH2_COLOR);
          Sync=0;
        }
        Key_Buffer=0;
      }
      if(((~GPIOA_IDR) & 0x0008)&&(Key_Buffer==KEYCODE_UP)){ // KEY_B & KEY_UP is push on
        Item_Index[CURRENT_ITEM]=Y_SENSITIVITY;
        Update[Item_Index[CURRENT_ITEM]]=1;
        ADC_Start();
        Frame=0;
        X_Counter=0;
        Item_Index[X_POSITION]=4096;
        Draw_Dot_Ti(Tp,ERASE,CH2_COLOR);
        Sync=0;
        if((Item_Index[INPUT_ATTENUATOR]==0)&&(Item_Index[Y_SENSITIVITY]<9))
          Item_Index[Y_SENSITIVITY]++; //����̽ͷ˥��=1ʱ
        if((Item_Index[INPUT_ATTENUATOR]==1)&&(Item_Index[Y_SENSITIVITY]<18))
          Item_Index[Y_SENSITIVITY]++; //����̽ͷ˥��=10ʱ
        Key_Buffer=0;
      }
      if(((~GPIOA_IDR) & 0x0008)&&(Key_Buffer==KEYCODE_DOWN)){ // KEY_B & KEY_DOWN is push on
        Item_Index[CURRENT_ITEM]=Y_SENSITIVITY;
        Update[Item_Index[CURRENT_ITEM]]=1;
        ADC_Start();
        Frame=0;
        X_Counter=0;
        Item_Index[X_POSITION]=4096;
        Draw_Dot_Ti(Tp,ERASE,CH2_COLOR);
        Sync=0;
        if((Item_Index[INPUT_ATTENUATOR]==0)&&(Item_Index[Y_SENSITIVITY]>0))
          Item_Index[Y_SENSITIVITY]--; //����̽ͷ˥��=1ʱ
        if((Item_Index[INPUT_ATTENUATOR]==1)&&(Item_Index[Y_SENSITIVITY]>11))
          Item_Index[Y_SENSITIVITY]--; //����̽ͷ˥��=10ʱ
        Key_Buffer=0;
      }
      if(Item_Index[Y_SENSITIVITY]!=Ref_Buffer [302]) Hide_Index[REF]=1;
      if(Item_Index[X_SENSITIVITY]!=Ref_Buffer [303]) Hide_Index[REF]=1;

      switch (Item_Index[CURRENT_ITEM]) {
        case SYNC_MODE:
          if(Key_Buffer==KEYCODE_LEFT)  Item_Index[CURRENT_ITEM]=TRIG_LEVEL;
          if(Key_Buffer==KEYCODE_RIGHT) Item_Index[CURRENT_ITEM]=Y_SENSITIVITY;
          if(Key_Buffer==KEYCODE_MANU){
            Hide_Index[REF]=1-Hide_Index[REF];                //����ʾ/�������ο����Ρ�
          }
          if(Key_Buffer==KEYCODE_DOWN){             //ָ����һ��ɨ��ͬ����ʽ  Char_to_Str
            if(Item_Index[SYNC_MODE]>0) Item_Index[SYNC_MODE]--;
            else                        Item_Index[SYNC_MODE]=5;
            ADC_Start();
            Sync=0;
            Frame=0;
            Item_Index[RUNNING_STATUS]=RUN;
            Update[RUNNING_STATUS]=1;
          }
          if(Key_Buffer==KEYCODE_UP){               //ָ����һ��ɨ��ͬ����ʽ
            if(Item_Index[SYNC_MODE]<5) Item_Index[SYNC_MODE]++;
            else                        Item_Index[SYNC_MODE]=0;
            ADC_Start();
	    Sync=0;
            Frame=0;
            Item_Index[RUNNING_STATUS]=RUN;
            Update[RUNNING_STATUS]=1;
          }
          if(Item_Index[SYNC_MODE]==5){
            Draw_Dot_Vn(Item_Index[VT]+Item_Index[VS], ERASE, LN1_COLOR);
            Draw_Dot_Vn(Item_Index[VT]-Item_Index[VS], ERASE, LN1_COLOR);
            Draw_Vn(Item_Index[VT]+Item_Index[VS], ERASE, LN1_COLOR);
            Draw_Vn(Item_Index[VT]-Item_Index[VS], ERASE, LN1_COLOR);
            Item_Index[TRIG_SENSITIVITY]=3;
            Hide_Index[VT]=1;
            Update[VERNIERS]=1;
          }

        break;

        case Y_SENSITIVITY:
          if(Key_Buffer==KEYCODE_LEFT)  Item_Index[CURRENT_ITEM]=SYNC_MODE;
          if(Key_Buffer==KEYCODE_RIGHT) Item_Index[CURRENT_ITEM]=X_SENSITIVITY;
          if(Key_Buffer==KEYCODE_MANU){
            Hide_Index[REF]=1-Hide_Index[REF];                //����ʾ/�������ο����Ρ�
          }
          if(Key_Buffer==KEYCODE_UP){
            ADC_Start();
            Frame=0;
            X_Counter=0;
            Item_Index[X_POSITION]=4096;
            Draw_Dot_Ti(Tp,ERASE,CH2_COLOR);
            Sync=0;
            if((Item_Index[INPUT_ATTENUATOR]==0)&&(Item_Index[Y_SENSITIVITY]<9))
              Item_Index[Y_SENSITIVITY]++; //����̽ͷ˥��=1ʱ
            if((Item_Index[INPUT_ATTENUATOR]==1)&&(Item_Index[Y_SENSITIVITY]<18))
              Item_Index[Y_SENSITIVITY]++; //����̽ͷ˥��=10ʱ
          }
          if(Key_Buffer==KEYCODE_DOWN){
            ADC_Start();
            Frame=0;
            X_Counter=0;
            Item_Index[X_POSITION]=4096;
            Draw_Dot_Ti(Tp,ERASE,CH2_COLOR);
            Sync=0;
            if((Item_Index[INPUT_ATTENUATOR]==0)&&(Item_Index[Y_SENSITIVITY]>0))
              Item_Index[Y_SENSITIVITY]--; //����̽ͷ˥��=1ʱ
            if((Item_Index[INPUT_ATTENUATOR]==1)&&(Item_Index[Y_SENSITIVITY]>11))
              Item_Index[Y_SENSITIVITY]--; //����̽ͷ˥��=10ʱ
          }
          if(Item_Index[Y_SENSITIVITY]!=Ref_Buffer [302]) Hide_Index[REF]=1;
        break;

        case X_SENSITIVITY:
          if(Key_Buffer==KEYCODE_LEFT)  Item_Index[CURRENT_ITEM]=Y_SENSITIVITY;
          if(Key_Buffer==KEYCODE_RIGHT) Item_Index[CURRENT_ITEM]=Y_POSITION;
          if(Key_Buffer==KEYCODE_MANU){
            Hide_Index[REF]=1-Hide_Index[REF];                //����ʾ/�������ο����Ρ�
          }
          if((Key_Buffer==KEYCODE_UP)&&(Item_Index[X_SENSITIVITY]<21)){
            Item_Index[X_SENSITIVITY]++;
            ADC_Start();
            Frame=0;
            X_Counter=0;
            Item_Index[X_POSITION]=4096;
            Draw_Dot_Ti(Tp,ERASE,CH2_COLOR);
            Sync=0;
          }
          if((Key_Buffer==KEYCODE_DOWN)&&(Item_Index[X_SENSITIVITY]>0)){
            Item_Index[X_SENSITIVITY]--;
            ADC_Start();
            Frame=0;
            X_Counter=0;
            Item_Index[X_POSITION]=4096;
            Draw_Dot_Ti(Tp,ERASE,CH2_COLOR);
            Sync=0;
          }
          if(Item_Index[X_SENSITIVITY]!=Ref_Buffer [303]) Hide_Index[REF]=1;
        break;

        case Y_POSITION:
          if(Key_Buffer==KEYCODE_LEFT)  Item_Index[CURRENT_ITEM]=X_SENSITIVITY;
          if(Key_Buffer==KEYCODE_RIGHT) Item_Index[CURRENT_ITEM]=MEASUR_KIND;
          if((Key_Buffer==KEYCODE_UP)&&(Item_Index[V0]<MAX_Y)){
            Draw_Dot_Vn(Item_Index[V0],ERASE,CH1_COLOR);
            Item_Index[V0]++;
            Hide_Index[V0]=0;
          }                                    //'��ֱλ��'�������ƣ���ʾ���ߡ�
          if((Key_Buffer==KEYCODE_DOWN)&&(Item_Index[V0]>(MIN_Y+1))){
            Draw_Dot_Vn(Item_Index[V0],ERASE,CH1_COLOR);
            Item_Index[V0]--;
            Hide_Index[V0]=0;
          }                                   //'��ֱλ��'�������ƣ���ʾ���ߡ�
          if(Key_Buffer==KEYCODE_MANU){
            Hide_Index[V0]=1-Hide_Index[V0];                //����ʾ/��������ֱλ�ƻ��ߡ�
            if(Hide_Index[V0]==0) Hide_Index[TP]=1;
          }
        break;

        case MEASUR_KIND:
          if(Key_Buffer==KEYCODE_LEFT) Item_Index[CURRENT_ITEM]=Y_POSITION;
          if(Key_Buffer==KEYCODE_RIGHT){
            Hide_Index[VS]=0;
            Item_Index[CURRENT_ITEM]=TRIG_SENSITIVITY;
          }
          if(Key_Buffer==KEYCODE_UP){
            if(Item_Index[MEASUR_KIND]<6) Item_Index[MEASUR_KIND]++;  //ѡ����һ�ֲ�������
            else                          Item_Index[MEASUR_KIND]=0;
          }
          if(Key_Buffer==KEYCODE_DOWN){
            if(Item_Index[MEASUR_KIND]>0) Item_Index[MEASUR_KIND]--;  //ѡ����һ�ֲ�������
            else                          Item_Index[MEASUR_KIND]=6;
          }
        break;  //�������͹��У�FREQN��CYCLE��DUTY��Vp-p��Vrms��Vavg DCV 7��

        case TRIG_SENSITIVITY:
          if(Key_Buffer==KEYCODE_LEFT){
            Hide_Index[VS]=1;
            Item_Index[CURRENT_ITEM]=MEASUR_KIND;
            Update[Y_VERNIER_2]=1;
          }
          if(Key_Buffer==KEYCODE_RIGHT){
            Item_Index[CURRENT_ITEM]++;//=TRIG_SLOPE;
            Hide_Index[VS]=1;
          }
          if(Key_Buffer==KEYCODE_MANU)  Hide_Index[VT]=1-Hide_Index[VT];  //����ʾ/������������ƽ����
          if((Key_Buffer==KEYCODE_UP)&&((Item_Index[TRIG_SENSITIVITY]+Item_Index[VT])<(MAX_Y-1))
             &&((Item_Index[VT]-Item_Index[TRIG_SENSITIVITY])>(MIN_Y+3))){
            Draw_Dot_Vn(Item_Index[VT]+Item_Index[VS], ERASE, LN1_COLOR);
            Draw_Dot_Vn(Item_Index[VT]-Item_Index[VS], ERASE, LN1_COLOR);
            Draw_Vn(Item_Index[VT]+Item_Index[VS], ERASE, LN1_COLOR);
            Draw_Vn(Item_Index[VT]-Item_Index[VS], ERASE, LN1_COLOR);
            Item_Index[TRIG_SENSITIVITY]++;                      //���ʹ���������
          }
          if((Key_Buffer==KEYCODE_DOWN)&&(Item_Index[TRIG_SENSITIVITY]>0)){
            Draw_Dot_Vn(Item_Index[VT]+Item_Index[VS], ERASE, LN1_COLOR);
            Draw_Dot_Vn(Item_Index[VT]-Item_Index[VS], ERASE, LN1_COLOR);
            Draw_Vn(Item_Index[VT]+Item_Index[VS], ERASE, LN1_COLOR);
            Draw_Vn(Item_Index[VT]-Item_Index[VS], ERASE, LN1_COLOR);
            Item_Index[TRIG_SENSITIVITY]--;                      //��ߴ���������
          }
        break;

        case TRIG_SLOPE:
          if(Key_Buffer==KEYCODE_LEFT){
            Hide_Index[VS]=0;
            Item_Index[CURRENT_ITEM]=TRIG_SENSITIVITY;
          }
          if(Key_Buffer==KEYCODE_RIGHT) Item_Index[CURRENT_ITEM]=INPUT_ATTENUATOR;
          if((Key_Buffer==KEYCODE_UP)||(Key_Buffer==KEYCODE_DOWN)){
            Item_Index[TRIG_SLOPE]=1-Item_Index[TRIG_SLOPE];  //ѡ�������ش���/�½��ش�����
          }
        break;

        case INPUT_ATTENUATOR:
          if(Key_Buffer==KEYCODE_LEFT) Item_Index[CURRENT_ITEM]=TRIG_SLOPE;
          if(Key_Buffer==KEYCODE_RIGHT){
            if(SD_Card_ON()) Item_Index[CURRENT_ITEM]=SAVE_WAVE_IMAGE;
            else Item_Index[CURRENT_ITEM]=OUTPUT_FREQUENCY;        //����SD������ָ��'Ƶ������趨'
          }
          if((Key_Buffer==KEYCODE_UP)||(Key_Buffer==KEYCODE_DOWN)){
            Item_Index[INPUT_ATTENUATOR]=1-Item_Index[INPUT_ATTENUATOR];//����1/��0.1������˥��ѡ��
          }
          if((Item_Index[INPUT_ATTENUATOR]==0)&&(Item_Index[Y_SENSITIVITY]>9))
            Item_Index[Y_SENSITIVITY]-=9;
          if((Item_Index[INPUT_ATTENUATOR]==1)&&(Item_Index[Y_SENSITIVITY]<10))
            Item_Index[Y_SENSITIVITY]+=9;
        break;

        case SAVE_WAVE_IMAGE:
          if(SD_Card_ON())
          {
            if(Key_Buffer==KEYCODE_LEFT) Item_Index[CURRENT_ITEM]=INPUT_ATTENUATOR;
            if(Key_Buffer==KEYCODE_RIGHT)  Item_Index[CURRENT_ITEM]=SAVE_WAVE_CURVE;
            if((Key_Buffer==KEYCODE_UP)&&(Item_Index[SAVE_WAVE_IMAGE]<255))
              Item_Index[SAVE_WAVE_IMAGE]++; //�����ļ���ź�+1
            if((Key_Buffer==KEYCODE_DOWN)&&(Item_Index[SAVE_WAVE_IMAGE]>1))
              Item_Index[SAVE_WAVE_IMAGE]--; //�����ļ���ź�-1
            if(Key_Buffer==KEYCODE_MANU){
              Update[SAVE_WAVE_IMAGE]=0;
              Cursor_Counter=0;
              if(FAT_Info()==0){
                __Char_to_Str(FileNum, Item_Index[SAVE_WAVE_IMAGE]);
                if(Open_BMP_File(FileNum)==0){
                  Display_Str(89,2,WHITE,PRN, "Save Image...");
          if(Writ_BMP_File()==0){
                    if(Item_Index[SAVE_WAVE_IMAGE]<255)
                      Item_Index[SAVE_WAVE_IMAGE]++;
                  } else Display_Str(89,2,WHITE,PRN,"ImageWriteErr");
                } else                Display_Str(89,2,WHITE,PRN,"!File NoPlace!");
              } else                  Display_Str(89,2,WHITE,PRN,"!Micro SD Err!");
            }
          } else {
            Item_Index[CURRENT_ITEM]=OUTPUT_FREQUENCY;                              //����SD������ָ��'Ƶ������趨'
          }
        break;

        case SAVE_WAVE_CURVE:
          if(SD_Card_ON())
          {
            if(Key_Buffer==KEYCODE_LEFT) Item_Index[CURRENT_ITEM]=SAVE_WAVE_IMAGE;
            if(Key_Buffer==KEYCODE_RIGHT){
              Item_Index[CURRENT_ITEM]=LOAD_WAVE_CURVE;
              Hide_Index[REF]=1;                     // �����ο����Ρ�
            }
            if((Key_Buffer==KEYCODE_UP)&&(Item_Index[SAVE_WAVE_CURVE]<255))
              Item_Index[SAVE_WAVE_CURVE]++; //�����ļ���ź�+1
            if((Key_Buffer==KEYCODE_DOWN)&&(Item_Index[SAVE_WAVE_CURVE]>1))
              Item_Index[SAVE_WAVE_CURVE]--; //�����ļ���ź�-1
            if(Key_Buffer==KEYCODE_MANU){
              Update[SAVE_WAVE_CURVE]=0;
              Cursor_Counter=0;
              if(FAT_Info()==0){
                __Char_to_Str(FileNum, Item_Index[SAVE_WAVE_IMAGE]);
                if(Open_DAT_File(FileNum)==0){
		  F_Buff[0]=1;  F_Buff[1]=0;//Ver0001
                  memcpy(F_Buff+2, View_Buffer, 300);
                  memcpy(F_Buff+302, Item_Index, 40);
                  memcpy(F_Buff+342, Hide_Index, 20);
                  if(Write_File()==0){
                    Item_Index[LOAD_WAVE_CURVE]=Item_Index[SAVE_WAVE_CURVE]; //����ǰд����ļ���Ŷ�Ϊ��ȡ�ļ����ȱʡֵ
                    if(Item_Index[SAVE_WAVE_CURVE]<255){
                      Item_Index[SAVE_WAVE_CURVE]++;
                    }
                  Update[SAVE_WAVE_CURVE]=1;
                  } else  Display_Str(89,2,WHITE,PRN,"!FileWriteErr!");
                } else    Display_Str(89,2,WHITE,PRN,"!File NoPlace!");
              } else      Display_Str(89,2,WHITE,PRN,"!Micro SD Err!");
            }
          } else {
            Item_Index[CURRENT_ITEM]=OUTPUT_FREQUENCY;                              //����SD������ָ��'Ƶ������趨'
          }
        break;

        case LOAD_WAVE_CURVE:
          if(SD_Card_ON()){
            if(Key_Buffer==KEYCODE_LEFT)   Item_Index[CURRENT_ITEM]=SAVE_WAVE_CURVE;
            if(Key_Buffer==KEYCODE_RIGHT) Item_Index[CURRENT_ITEM]=OUTPUT_FREQUENCY;
            if((Key_Buffer==KEYCODE_UP)&&(Item_Index[LOAD_WAVE_CURVE]<255))
              Item_Index[LOAD_WAVE_CURVE]++; //�����ļ���ź�+1
            if((Key_Buffer==KEYCODE_DOWN)&&((Item_Index[LOAD_WAVE_CURVE])>1))
              Item_Index[LOAD_WAVE_CURVE]--; //�����ļ���ź�-1
            if(Key_Buffer==KEYCODE_MANU) {        //��'M'��
              Cursor_Counter=0;
              if(FAT_Info()==0){
                __Char_to_Str(FileNum, Item_Index[SAVE_WAVE_IMAGE]);
                if(Open_DAT_File(FileNum)==0){
                  if(Read_File()==0){
                    memcpy(Ref_Buffer, F_Buff+2, 300);
                    if((F_Buff[0]==1)&&(F_Buff[1]==0)){ //Ver0001
                      memcpy(Item_Index, F_Buff+302, 40);
                      memcpy(Hide_Index, F_Buff+342, 20);
                      memset(Update, 1, 24);
                    }
                  } else  Display_Str(89,2,WHITE,PRN,"!File ReadErr!");
                } else    Display_Str(89,2,WHITE,PRN,"!File NoFound!");
              } else      Display_Str(89,2,WHITE,PRN,"!Micro SD Err!");
              Hide_Index[REF]=0;//;��ʾ�²ο���������
            }
          } else  Item_Index[CURRENT_ITEM]=OUTPUT_FREQUENCY; //����SD������ָ��'Ƶ������趨'
        break;

        case OUTPUT_FREQUENCY:
          if(Key_Buffer==KEYCODE_LEFT) {
            if(SD_Card_ON()){
              Item_Index[CURRENT_ITEM]=LOAD_WAVE_CURVE;
              Hide_Index[REF]=1;                     // �����ο����Ρ�
            }
            else Item_Index[CURRENT_ITEM]=INPUT_ATTENUATOR;  //����SD������ָ��'����˥��ѡ��'
          }
          if(Key_Buffer==KEYCODE_RIGHT) Item_Index[CURRENT_ITEM]=X_VERNIER_2;
          if((Key_Buffer==KEYCODE_DOWN)&&(Item_Index[OUTPUT_FREQUENCY]<15))
            Item_Index[OUTPUT_FREQUENCY]++;  //��׼Ƶ���������һ��
          if((Key_Buffer==KEYCODE_UP)&&(Item_Index[OUTPUT_FREQUENCY]>0))
            Item_Index[OUTPUT_FREQUENCY]--;  //��׼Ƶ��������һ��
        break;

        case X_VERNIER_2:
          if(Key_Buffer==KEYCODE_LEFT)   Item_Index[CURRENT_ITEM]=OUTPUT_FREQUENCY;
          if(Key_Buffer==KEYCODE_RIGHT) Item_Index[CURRENT_ITEM]=X_VERNIER_1;
          if(Key_Buffer==KEYCODE_MANU){
            Hide_Index[T2]=1-Hide_Index[T2];                  //����ʾ/������ˮƽ�����α�T2
            Draw_Ti(Item_Index[T2],ERASE,LN2_COLOR);
          }
          if((Key_Buffer==KEYCODE_UP)&&(Item_Index[T2]<MAX_X)) { //ʱ������α���2����
            Draw_Dot_Ti(Item_Index[T2],ERASE,LN2_COLOR);
            Draw_Ti(Item_Index[T2],ERASE,LN2_COLOR);
            Item_Index[T2]++;
            Hide_Index[T1]=0;
            Hide_Index[T2]=0;
          }
          if((Key_Buffer==KEYCODE_DOWN)&&(Item_Index[T2]>MIN_X+1)) { //ʱ������α���2����
            Draw_Dot_Ti(Item_Index[T2],ERASE,LN2_COLOR);
            Draw_Ti(Item_Index[T2],ERASE,LN2_COLOR);
            Item_Index[T2]--;
            Hide_Index[T1]=0;
            Hide_Index[T2]=0;
          }
          Update[DELTA_T]=1;
          Update[VERNIERS]=1;
        break;

        case X_VERNIER_1:
          if(Key_Buffer==KEYCODE_LEFT) Item_Index[CURRENT_ITEM]=X_VERNIER_2;
          if(Key_Buffer==KEYCODE_RIGHT) {
            Item_Index[CURRENT_ITEM]=X_POSITION;
            Draw_View_Area();
          }
          if(Key_Buffer==KEYCODE_MANU){
            Hide_Index[T1]=1-Hide_Index[T1];                  //����ʾ/������ˮƽ�����α�T1
            Draw_Ti(Item_Index[T1],ERASE,LN2_COLOR);
          }
          if((Key_Buffer==KEYCODE_UP)&&(Item_Index[T1]<MAX_X)) { //ʱ������α���1����
            Draw_Dot_Ti(Item_Index[T1],ERASE,LN2_COLOR);
            Draw_Ti(Item_Index[T1],ERASE,LN2_COLOR);
            Item_Index[T1]++;
            Hide_Index[T1]=0;
            Hide_Index[T2]=0;
          }
          if((Key_Buffer==KEYCODE_DOWN)&&(Item_Index[T1]>MIN_X+1)) { //ʱ������α���1����
            Draw_Dot_Ti(Item_Index[T1],ERASE,LN2_COLOR);
            Draw_Ti(Item_Index[T1],ERASE,LN2_COLOR);
            Item_Index[T1]--;
            Hide_Index[T1]=0;
            Hide_Index[T2]=0;
          }
          Update[DELTA_T]=1;
          Update[VERNIERS]=1;
        break;

        case X_POSITION:

          Draw_Dot_Ti(Tp,ERASE,CH2_COLOR);
          if(Key_Buffer==KEYCODE_MANU){
            Hide_Index[TP]=1-Hide_Index[TP];                  //����ʾ/����������ָʾ�α�Tp
            Draw_Ti(Tp,ERASE,CH2_COLOR);
            if(Hide_Index[TP]==0) Hide_Index[V0]=1;
          }

          if(Key_Buffer==KEYCODE_LEFT) {
            Item_Index[CURRENT_ITEM]=X_VERNIER_1;
            Erase_View_Area();
          }
          if(Key_Buffer==KEYCODE_RIGHT){
            Item_Index[CURRENT_ITEM]=Y_VERNIER_2;
            Update[17]=1;
            Erase_View_Area();
          }
          if(Key_Buffer==KEYCODE_UP){
            Draw_Ti(Tp,ERASE,CH2_COLOR);
            Draw_Dot_Ti(Tp,ERASE,CH2_COLOR);
            Stop=0;
            Frame=0;
            Item_Index[X_POSITION]+=25;   //ˮƽ��ʾ��ʼλ������10
            Draw_View_Area();
          }
          if(Key_Buffer==KEYCODE_DOWN){
            Draw_Ti(Tp,ERASE,CH2_COLOR);
            Draw_Dot_Ti(Tp,ERASE,CH2_COLOR);
            Stop=0;
            Frame=0;
            Item_Index[X_POSITION]-=25; //ˮƽ��ʾ��ʼλ������25
            Draw_View_Area();
          }
          Update[VERNIERS]=1;
        break;

        case Y_VERNIER_2:
          if(Key_Buffer==KEYCODE_LEFT) {
            Item_Index[CURRENT_ITEM]=X_POSITION;
            Draw_View_Area();
          }
          if(Key_Buffer==KEYCODE_RIGHT) Item_Index[CURRENT_ITEM]=Y_VERNIER_1;
            if(Key_Buffer==KEYCODE_MANU){
            Hide_Index[V2]=1-Hide_Index[V2];                  //����ʾ/��������ֱ�����α�V2��
            Draw_Vi(Item_Index[V2],ERASE,LN2_COLOR);
          }
          if((Key_Buffer==KEYCODE_UP)&&(Item_Index[V2]<MAX_Y)){
            Draw_Dot_Vn(Item_Index[V2],ERASE,LN2_COLOR);
            Draw_Vi(Item_Index[V2],ERASE,LN2_COLOR);
            Item_Index[V2]++;                               //��ֱ�����α�V2����
          }
          if((Key_Buffer==KEYCODE_DOWN)&&(Item_Index[V2]>MIN_Y+1)){
            Draw_Dot_Vn(Item_Index[V2],ERASE,LN2_COLOR);
            Draw_Vi(Item_Index[V2],ERASE,LN2_COLOR);
            Item_Index[V2]--;                               //��ֱ�����α�V2����
          }
          Update[VERNIERS]=1;
        break;

        case Y_VERNIER_1:
          if(Key_Buffer==KEYCODE_LEFT) Item_Index[CURRENT_ITEM]=Y_VERNIER_2; //ָ��'��ֱ�����α�V2����'ģʽ
          if(Key_Buffer==KEYCODE_RIGHT)  Item_Index[CURRENT_ITEM]=TRIG_LEVEL; //ָ��'������ƽ�趨'ģʽ
          if(Key_Buffer==KEYCODE_MANU){
            Hide_Index[V1]=1-Hide_Index[V1];                  //����ʾ/��������ֱ�����α�V1��
            Draw_Vi(Item_Index[V1],ERASE,LN2_COLOR);
          }
          if((Key_Buffer==KEYCODE_UP)&&(Item_Index[V1]<MAX_Y)){
            Draw_Dot_Vn(Item_Index[V1],ERASE,LN2_COLOR);
            Draw_Vi(Item_Index[V1],ERASE,LN2_COLOR);
            Item_Index[V1]++;                               //��ֱ�����α�V1����
          }
          if((Key_Buffer==KEYCODE_DOWN)&&(Item_Index[V1]>MIN_Y+1)){
            Draw_Dot_Vn(Item_Index[V1],ERASE,LN2_COLOR);
            Draw_Vi(Item_Index[V1],ERASE,LN2_COLOR);
            Item_Index[V1]--;                               //��ֱ�����α�V1����
          }
          Update[VERNIERS]=1;
          break;

        case TRIG_LEVEL:
          if(Key_Buffer==KEYCODE_LEFT) Item_Index[CURRENT_ITEM]=Y_VERNIER_1;
          if(Key_Buffer==KEYCODE_RIGHT)  Item_Index[CURRENT_ITEM]=SYNC_MODE;
          if(Key_Buffer==KEYCODE_MANU)  Hide_Index[VT]=1-Hide_Index[VT];  //����ʾ/������������ƽ����
          if((Key_Buffer==KEYCODE_UP)&&(Item_Index[VT]<(MAX_Y-Item_Index[TRIG_SENSITIVITY]))){
            Draw_Dot_Vn(Item_Index[VT]+Item_Index[VS], ERASE, LN1_COLOR);
            Draw_Dot_Vn(Item_Index[VT]-Item_Index[VS], ERASE, LN1_COLOR);
            Draw_Vn(Item_Index[VT]+Item_Index[VS], ERASE, LN1_COLOR);
            Draw_Vn(Item_Index[VT]-Item_Index[VS], ERASE, LN1_COLOR);
            Item_Index[VT]++;                               //������ƽ�α�Vt����
          }
          if((Key_Buffer==KEYCODE_DOWN)&&(Item_Index[V1]>(MIN_Y+5+Item_Index[TRIG_SENSITIVITY]))){
            Draw_Dot_Vn(Item_Index[VT]+Item_Index[VS], ERASE, LN1_COLOR);
            Draw_Dot_Vn(Item_Index[VT]-Item_Index[VS], ERASE, LN1_COLOR);
            Draw_Vn(Item_Index[VT]+Item_Index[VS], ERASE, LN1_COLOR);
            Draw_Vn(Item_Index[VT]-Item_Index[VS], ERASE, LN1_COLOR);
            Item_Index[VT]--;                               //������ƽ�α�Vt����
          }
          Update[VERNIERS]=1;
          break;
      }
      Key_Buffer=0;
    }
  }

  return 0;
}
/********************************* END OF FILE ********************************/
