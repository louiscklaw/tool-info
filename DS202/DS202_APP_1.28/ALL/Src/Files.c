/******************** (C) COPYRIGHT 2014 e-Design Co.,Ltd. *********************
Project Name: DS202 V1.0		
* File Name:  Files.c       Hardware: DS202 V2.1                     Author: xie
*******************************************************************************/
#include <stdlib.h>
#include "Version.h"
#include "string.h"
#include "STM32F30x.h"
#include "stm32f30x_flash.h"
#include "Draw.h"
#include "Func.h"
#include "File.h"
#include "FAT12.h"
#include "Menu.h"
#include "Process.h"
#include "Flash.h"
#include "Func.h"
#include "Drive.h"
#include "Bios.h"
#include "Lcd.h"

#define Page_Address    0x08007800   
#define Kpg_Address     0x08007800  /*����У׼���Kpg[]���ƫ�ƺ�����ϵ�� */

u8   Cal_Flag   = 1;
u8   Rest_Flag  = 0;
u8   Save_Flag  = 0;
u8   About_Flag = 0;
u8*  NamePtr;
u16  BmpNum;
u16  BMP_Ag_Color[16];

uc8  BmpHead[54]   = {0X42, 0X4D, 0X76, 0X96, 0X00, 0X00, 0X00, 0X00,
                      0X00, 0X00, 0X76, 0X00, 0X00, 0X00, 0X28, 0X00,
                      0X00, 0X00, 0X40, 0X01, 0X00, 0X00, 0XF0, 0X00,
                      0X00, 0X00, 0X01, 0X00, 0X04, 0X00, 0X00, 0X00,
                      0X00, 0X00, 0X82, 0X0B, 0X00, 0X00, 0X12, 0X0b,
                      0X00, 0X00, 0X12, 0X0b, 0X00, 0X00, 0X00, 0X00,
                      0X00, 0X00, 0X00, 0X00, 0X00, 0X00};

uc16 CLK_TAB[44] ={
   0x000,0x000,0x000,0x000,0x070,0x3FF,0x070,0x000,0x000,0x000,0x000,
   0x000,0x002,0x004,0x008,0x070,0x070,0x070,0x080,0x100,0x200,0x000,
   0x020,0x020,0x020,0x020,0x070,0x070,0x070,0x020,0x020,0x020,0x020,
   0x000,0x200,0x100,0x080,0x070,0x070,0x070,0x008,0x004,0x002,0x000};

uc16 BMP_Color[16] = {WHT,  CYN, CYN_,  YEL,  YEL_, PUR, PUR_, GRN,
                      GRN_, GRY, ORN, BLU, RED, BLK, LGN,  DAR};

u8   FileBuff[910];
u8   F_Sector[128];
u16  RsvdSecCnt, SectorSize, FAT_Size, FileLen;
u16  SecPerClus, DirFliePtr, DirSecNum, FAT1_Addr, FAT_Ptr;
u32  DiskStart, Hidden_Sec, Root_Addr, ClusterNum, File_Addr;
u32* FilePtr;

extern u8 DiskBuf[4096];

/*******************************************************************************
Color_Num: �����ǰ��ɫ�Ķ�Ӧ��ɫ����
*******************************************************************************/
u8 Color_Num(u16 Color)
{
  if(Color == WHT)                    return 0;
  else if((Color & CYN  )== CYN  )    return 1;
  else if((Color & CYN_ )== CYN_ )    return 2;
  else if((Color & YEL  )== YEL  )    return 3;
  else if((Color & YEL_ )== YEL_ )    return 4;
  else if((Color & PUR  )== PUR  )    return 5;
  else if((Color & PUR_ )== PUR_ )    return 6;
  else if((Color & GRN  )== GRN  )    return 7;
  else if((Color & GRN_ )== GRN_ )    return 8;
  else if((Color & GRY  )== GRY  )    return 9;
  else if((Color & ORN  )== ORN  )    return 10;
  else if((Color & BLU  )== BLU  )    return 11;
  else if((Color & RED  )== RED  )    return 12;
  else if((Color & BLK  )== BLK  )    return 13;
  else if((Color & LGN  )== LGN  )    return 14;
  else                                return 15;
}
/*******************************************************************************
 ��ָ����չ�����ļ�            ���룺�ļ���չ��             ����ֵ��0x00=�ɹ�
*******************************************************************************/
void Make_Filename(s16 FileNo,unsigned char* FileName)
{
  u8 Num[4];
  u8ToDec3Str(Num, FileNo);
  FileName[4] = Num[0];
  FileName[5] = Num[1];
  FileName[6] = Num[2];
}

