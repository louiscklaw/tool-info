/********************* (C) COPYRIGHT 2014 e-Design Co.,Ltd. ********************
File Name : main.c
Version   : DS202 Ver 1.0                                          Author : bure
*******************************************************************************/
#include "Version.h"
#include "TouchPad.h"
#include "Process.h"
#include "Drive.h"
#include "Func.h"
#include "Draw.h"
#include "Bios.h"
#include "Menu.h"
#include "Disk.h"
#include "LCD.h"
#include "FAT12.h"
#include "File.h"
#include "Math.h"

typedef void (*pFunc)(void);
void MSD_Disk_Config(void);
void Set_Licence(u16 x, u16 y);

//===============================APP�汾��======================================
u8  APP_VERSION[] = "APP V1.28";

u8  Key_Flag = 0, CalPop_Flag = 1, ResPop_Flag = 1; 
u8  Menu_Temp[5], NumStr[20];
u16 FileInfo,     Label_Cnt;

void main(void)
{
  //===============================ϵͳ��ʼ��===================================
  __Ctrl(SYS_CFG, RCC_DEV | TIM_DEV | GPIO_OPA | ADC_DAC  | I2C_SPI);
  __Ctrl(LCD_CLR, BLK);
  __Disp_Logo(LOGO_X, 140);
  /*-------���ɲ�ͬ����APPʱҪ���Ĵ˴���icf�ļ�-------------------------------*/
  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x8000);      //��һ����APP
  // NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x20000);  //�ڶ�����APP
  SysTick_Config(SystemCoreClock/1000);                 //SysTick = 1mS
  __Ctrl(B_LIGHT, 50);                                  //�� LCD �������� 50%
  __Ctrl(PM_CTRL, CHG_ACT);                            //���ģʽ LED����
  __Ctrl(BUZZVOL, 50);                                 //�趨����������(0~100%) 
  Beep(200);                                           //200mS                                     
  USB_MSD_Config();
  Init_Fat_Value();
  TIM6_Config(); 
  __Ctrl(SMPL_ST, DISABLE);
  __Ctrl(SMPL_ST, SIMULTANEO);
  __Ctrl(SMPLBUF, (u32)Smpl);
  __Ctrl(SMPLNUM, DEPTH[PopMenu1_Value[WIN_Depth]]);
  __Ctrl(SMPL_ST, ENABLE);
  
  //=============================��ʾ������ʾ��Ϣҳ��===========================  
  SetColor(BLK, WHT);
  DispStr(8,      90, PRN, "      Oscilloscope DS202 ");
  DispStr(8+25*8, 90, PRN,                            APP_VERSION);
  DispStr(8,      70, PRN, "        System Initializing...       ");
  __Ctrl(DELAYmS, 500); 
  
  ADC_StartConversion(ADC1);  
  ADC_StartConversion(ADC3);
  memset(VRAM_PTR+TR1_pBUF, ~0,    900); 
  memcpy(VRAM_PTR+TAB_PTR,  PARAM,  90);  
  __Ctrl(SYS_CFG, CTP_DEV);
  Gesture_Enable();                            //����������ʹ��
  CTP_Upgrade();                               //�������̼���������
  
  //=============================��һ��д�̼��Զ�У׼===========================   
  Read_CalFlag();
  if(Cal_Flag == 1){  
    Cal_Flag = 0;
    SetColor(BLK, WHT);
    DispStr(8, 90, PRN, "                                        ");
    DispStr(8, 70, PRN, "                                        ");
    DispStr(8, 90, PRN, "      Run the calibration program...    ");
    DispStr(8, 70, PRN, "        Please wait a few seconds       ");
    Zero_Align();                              //��ֱλ�����У�� 
    Save_Kpg();
  }
  //=============================��������ʾ=====================================   
  Read_Kpg();                                   //��ȡУ׼����
  Load_Param();                                 //��U�̶�ȡ�������
  ClrScrn(DAR);                                 //��������
  menu.menu_flag = 1;                           //menu���˵�
  menu.mflag  |= UPD;                           //menuѡ��
  menu.iflag  |= UPD;                           //menu�Ӳ˵�
  Label_Flag  |= UPD;                           //�α�
  Show_Title();                                 //��ʾ
  Show_Menu();                                  //���˵���ѡ��   
  Update_Proc_All();                            //����ˢ��
  Update_Label();                               //�α���ʾ
  Battery_Show();              
  MenuCnt = 5000;                               //Menu���˵���һ�δ���ʱ��
  PD_Cnt      = PopMenu3_Value[SYS_Standy]*Unit;   //��Ļ����ʱ��
  AutoPwr_Cnt = PopMenu3_Value[SYS_PowerOff]*Unit; //�Զ��ػ�ʱ��
  
  //========================��� Licence ��ȷ��ر�DEMO��=======================   
  if(__Info(LIC_OK) == 1){  
    PopType &= ~DEMO_POP;  
    ClosePop();
  }
  else Demo_Pop();
  
  //===================================��ѭ������===============================
  while(1){
    //=================��ADC����=====================   
    SetColor(BLK, WHT);
    Word2Hex(NumStr,  ADC1_2->CDR ); 
    Word2Hex(NumStr,  ADC3_4->CDR ); 
    //====================����=======================
    if((PopMenu3_Value[SYS_Standy]!=0) && (PD_Cnt == 0)){
      __Ctrl(B_LIGHT,0);                   //�رձ��� 
      __Ctrl(PM_CTRL, FLASH1HZ);           //LED��˸
    }
    //==================�Զ��ػ�=====================
    if((PopMenu3_Value[SYS_PowerOff] != 0) && (AutoPwr_Cnt == 0)){
      Beep(500);
      __Ctrl(DELAYmS, 500);                //�ػ�ǰ��������0.5s��ʾ 
      __Ctrl(PM_CTRL, CHG_ACT);            //���ģʽ LED���� 
      __Ctrl(PM_CTRL, PWR_OFF);            //power off
    }
    
    if(menu.menu_flag == 1)Show_Menu();
    if((PopType & DEMO_POP)&&!(PopType&(DAILOG_POP|PWR_POP|LIST_POP|FILE_POP)))
      MovePop();                          //δ��������ʾDemo����    
    if(About_Flag == 0){                   //��ʾAboutʱ����ˢ�²��δ���
      Process();
      __DrawWindow(VRAM_PTR);
    }
    Keys_Detect();                         //����ɨ��
    
    //===========���ز˵�ʱ��ֻ����������Ч�ʹ�����S������Ч===========
    if((menu.menu_flag == 1)||(KeyIn == K_OK)||(KeyIn == K_RUN)||(KeyIn == K_PWR)
       ||(KeyIn == RUN_HOLD)||(KeyIn == DOUBLE_K))
      Key_Flag = 1;
    else Key_Flag = 0;
    
    if(About_Flag == 1){                   //��ʾAboutʱ��ֻ�С�M�����ͽ�ͼ��Ч
      Key_Flag = 0;
      if((KeyIn == K_ACT)||(KeyIn == RUN_HOLD))
        Key_Flag = 1;
    }
    //=================================��������=================================     
    
    if(KeyIn && Key_Flag){       
      
      //==========�а����ָ��������Զ��ػ�ʱ��===============
      if((PopMenu3_Value[SYS_Standy] != 0) &&(PD_Cnt == 0)){
        __Ctrl(B_LIGHT, PopMenu3_Value[SYS_BKLight]*10);
        __Ctrl(PM_CTRL, CHG_ACT);          //���ģʽ LED���� 
      }
      PD_Cnt      = PopMenu3_Value[SYS_Standy]*Unit;   //����ʱ�� 
      AutoPwr_Cnt = PopMenu3_Value[SYS_PowerOff]*Unit; //�Զ��ػ�ʱ��
      
      //=======================��������=====================
      switch (KeyIn){
        //-------------��е����---------------- 
      case PWR_HOLD:
        break;  
        
      case RUN_HOLD:                       //����RUN������ͼ��ݰ���
        Beep(50); 
        __Ctrl(DELAYmS, 100);
        Beep(50); 
        FileInfo = Save_Bmp(PopMenu3_Value[SAVE_Bmp]);
        DispFileInfo(FileInfo);
        
        SetColor(DAR, ORN);
        Print_dT_Info(INV);                //��ʾT1-T2
        Update[T1F] &=~ UPD ;
        break;
        
      case K_PWR:                          //��Դ��
        if(PopType & PWR_POP){             //�ڹػ������£�����Դ���رմ���
          PopType &= ~PWR_POP;
          ClosePop();
        } 
        break;
        
      case K_RUN:                         //RUN��
        if((PopType & PWR_POP)){          //�ڹػ�������
          __Ctrl(B_LIGHT, 0);
          Save_One_Param(Option, SAVE_Bmp) ;
          __Ctrl(PM_CTRL, PWR_OFF);       //power off
        }
        else{                             //����ͣ������
          if(Status == STOP) {
            Status &= ~STOP;
            if(PopMenu1_Value[TRI_Sync] == SNGL)ADC_Start();
            if(PopMenu1_Value[TRI_Sync] == NORM)ADC_Start();
            Norm_Clr = 1;
            SNGL_Kflag = 1;
            Update_Proc_All();
          }
          else  {
            Status  |=  STOP;
            Ch1_Posi = PopMenu1_Value[CH1_Posi];
            Ch2_Posi = PopMenu1_Value[CH2_Posi];
            Ch3_Posi = PopMenu1_Value[CH3_Posi];
          }
          Update_Status();
        }
        break; 
        
        //-------------����ģ��----------------  
      case HOLD_K:                        //����
        if(PopType & (FILE_POP)){ 
          ClosePop(); 
          menu.current = Menu_Temp[0];
          menu.menu_index[menu.current] = Menu_Temp[1];
          break;          
        }
        else if(!(PopType & ( DAILOG_POP))){ 
          Menu_Temp[0] = menu.current;
          Menu_Temp[1] = menu.menu_index[menu.current];
          
          ClosePop();   
          PopCnt = 5000;                  //�趨 Pop �Զ��رն�ʱ 5000mS
          menu.current = Option;
          menu.menu_index[menu.current] = 0;
          Cur_PopItem = 1;
          Show_PopMenu(Cur_PopItem);      //�����Ӳ˵�_Pop
          List_Pop();
        }
        break;
      case DOUBLE_K:                      //˫��
        if(!PopMenu1_Value[TRI_Fit]){
          Auto_Fit();
          menu.iflag |= UPD;
        }
        break;
        
      case K_OK:
        if((PopType & PWR_POP)){          //�ػ�����
          __Ctrl(B_LIGHT, 0);
          Save_Param();
          __Ctrl(PM_CTRL, PWR_OFF);       //power off  
        }
        else if((PopType & FILE_POP)&&(menu.current == Option)  //�ļ��Ӳ˵�����
                &&(menu.menu_index[menu.current] == 0)){
                  FileInfo = 1;
                  if(Cur_PopItem == SAVE_PAM) {
                    menu.current = Menu_Temp[0];
                    menu.menu_index[menu.current] = Menu_Temp[1];
                    Save_Param();
                    FileInfo = 0;
                    DispFileInfo(FileInfo);
                    menu.current = Option;
                    menu.menu_index[menu.current] = 0;
                    Show_PopMenu(Cur_PopItem);
                    break;
                  }
                  else if(Cur_PopItem == SAVE_BMP) {
                    ClosePop();
                    __DrawWindow(VRAM_PTR);
                    FileInfo = Save_Bmp(PopMenu3_Value[SAVE_Bmp]); 
                    List_Pop();
                  }
                  else if(Cur_PopItem == SAVE_DAT) {
                    FileInfo = Save_Dat(PopMenu3_Value[SAVE_Dat]);
                  }
                  else if(Cur_PopItem == SAVE_BUF) {
                    FileInfo = Save_Buf(PopMenu3_Value[SAVE_Buf]);
                  }
                  else if(Cur_PopItem == SAVE_CSV) {
                    FileInfo = Save_Csv(PopMenu3_Value[SAVE_Csv]);
                  }
                  else if(Cur_PopItem == LOAD_DAT) {
                    FileInfo = Load_Dat(PopMenu3_Value[LOAD_Dat]);
                  }
                  else if(Cur_PopItem == LOAD_BUT) {
                    FileInfo = Load_Buf(PopMenu3_Value[LOAD_Buf]);
                  }
                  Show_PopMenu(Cur_PopItem);
                  Show_Title();
                  DispFileInfo(FileInfo);
                  if(PopType & DAILOG_POP)  ClosePop();
                  break;
                }
        
        else if((menu.current == Option) && (menu.menu_index[menu.current] == 3)
                && (PopType & (LIST_POP |DAILOG_POP))){   //CALУ׼ѡ��
                  if(Cur_PopItem == CAL_ZERO) {
                    if(CalPop_Flag == 1){
                      Dialog_Pop("Auto Calibration?");
                      PopCnt = 5000;
                      CalPop_Flag = 0;
                      break;
                    }
                    if(CalPop_Flag == 0){
                      if(PopType & DAILOG_POP){          //DAILOG_POP�Ի���ѡ��
                        Save_Kpg(); 
                        ClosePop();
                        CalPop_Flag = 1;
                      }
                      else if(PopType & LIST_POP){       //MENU_POP�Ի���ѡ��
                        if(Cur_PopItem == CAL_ZERO) {
                          ClosePop();
                          Tips_Pop("Waiting for Calibration ...");
                          __DrawWindow(VRAM_PTR);        //ˢ�½���
                          Zero_Align();
                          Update_Proc_All();
                          ClosePop();
                          Dialog_CalPop("Cal completed,Save data?",48,110,32,26*6);
                          PopCnt = 5000;
                        }
                      }
                    }
                  }
                  else if(Cur_PopItem == RES_DATA) {
                    if(ResPop_Flag ==1){
                      Dialog_Pop("  Restore Data ?");
                      PopCnt = 5000;
                      ResPop_Flag = 0;
                      break;
                    }
                    if(ResPop_Flag == 0){
                      if(PopType & DAILOG_POP){
                        menu.current = Oscillo;
                        menu.menu_index[Oscillo] = 0;
                        Save_Param();
                        ClosePop();
                        menu.mflag |= UPD;               //menuѡ��
                        Show_Menu();                     //���˵���ѡ�� 
                        ResPop_Flag = 1;
                      }
                      else if(PopType & LIST_POP){
                        Restore_OrigVal();
                        menu.current = Option;
                        menu.menu_index[Option] = 3;
                        Show_Title();                   //��ʾ
                        Show_Menu();                    //���˵���ѡ��   
                        Update_Proc_All();              //����ˢ��
                        ClosePop();
                        Dialog_CalPop(" Restored,Save Setting?", 90, 50, 32, 26*6);
                        PopCnt = 5000;                
                      }
                    }
                  }
                  break;
                }
        else if(!(PopType & LIST_POP)){ 
          if(menu.menu_flag){                          //���ز˵�����
            MenuCnt = 0;
            menu.menu_flag = 0;
            ParamTab[M_WX] = 300;
            Clear_Label_R(DAR);                        //��������
          }
          else{                                        //�����˵�����
            if(__Info(LIC_OK) == 1);                   //�л�����ʱdemo��λ
            else if((ParamTab[PXx1]+ParamTab[PWx1]) >= (WIDTH_MINI+1))Demo_Pop();
            ParamTab[M_WX] = WIDTH_MINI;
            menu.menu_flag = 1;
            Show_Menu();
            menu.mflag |= UPD;
          }
        }
        break;
        
      case K_ACT:
        if(PopType & PWR_POP){           //�ڹػ������£�����Դ���رմ���
          PopType &= ~PWR_POP;
          ClosePop();
        }
        
        else if(!(PopType & (LIST_POP|DAILOG_POP|FILE_POP))){ 
          //���Ӵ���ʱ�����Ӵ���
          PopCnt = 5000;                 // �趨 Pop �Զ��رն�ʱ 5000mS
          Cur_PopItem = 1;               // �Ӵ���Ĭ��ѡ��Ϊ��һѡ��
          Show_PopMenu(Cur_PopItem);     //�����Ӳ˵�_Pop
          if(PopType & FILE_POP){//��������ļ������Ӵ��ڣ���¼��ǰҳ�͵�ǰѡ��
            Menu_Temp[0] = menu.current;
            Menu_Temp[1] = menu.menu_index[menu.current];
          }
          if((menu.menu_index[menu.current] != 5)||(menu.current == 0))
            List_Pop();                  //��ص�ѹ��about����������
        }
        
        else if(PopType & (LIST_POP|DAILOG_POP|FILE_POP)){ 
          //���Ӵ���ʱ���ر��Ӵ���          
          if(PopType & FILE_POP){//�ļ������Ӵ���,�ָ���ʱ�ĵ�ǰҳ��ѡ��
            menu.current = Menu_Temp[0];
            menu.menu_index[menu.current] = Menu_Temp[1];
          } 
          ClosePop();
          CalPop_Flag  = 1;             //Auto_Cal?
          ResPop_Flag  = 1;             //Restore?
          Windows_Flag = 0;             //�ر�windows
          Update_Windows();       
        }
        break;
        
      case K_UP:
        if((PopType & LIST_POP)|| (Windows_Pop == 1)){   //�Ӳ˵�Popѡ��
          if((menu.current == Option) && (menu.menu_index[menu.current] == 1) 
             && (PopMenu3_Value[WAVE_Type] > 0)){
               if(Cur_PopItem <= 1)           //menu_key_chose
                 Cur_PopItem = Cur_Limit-1 ;  //ģ�����ʱ��DUTY��ѭ��
               else Cur_PopItem--;            
             }else{
               if(Cur_PopItem <= 1)           //menu_key_chose
                 Cur_PopItem = Cur_Limit ;
               else Cur_PopItem--;
             }
          Show_PopMenu(Cur_PopItem);
          menu.mflag &= ~UPD;
          menu.iflag &= ~UPD;
	  CalPop_Flag = 1;
          ResPop_Flag = 1;
        }
        else if(PopType & FILE_POP){       //�ļ�����Popʱ����ѡ��
          menu.current = Option;
          menu.menu_index[menu.current] = 0;
          if(Cur_PopItem <= 1)             //menu_key_chose
            Cur_PopItem = Cur_Limit ;
          else Cur_PopItem--;
          Show_PopMenu(Cur_PopItem);
          menu.mflag &= ~UPD;
          menu.iflag &= ~UPD;
        }
        else if(!(PopType & (DAILOG_POP | PWR_POP))){     //���˵�ѡ��
          if(menu.menu_index[menu.current] <= 0)          //menu_key_chose
            menu.menu_index[menu.current] = Menu_Limit[menu.current]-1;
          else 
            menu.menu_index[menu.current]--;
          menu.iflag |= UPD;
        }
        break;
        
      case K_DOWN:
        if((PopType & LIST_POP)|| (Windows_Pop == 1)){
          if((menu.current == Option)&&
             (menu.menu_index[menu.current] == 1)
               &&(PopMenu3_Value[WAVE_Type] > 0)){
                 if(Cur_PopItem >= Cur_Limit-1)           //menu_key_chose
                   Cur_PopItem = 1 ;
                 else Cur_PopItem++;
                 
               }else{
                 if(Cur_PopItem >= Cur_Limit  )           //menu_key_chose
                   Cur_PopItem = 1;
                 else 
                   Cur_PopItem++;
               }
          Show_PopMenu(Cur_PopItem);
          menu.mflag &= ~UPD;
          menu.iflag &= ~UPD;
          CalPop_Flag = 1;
          ResPop_Flag = 1;
        }
        else if(PopType & FILE_POP){                      //�ļ�����Popʱ����
          menu.current = Option;
          menu.menu_index[menu.current] = 0;
          if(Cur_PopItem >= Cur_Limit  )                  //menu_key_chose
            Cur_PopItem = 1;
          else 
            Cur_PopItem++;
          Show_PopMenu(Cur_PopItem);
          menu.mflag &= ~UPD;
          menu.iflag &= ~UPD;
        }
        else if(!(PopType & (DAILOG_POP | PWR_POP))){
          if(menu.menu_index[menu.current] >= Menu_Limit[menu.current]-1)
            menu.menu_index[menu.current] = 0;            //menu_key_chose
          else 
            menu.menu_index[menu.current]++;
          menu.iflag |= UPD;
        }
        break;        
        
      case K_LEFT:
        
        if(PopType & FILE_POP) {                          //�ļ�����Popʱ����
          PMenu_Proc(dec, Cur_PopItem, 0);
          menu.mflag &= ~UPD;
          menu.iflag &= ~UPD;
        }
        else {
          if((PopType & LIST_POP)|| (Windows_Pop == 1)) {
            PMenu_Proc(dec, Cur_PopItem, 0);
            CalPop_Flag = 1;
            ResPop_Flag = 1;
          }
          else if(!(PopType & (DAILOG_POP | PWR_POP)))  Item_Proc(dec);
        }
        break;
        
      case K_RIGHT:
        
        if(PopType & FILE_POP) {                      //�ļ�����Popʱ����
          PMenu_Proc(add, Cur_PopItem, 0);
          menu.mflag &= ~UPD;
          menu.iflag &= ~UPD;
        }
        else{
          if((PopType & LIST_POP) || (Windows_Pop == 1)) {
            PMenu_Proc(add, Cur_PopItem, 0);
            CalPop_Flag = 1;
            ResPop_Flag = 1;
          }
          else if(!(PopType & (DAILOG_POP | PWR_POP)))   Item_Proc(add);
        }
        break;
        
      }//----switch end-----//
      if((KeyIn == DOUBLE_K)){ 
        if(!PopMenu1_Value[TRI_Fit])Beep(50);        //��Auto_Fitʱ��˫������
      }
      else Beep(50);
      
      KeyIn = 0;
      Update_Label();
    }//---Key_In end------
    
    // ===============================APP �ػ�����==============================
    if(__Info(KEY_IN) != K_PWR) PwrCnt = 2000+1000;  //�����ػ���ʾ����ʱ 2000mS
    if(PwrCnt < 1000){                               //�˳��ػ���ʾ����ʱ 1000mS
      if(PwrCnt) {
        Power_Pop("   Power Option  ", 75, 80, 56, 24*6);
      }
    }
    
    if(About_Flag == 0){                             //�������ݶ�ʱˢ��
      if((Label_Cnt == 50)){
        Label_Cnt = 0;
        Label_Flag |= UPD;
        Update_Label();   
        Print_dT_Info(INV);
        Print_dV_Info(INV);
        Battery_update();
        if((menu.menu_flag == 1) && (menu.current == Measure))Show_Measure();
      }else Label_Cnt++;
    }
  }
}

/******************************** END OF FILE *********************************/