/*******************************************************************************
Save_Dat: ���浱ǰ��Ļ��ʾͼ��ԭʼ����    ���룺�ļ����     ����ֵ��0x00=�ɹ�
*******************************************************************************/
u8 Save_Dat(s16 FileNo)
{
  u8  pFileName[12]="FILE    DAT";
  u16 i;
  u16 Rvalue=DISK_RW_ERR ;
  u16 pCluster[3];
  u32 pDirAddr[1];
  Make_Filename(FileNo, pFileName);
  memset(DiskBuf,0,4096);
  if(OpenFileWr(DiskBuf, pFileName, pCluster, pDirAddr)!=OK) return Rvalue;
  DiskBuf[0]=PopMenu1_Value[CH1_Posi];
  DiskBuf[1]=PopMenu1_Value[CH2_Posi];
  DiskBuf[2]=PopMenu1_Value[CH3_Posi];
  DiskBuf[3]=PopMenu1_Value[TIM_Base];
  DiskBuf[4]=PopMenu1_Value[CH1_Vol];
  DiskBuf[6]=PopMenu1_Value[CH2_Vol];
  
  for(i=0; i<3; i++){
    memcpy(DiskBuf+8+300*i, V_Buf+TR1_pBUF+300*i, 300);
  } 
  if(ProgFileSec(DiskBuf, pCluster)!= OK) return FILE_RW_ERR; // д������
  if(CloseFile(DiskBuf, 512*2, pCluster, pDirAddr)!= OK) return FILE_RW_ERR;
  if(PopMenu3_Value[SAVE_Dat]<99)PopMenu3_Value[SAVE_Dat]++;
  return OK;
}
/*******************************************************************************
Load_Dat: ���ر��������Ļͼ��ԭʼ����    ���룺�ļ����     ����ֵ��0x00=�ɹ�
*******************************************************************************/
u8 Load_Dat(s16 FileNo)
{
  u8 pFileName[12]="FILE    DAT";
  u16 pCluster[3];
  u32 pDirAddr[1];
  u16  Rvalue=0;
  u16 i;
  Make_Filename(FileNo, pFileName);
  if((Rvalue=OpenFileRd(DiskBuf, pFileName, pCluster, pDirAddr))!= OK) 
    return Rvalue;
  if(ReadFileSec(DiskBuf, pCluster)!= OK) return FILE_RW_ERR;
  for(i=0; i<3; i++){
    memcpy(FileBuff+300*i, DiskBuf+300*i, 300);
  }
  PopMenu1_Value[TIM_Base] = DiskBuf[3];
  PopMenu1_Value[CH1_Vol]  = DiskBuf[4];
  PopMenu1_Value[CH2_Vol]  = DiskBuf[6];
  if(PopMenu3_Value[LOAD_Dat]<99)PopMenu3_Value[LOAD_Dat]++;
  return OK;
}

/*******************************************************************************
Save_Bmp: ���浱ǰ��Ļ��ʾͼ��ΪBMP��ʽ    ���룺�ļ����     ����ֵ��0x00=�ɹ�
*******************************************************************************/
u8 Save_Bmp(s16 FileNo)
{
  u8  pFileName[12]="IMG_    BMP";
  u16 pCluster[3];
  u32 pDirAddr[1];
  u32 Rvalue=DISK_RW_ERR;
  s16 x, y, i=54, j,ColorH,ColorL ;
  u16 k=0, l=0;
  u32 length=4096;

  __USB_Port(DISABLE);
  Make_Filename(FileNo, pFileName);
  if(OpenFileWr(DiskBuf, pFileName, pCluster, pDirAddr)!=OK) return Rvalue;
  memcpy(DiskBuf, BmpHead, 54);
  i = 0x0036;                                     // ��ɫ���ſ�ʼ��ַ
  for(j=0; j<16; ++j){
    DiskBuf[j*4 +i+0]=(BMP_Color[j] & 0xF800)>>8;  // Bule
    DiskBuf[j*4 +i+1]=(BMP_Color[j] & 0x07E0)>>3;  // Green&
    DiskBuf[j*4 +i+2]=(BMP_Color[j] & 0x001F)<<3;  // Red
    DiskBuf[j*4 +i+3]= 0;                          // Alpha
  }
  i = 0x0076;                                     // ͼ�����ݿ�ʼ��ŵ�ַ
  for(y=0; y<240; y++){
    for(x=0; x<320 ; x+=2){
      __SetPosi(x, y); 
      ColorH = __ReadPixel();
      __SetPosi(x+1, y); 
      ColorL = __ReadPixel();
      DiskBuf[i] =(Color_Num(ColorH)<<4)+ Color_Num(ColorL);
      i++;
      if(i>=length){
        i=0;
        if(ProgFileSec(DiskBuf, pCluster)!= OK) return FILE_RW_ERR; // д������
        if(l==0)PrintClk(168,2,(k++ >>1)& 3);         // ����ָʾ
        l++;
        if(l>=2)l=0;
      }
    }
  }
  if(i!=0){
      if(ProgFileSec(DiskBuf, pCluster)!= OK){List_Pop();return FILE_RW_ERR;} 
    }
  if(CloseFile(DiskBuf, 76*512, pCluster, pDirAddr)!= OK) return FILE_RW_ERR;
  if(PopMenu3_Value[SAVE_Bmp]<99)PopMenu3_Value[SAVE_Bmp]++;
  __USB_Port(ENABLE);
  return OK;
}

/*******************************************************************************
Open_Bmp: ����Ļ��ʾͼ��ΪBMP��ʽ    ���룺�ļ����     ����ֵ��0x00=�ɹ�
*******************************************************************************/
u8 Open_Bmp(u8 FileNum)
{
    u8  Filename[12] = "ABOUT_F BMP";
    u16 width,high; 
    u16 i,j, x=10, y=20;
    u16 pCluster[3];
    u32 pDirAddr[1]; 
     
    if(OpenFileRd(DiskBuf, Filename, pCluster, pDirAddr)!=OK) {
      About_Pop(" About.file is Error ", 105, 20,16, 23*6 );
      About_Flag = 0;
      return FILE_RW_ERR;
    }
     if(ReadFileSec(DiskBuf, pCluster)!= OK) return FILE_RW_ERR;
     i = 0x0036;                              //��ɫ���ſ�ʼ��ַ
     for(j=0; j<16; j++){
       BMP_Ag_Color[j]=((DiskBuf[j*4 +i+0]<<8)&0xF800)
                      |((DiskBuf[j*4 +i+1]<<3)&0x07E0)
                      |((DiskBuf[j*4 +i+2]>>3)&0x001F);
     }
     width = DiskBuf[0x12];
     high =  DiskBuf[0x16];
     if((width == 250) &&(high == 200) ){
       Clear_Label_L(DAR);                   //�������ͨ��Сͼ��
       Draw_RECT(CYN, 7, 20, 203, 254, 2 );  //���վ���
       i = 0x0076;                           //ͼ�����ݿ�ʼ��ŵ�ַ
       for(y=22; y<199+22; y++){
         for(x=9; x<256+9 ; x+=2){
           if(x>249+9);
           else{
             __SetPosi(x, y);
             __SetPixel(BMP_Ag_Color[DiskBuf[i]>>4]);
             __SetPosi(x+1, y);
             __SetPixel(BMP_Ag_Color[DiskBuf[i]-(DiskBuf[i]>>4)*16]);
           }
           i++;
           if(i>=4096){
             if(ReadFileSec(DiskBuf, pCluster)!= OK) return FILE_RW_ERR;
             i=0;
           }
         }
       }       
       
     }else {
       About_Pop(" About.Bmp is Error ", 105, 20,16, 22*6 );
       About_Flag = 0;
     }
  return 0;
}

/*******************************************************************************
Save_Buf: ����ɼ����ݻ�����ΪBUF��ʽ    ���룺�ļ����     ����ֵ��0x00=�ɹ�
*******************************************************************************/
u8 Save_Buf(s16 FileNo)
{
  u8 pFileName[12]="DATA_   BUF";
  u16 pCluster[3];
  u32 pDirAddr[1];
  u32 i=0, Rvalue=DISK_RW_ERR;
  u16 *p;
  u8  l=0;
  u16 n,k;

  Make_Filename(FileNo, pFileName);
  memset(DiskBuf,0,4096);
  if(OpenFileWr(DiskBuf, pFileName, pCluster, pDirAddr)!=OK) return Rvalue;
  n=4; k=4096;
  
  for(i=0; i<n; i++){ 
    if(PopMenu1_Value[WIN_Depth]==3)
    {
      if(i<4)memcpy(DiskBuf,&(Smpl[i*k/2]),k);
      else   memcpy(DiskBuf,&(Smpl[i*k/2+4096]),k);
    }else{
      if(i<2)memcpy(DiskBuf,&(Smpl[i*k/2]),k);
      else   memcpy(DiskBuf,&(Smpl[i*k/2+4096]),k);
    }
    if(ProgFileSec(DiskBuf, pCluster)!= OK) return FILE_RW_ERR; // д������
    PrintClk(264,2,(l++ >>1) & 3);                              // ����ָʾ
  }
  memset(DiskBuf,0,k);
  p =(u16*)&DiskBuf;
  *p++=Smpl[0];
  *p++=Smpl[1];
  *p++=PopMenu1_Value[CH1_Vol];
  *p++=PopMenu1_Value[CH2_Vol];
  *p++=PopMenu1_Value[WIN_Depth];
  *p++=PopMenu1_Value[TIM_Base];
  if(ProgFileSec(DiskBuf, pCluster)!= OK) return FILE_RW_ERR; // д������
  if(CloseFile(DiskBuf, 34*512, pCluster, pDirAddr)!= OK) return FILE_RW_ERR;
  if(PopMenu3_Value[SAVE_Buf]<99)PopMenu3_Value[SAVE_Buf]++;
  return OK;
}

/*******************************************************************************
Load_Buf: ���ر�����Ĳɼ����ݻ�����    ���룺�ļ����     ����ֵ��0x00=�ɹ�
*******************************************************************************/
u8 Load_Buf(s16 FileNo)
{
  u8 pFileName[12]="DATA    BUF";
  u16 pCluster[3];
  u32 pDirAddr[1];
  u16 *ptr;
  u32 i,Rvalue=1;
  u32 n,k;

  Make_Filename(FileNo, pFileName);
  if( (Rvalue=OpenFileRd(DiskBuf, pFileName, pCluster, pDirAddr))!= OK) 
    return Rvalue;
  n=4;k=4096;
  for(i=0;i<n;i++){
    if(ReadFileSec(DiskBuf, pCluster)!= OK) return FILE_RW_ERR;
    if(PopMenu1_Value[WIN_Depth]==3)
    {
      if(i<4)memcpy(&(Smpl[i*k/2]),DiskBuf,k);
      else   memcpy(&(Smpl[i*k/2+4096]),DiskBuf,k);
    }else{
      if(i<2)memcpy(&(Smpl[i*k/2]),DiskBuf,k);
      else   memcpy(&(Smpl[i*k/2+4096]),DiskBuf,k);
    }
  }
  
  if(ReadFileSec(DiskBuf, pCluster)!= OK) return FILE_RW_ERR;
  ptr=(u16*)F_Sector;                               //�����ֳ�
  *ptr++=0xaa55;
  *ptr++=Smpl[0];
  *ptr++=Smpl[1];
  *ptr++=PopMenu1_Value[CH1_Vol];
  *ptr++=PopMenu1_Value[CH2_Vol];
  *ptr++=PopMenu1_Value[WIN_Depth];
  *ptr++=PopMenu1_Value[TIM_Base];  			
  ptr =(u16*)DiskBuf;                              //��ԭ���Ĳ���
  Smpl[0]=*ptr++;
  Smpl[1]=*ptr++;
  PopMenu1_Value[CH1_Vol]   = *ptr++;
  PopMenu1_Value[CH2_Vol]   = *ptr++;
  PopMenu1_Value[WIN_Depth] = *ptr++;
  PopMenu1_Value[TIM_Base] = *ptr++;
  Status |=  STOP;
  Update_Status();
  if(PopMenu3_Value[LOAD_Buf]<99)PopMenu3_Value[LOAD_Buf]++;
  return 0;
}


void make_Vertical(u8 TRACK,u8* buf,u8* len)
{
  u8 i=0;
  u8* ptr;

  ptr = (u8*)&Vol_Str[PopMenu1_Value[TRACK]];
  while(*ptr != 0){
    if(*ptr == 0x21)  buf[i] = 0x20;
    else              buf[i] = *ptr;
    ptr++;
    i++;
  };
  buf[i]   = 0x2c;
  buf[i+1] = 0;
  *len = i+1;
}
/*******************************************************************************
Save_Csv: ����ɼ����ݻ�����ΪCSV��ʽ    ���룺�ļ����     ����ֵ��0x00=�ɹ�
*******************************************************************************/
u8 Save_Csv(s16 FileNo)
{
  s32 Ak = (KgA[KindA]*4)/GK[GainA+5*KindA];
  s32 Bk = (KgB[KindB]*4)/GK[GainB+5*KindB]; // 8192~409
  
  u8 pFileName[12]="DATA    CSV";
  u16 pCluster[3];
  u32 pDirAddr[1];
  u32 i,j,k=0,l=0,Rvalue=DISK_RW_ERR,length;
  u8 Num[4],track[4];
  s16 temp;
  u8 count;
  
  length=4096;
  
  Make_Filename(FileNo, pFileName);
  if(OpenFileWr(DiskBuf, pFileName, pCluster, pDirAddr)!=OK) return Rvalue;
  memcpy(DiskBuf,"CH1",3);
  make_Vertical(CH1_Vol, &DiskBuf[3],&count);
  k=3+count;
  memcpy(&DiskBuf[k],"CH2",3);
  make_Vertical(CH2_Vol, &DiskBuf[k+3],&count);
  k=k+3+count;
  memcpy(&DiskBuf[k], "CH3\r\n", 4);
  k += 4;

  for(i=0; i<DEPTH[PopMenu1_Value[WIN_Depth]]; i++){
    temp = ((((Smpl[i])-2048)*Ak)>>12)+100;          // ���㵱ǰ���ε�Ļ�ֵ
    if(temp > 0){
      if(temp > 200)  track[0] = 199;
      else            track[0] = temp;
    } else            track[0] = 0;
    temp = ((((Smpl[i+8192])-2048)*Bk)>>12)+100;     // ���㵱ǰ���ε�Ļ�ֵ
    if(temp > 0){
      if(temp > 200)  track[1] = 199;
      else            track[1] = temp;
    } else            track[1] = 0;
    
   for(j=0; j<2; j++){
    u8ToDec3Str(Num,track[j]);
    for(count=0; count<3; count++){
      if(Num[count] == 0) break;
      DiskBuf[k++] = Num[count];
      if(k >= length){
        if(ProgFileSec(DiskBuf, pCluster)!= OK) return FILE_RW_ERR; // д������
        PrintClk(264,2,(l++ >>1) & 3);                              // ����ָʾ
        k = 0;
      }
    }
    DiskBuf[k++] = 0x2c;
    if(k >= length){
      if(ProgFileSec(DiskBuf, pCluster)!= OK) return FILE_RW_ERR; // д������
      PrintClk(264,2,(l++ >>1)& 3);                               // ����ָʾ
      k = 0;
    }
   }
    DiskBuf[k++] = 0x0d;
    if(k >= length){
      if(ProgFileSec(DiskBuf, pCluster)!= OK) return FILE_RW_ERR; // д������
      PrintClk(264,2,(l++ >>1)& 3);                               // ����ָʾ
      k = 0;
    }
    DiskBuf[k++] = 0x0a;
    if(k >= length){
      if(ProgFileSec(DiskBuf, pCluster)!= OK) return FILE_RW_ERR; // д������
      PrintClk(264,2,(l++ >>1)& 3);                               // ����ָʾ
      k = 0;
    }
  }
  if(k != 0){
    DiskBuf[k++]=0x0d;
    DiskBuf[k++]=0x0a;
    memset(&DiskBuf[k],0,(length-k));
    k=0;
    if(ProgFileSec(DiskBuf, pCluster)!= OK) return FILE_RW_ERR; // д������
    PrintClk(264,2,(l++ >>1)& 3);                               // ����ָʾ
  }
  if(CloseFile(DiskBuf, l*length, pCluster, pDirAddr)!= OK) 
    return DISK_RW_ERR;
  if(PopMenu3_Value[SAVE_Csv]<99)PopMenu3_Value[SAVE_Csv]++;
  return OK;
}
/*******************************************************************************
 Save_Parameter: ���浱ǰ�Ĺ������� ��Flash                  Return: 0= Success
*******************************************************************************/
u8 Save_Parameter(void)           
{
  unsigned short i, j;
  u16* ptr;

  FLASH_Unlock();
  j=FLASH_ErasePage(Page_Address);
  if(j== FLASH_COMPLETE){
    ptr=(u16*)&DiskBuf;
    *ptr++=0x0300;                              //Ver:3.00
    *ptr++=menu.current;
    *ptr++=Status;
    for(i=0;i<10;i++) *ptr++=menu.menu_index[i];
    for(i=0;i<33;i++) *ptr++=PopMenu1_Value[i];
    for(i=0;i<18;i++) *ptr++=PopMenu2_Value[i];
    for(i=0;i<17;i++) *ptr++=PopMenu3_Value[i];
    for(i=0;i<8;i++)  *ptr++=Kpg[i];             //88+3
    *ptr++=0xaa55;
    ptr=(u16*)&DiskBuf;
    for(i=0; i<256; i+=2){                       //128 SHORT
      j= FLASH_ProgramHalfWord(Page_Address+i, *ptr++);
      if(j!= FLASH_COMPLETE) break;
    }
  }
  FLASH_Lock();
  return j;
}

/*******************************************************************************
Read_Parameter: ��FLASH����������
*******************************************************************************/
void Read_Parameter(void)  //��FLASH����������
{
  u8 i;
  u16* ptr;
  ptr=(u16*)(Page_Address+89*2);
  if(*ptr!=0xaa55) return ;
  ptr=(u16*)Page_Address;
  if(*ptr++!=0x0300) return ;
  menu.current=*ptr++;
  ptr++;
  for(i=0;i<10;i++) menu.menu_index[i]=*ptr++;
  for(i=0;i<33;i++) PopMenu1_Value[i] =*ptr++;
  for(i=0;i<18;i++) PopMenu2_Value[i] =*ptr++;
  for(i=0;i<17;i++) PopMenu3_Value[i] =*ptr++;

  return ;
}
/*******************************************************************************
 Save_Kpg: ����У׼����                          Return: 0= Success
*******************************************************************************/
u8 Save_Kpg(void)
{
  unsigned short i, j;
  u16* ptr;

  FLASH_Unlock();
  j=FLASH_ErasePage(Kpg_Address);
  if(j== FLASH_COMPLETE){
    ptr=(u16*)&DiskBuf;
    *ptr++=0x0300;                              //Ver:3.00
    *ptr++=Cal_Flag;
    for(i=0;i<8;i++)  *ptr++=Kpg[i];
    *ptr++=0xaa55;
    ptr=(u16*)&DiskBuf;
    for(i=0; i<256; i+=2){                      //128 SHORT
      j= FLASH_ProgramHalfWord(Kpg_Address+i, *ptr++);
      if(j!= FLASH_COMPLETE) break;
    }
  }
  FLASH_Lock();
  return j;
}
/*******************************************************************************
Read_Kpg: ��FLASH����У׼����
*******************************************************************************/
void Read_Kpg(void)
{
  u8 i;
  u16* ptr;
  ptr=(u16*)(Kpg_Address+10*2);
  if(*ptr!=0xaa55) return ;
  ptr=(u16*)Kpg_Address;
  if(*ptr++!=0x0300) return ;
  Cal_Flag = *ptr++;
  for(i=0;i< 4;i++) Kpg[i] = *ptr++;  //CH_A,CH_B ���ƫ��
  return ;
}
/*******************************************************************************
Read_Kpg: ��FLASH����У׼����
*******************************************************************************/
void Read_CalFlag(void)
{
  u16* ptr;
  ptr=(u16*)(Kpg_Address+10*2);
  if(*ptr!=0xaa55) return ;
  ptr=(u16*)Kpg_Address;
  if(*ptr++!=0x0300) return ;
  Cal_Flag = *ptr++;
  return ;
}

/*******************************************************************************
 Print_Clk: ����ָʾ
*******************************************************************************/
void PrintClk(u16 x0, u16 y0, u8 Phase)
{
  u16 i, j, b;
  x0=272;
  SetColor(DAR, WHT);
  DispStr(x0-20, 2, PRN, "          ");
  for(i=0; i<11; ++i){
    b = CLK_TAB[Phase *11 + i];
    for(j=0; j<11; ++j){
      __SetPosi((x0+j), (y0+1+i));
      if((b >> j)& 0x001)
        __SetPixel(RED);
      else
        __SetPixel(Background);
    }
  }
}
/*******************************************************************************
 Save_Parameter: ���浱ǰ�Ĺ�������                         Return: 0= Success
*******************************************************************************/
u8 Save_Param(void)             // ���湤���������ļ� ��U��
{
  u8  Sum = 0, Filename[12], Versions = 0x06; 
  u16 i, Tmp[2];
  u16* ptr =(u16*)DiskBuf;
  
  u16 pCluster[3];
  u32 pDirAddr[1]; 

  Word2Hex(Filename, __Info(DEV_SN));
  Filename[8] = 'P'; Filename[9] = 'A'; Filename[10] = 'R';
  switch (OpenFileRd(DiskBuf, Filename, pCluster, pDirAddr)){
  case OK:                                                     //ԭWPT�ļ�����
    Tmp[0] = *pCluster;
    Filename[8] = 'B'; Filename[9] = 'A'; Filename[10] = 'K';  //ת��BAK�ļ�
    if(OpenFileWr(DiskBuf, Filename, pCluster, pDirAddr)!= OK)
      return DISK_RW_ERR;
    if(ReadFileSec(DiskBuf, Tmp     )!= OK) return FILE_RW_ERR;
    if(ProgFileSec(DiskBuf, pCluster)!= OK) return FILE_RW_ERR;//����BAK�ļ�
    if(CloseFile(DiskBuf, 512, pCluster, pDirAddr)!= OK) 
      return FILE_RW_ERR;
  case NEW:                                                    //ԭWPT�ļ�������
    Filename[8] = 'P'; Filename[9] = 'A'; Filename[10] = 'R';  //����WPT�ļ�
    if(OpenFileWr(DiskBuf, Filename, pCluster, pDirAddr)!= OK) 
      return DISK_RW_ERR;
    memset(DiskBuf, 0, 512);
    *ptr++ =(menu.current <<8)+ Versions;           //���������汾�ż���ǰҳ
    *ptr++=Status;
    for(i=0;i<10;i++) *ptr++=menu.menu_index[i];
    for(i=0;i<33;i++) *ptr++=PopMenu1_Value[i];
    for(i=0;i<18;i++) *ptr++=PopMenu2_Value[i];
    for(i=0;i<17;i++) *ptr++=PopMenu3_Value[i];

    for(i=0; i<511; i++)  Sum += DiskBuf[i];  // ���������У���
    DiskBuf[511] = (~Sum)+ 1;
    if(ProgFileSec(DiskBuf, pCluster)!= OK) return FILE_RW_ERR; // д������
    if(CloseFile(DiskBuf, 512, pCluster, pDirAddr)!= OK) 
      return FILE_RW_ERR;
    
    Save_Flag = 1; //���������־λ
    
    return OK;
    default:  return FILE_RW_ERR;
  }
}
/*******************************************************************************
 Save_One_Param: ���浱ǰ�Ĺ�������                         Return: 0= Success
*******************************************************************************/
u8 Save_One_Param(u8 Page, u8 Num)             // ���湤���������ļ� ��U��
{
  u8   Sum = 0, Filename[12], Versions = 0x06; 
  u16  i,Tmp[2];
  u16* ptr =(u16*)DiskBuf;
  s16  Temp[2];
  
  u16 pCluster[3];
  u32 pDirAddr[1]; 
  
  
  switch(Page){                           //��ȡ֮ǰ����Ĳ���
    case Oscillo:
      Temp[0] = PopMenu1_Value[Num]; 
      if(Load_Param())Restore_OrigVal();  //�������������ȡ��û�лָ���������
      else PopMenu1_Value[Num] = Temp[0]; //����Ҫ��������Ĳ���
      break;
    case Measure:
      Temp[0] = PopMenu2_Value[Num];
      if(Load_Param())Restore_OrigVal();
      else PopMenu2_Value[Num] = Temp[0];
      break;
    case Option:
      Temp[0] = PopMenu3_Value[Num];
      if(Load_Param())Restore_OrigVal();
      else {
        PopMenu3_Value[Num] = Temp[0];
         //�ָ�����û����ʱ��Bmp���Ϊδ�ָ�ǰ��ţ�
        if((Rest_Flag==1) && (Save_Flag==0))
        PopMenu3_Value[SAVE_Bmp] = BmpNum;
      }
      break;
    }

  Word2Hex(Filename, __Info(DEV_SN));
  Filename[8] = 'P'; Filename[9] = 'A'; Filename[10] = 'R';
  switch (OpenFileRd(DiskBuf, Filename, pCluster, pDirAddr)){
  case OK:                                                     // ԭWPT�ļ�����
    Tmp[0] = *pCluster;
    Filename[8] = 'B'; Filename[9] = 'A'; Filename[10] = 'K';  // ת��BAK�ļ�
    if(OpenFileWr(DiskBuf, Filename, pCluster, pDirAddr)!= OK)
      return DISK_RW_ERR;
    if(ReadFileSec(DiskBuf, Tmp     )!= OK) return FILE_RW_ERR;
    if(ProgFileSec(DiskBuf, pCluster)!= OK) return FILE_RW_ERR;  // ����BAK�ļ�
    if(CloseFile(DiskBuf, 512, pCluster, pDirAddr)!= OK) 
      return FILE_RW_ERR;/**/
  case NEW:                                                   // ԭWPT�ļ�������
    Filename[8] = 'P'; Filename[9] = 'A'; Filename[10] = 'R'; // ����WPT�ļ�
    if(OpenFileWr(DiskBuf, Filename, pCluster, pDirAddr)!= OK)
      return DISK_RW_ERR;
    memset(DiskBuf, 0, 512);
    
    *ptr++ =(menu.current <<8)+ Versions;           // ���������汾�ż���ǰҳ
    *ptr++=Status;
    for(i=0;i<10;i++) *ptr++=menu.menu_index[i];
    for(i=0;i<33;i++) *ptr++=PopMenu1_Value[i];
    for(i=0;i<18;i++) *ptr++=PopMenu2_Value[i];
    for(i=0;i<17;i++) *ptr++=PopMenu3_Value[i];
    
    for(i=0; i<511; i++)  Sum += DiskBuf[i];  // ���������У���
    DiskBuf[511] = (~Sum)+ 1;
    if(ProgFileSec(DiskBuf, pCluster)!= OK) 
      return FILE_RW_ERR; // д������
    if(CloseFile(DiskBuf, 512, pCluster, pDirAddr)!= OK) 
      return FILE_RW_ERR;
    return OK;
    default:  return FILE_RW_ERR;
  }
}
/*******************************************************************************
Load_Parameter: ����֮ǰ�Ĺ�������                           Return: 0= Success
*******************************************************************************/
u8 Load_Param(void)    //����֮ǰ�Ĺ������� ��U��
{ 
  u8  Sum = 0, Filename[12], Versions = 0x06; 
  u16 i,rc;
  u16* ptr =(u16*)DiskBuf;
  
  u16 pCluster[3];
  u32 pDirAddr[1]; 
  
  Word2Hex(Filename, __Info(DEV_SN));
  Filename[8] = 'P'; Filename[9] = 'A'; Filename[10] = 'R'; Filename[11] = 0; 
  rc = OpenFileRd(DiskBuf, Filename, pCluster, pDirAddr);
  if(rc != OK) return FILE_RW_ERR;

  else
  {
    if(ReadFileSec(DiskBuf, pCluster)!= OK) return FILE_RW_ERR;
    if(Versions !=(*ptr & 0xFF)) return VER_ERR;  // �汾������
    for(i=0; i<512; ++i) Sum += DiskBuf[i];
    if(Sum != 0) return SUM_ERR;                  // У��ͳ�����
    
    menu.current =(*ptr++ >>8);                   // ����֮ǰ�� Current Title
    ptr++;
    for(i=0;i<10;i++) menu.menu_index[i]=*ptr++;
    for(i=0;i<33;i++) PopMenu1_Value[i] =*ptr++;
    for(i=0;i<18;i++) PopMenu2_Value[i] =*ptr++;
    for(i=0;i<17;i++) PopMenu3_Value[i] =*ptr++;
  
  return rc;
 }
}

/*******************************************************************************
Restore_OrigVal     �ָ���������
*******************************************************************************/
void Restore_OrigVal(void)
{
    u8 i;
    Rest_Flag = 1;
    BmpNum = PopMenu3_Value[SAVE_Bmp];
    menu.current = Oscillo;
    for(i=0;i<3;i++)  menu.menu_index[i]=0;
    for(i=0;i<33;i++) PopMenu1_Value[i] =PopMenu1_CValue[i];
    for(i=0;i<18;i++) PopMenu2_Value[i] =PopMenu2_CValue[i];
    for(i=0;i<17;i++) PopMenu3_Value[i] =PopMenu3_CValue[i];
}
/******************************** END OF FILE *********************************/

