﻿/** ###################################################################
**     Filename  : Project.C
**     Project   : Project
**     Processor : MC9S08QE128CLH
**     Version   : Driver 01.11
**     Compiler  : CodeWarrior HCS08 C Compiler
**     Date/Time : 15/07/2013, 09:00 p.m.
**     Abstract  :
**         Main module.
**         This module contains user's application code.
**     Settings  :
**     Contents  :
**         No public methods
**
** ###################################################################*/
/* MODULE Project */


/* Including needed modules to compile this module/procedure */
#include "Cpu.h"
#include "Events.h"
#include "PWM1.h"
#include "PWM2.h"
#include "Bit1.h"
#include "Bit2.h"
#include "linea.h"
#include "bobinas.h"
#include "TI1.h"
#include "TI2.h"
/* Include shared modules, which are used for whole project */
#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"
int estado=0;
char error;
int i=0;
int j=0;
int k=0;
int CW = 0;
int CounterCW = 0;
unsigned char maxi=200;
unsigned char maxd=270;
unsigned char multiplo=1;
unsigned char multiplo2=3;
unsigned char contador=0;
unsigned char valori,valord,actual,anterior;
unsigned int acumulado, dutyi, dutyd,arestar;
bool lineanegra=FALSE;
bool primeramed=TRUE;
bool fueraneg=FALSE;
extern byte bit_medido;
byte primera_vez=1;
byte Contador_buffer=0;
byte Buffer[8];
byte flag;
byte Medicion1;
byte dato;
int count;
bool tienesquemedir=0;
byte contador_entrada;
byte E0,E1,error3, error4;
byte Recibiendo_dato;

/* User includes (#include below this line is not maintained by Processor Expert) */

void main(void)
{
  /* Write your local variable definition here */

  /*** Processor Expert internal initialization. DON'T REMOVE THIS CODE!!! ***/
  PE_low_level_init();
  /*** End of Processor Expert internal initialization.                    ***/

  /* Write your code here */
  PTBDD_PTBDD0=TRUE;
  PTBDD_PTBDD1=TRUE;  
  PTADD_PTADD2=TRUE;
  PTADD_PTADD3=TRUE;
  /* For example: for(;;) { } */
   for(;;){
        PTAD_PTAD2=FALSE;
        PTAD_PTAD3=FALSE;
        PTBD_PTBD1=FALSE;
        PTBD_PTBD0=FALSE;
        
      
   /*//////////////////////////////////////////////////////////
   //                                                        //
   //        Andar en linea recta  (Estado = 0)              //
   //                                                        //
   //                                                        //
   //////////////////////////////////////////////////////////*/
   
   
    if(estado==0){
    
                  error=PWM1_Enable();
                        error=PWM2_Enable();
                        error=PWM2_SetDutyUS(maxi);
                        error=PWM1_SetDutyUS(maxd);
                        Bit2_ClrVal();
                        Bit1_ClrVal();
    
                  
                        lineanegra=linea_GetVal();          
                  while(!lineanegra){
                                        lineanegra=linea_GetVal();
                                        error=bobinas_MeasureChan(1,0);
                                        while(error!=ERR_OK){error=bobinas_MeasureChan(1,0);}
                                        error=bobinas_MeasureChan(1,1); while(error!=ERR_OK){error=bobinas_MeasureChan(1,1);}
                                        error=bobinas_GetChanValue(0,&valori);  while(error!=ERR_OK){error=bobinas_GetChanValue(0,&valori);}
                                        error=bobinas_GetChanValue(1,&valord); while(error!=ERR_OK){error=bobinas_GetChanValue(1,&valord);}             
                                        if(valori>=valord){
                                                          actual=valori-valord;
                                                          actual=(unsigned char) actual;
                                                          if(primeramed){
                                                                         acumulado= actual;
                                                                         anterior=actual;
                                                                         primeramed=FALSE;
                                                                         
                                                          } else acumulado=(anterior+actual)/2;
                                                          anterior=actual;
                                                          if(acumulado<=75){                                                          
                                                          arestar=acumulado*multiplo;                                                         
                                                          dutyi=maxi+arestar;
                                                          error=PWM2_SetDutyUS(dutyi);
                                                          }
                                                          if(acumulado>75 && acumulado<100){                                                          
                                                          arestar=acumulado*multiplo2;                                                         
                                                          dutyi=maxi+arestar;
                                                          error=PWM2_SetDutyUS(dutyi);
                                                          }
                                                          
                                                          if(acumulado>=100) error=PWM2_SetDutyUS(600); 
                                                          error=PWM1_SetDutyUS(maxd);
                                                        
                                        } else {
                                                actual=valord-valori;
                                                actual=(unsigned char) actual;
                                                if(primeramed){
                                                               acumulado=actual;
                                                               anterior=actual;
                                                               primeramed=FALSE;
                                                               
                                                } else acumulado=(anterior+actual)/2;
                                                anterior=actual;
                                                if(acumulado<=75){                                                  
                                                arestar=acumulado*multiplo;
                                                dutyd=maxd+arestar;                                                
                                                error=PWM1_SetDutyUS(dutyd);
                                                }
                                                if(acumulado>75 && acumulado<90){                                                  
                                                arestar=acumulado*multiplo2;
                                                dutyd=maxd+arestar;                                                
                                                error=PWM1_SetDutyUS(dutyd);
                                                }
                                                
                                                if(acumulado>90) error=PWM1_SetDutyUS(600); 
                                                error=PWM2_SetDutyUS(maxi);
                                          
                                        }
                                        
                                        
                  

                  }
                  error=PWM1_Disable();
                      error=PWM2_Disable();
                      Bit1_ClrVal();
                      Bit2_ClrVal();
                  estado=3;
    }
    
    
    
   /*//////////////////////////////////////////////////////////
   //                                                        //
   //        Espera para Lectura de Linea (Estado= 3)        //
   //                                                        //
   //                                                        //
   //////////////////////////////////////////////////////////*/
   
   if(estado==3){ // recepción de instrucción
   
            do{
      
          error=bobinas_MeasureChan(0,2); while(error!=ERR_OK){error=bobinas_MeasureChan(0,2);}
          flag=bobinas_GetChanValue(2,&Medicion1); while(flag!=ERR_OK){flag=bobinas_GetChanValue(2,&Medicion1);}
      
         } while ((Medicion1>170) && (primera_vez==1));
          
          
  
  if(primera_vez==1){
      Buffer[0]=0;
      Contador_buffer++;
     (void)TI2_Enable();
     //(void)TI1_EnableEvent();
      primera_vez=0;
 }
 
 
  //RECIBIR Y PROCESAR INSTRUCCION
 
 while(Contador_buffer<=7){
  if(tienesquemedir==1) {
    if(Contador_buffer<=7) {
           error=bobinas_MeasureChan(0,2); while(error!=ERR_OK){error=bobinas_MeasureChan(0,2);}
          flag=bobinas_GetChanValue(2,&Medicion1); while(flag!=ERR_OK){flag=bobinas_GetChanValue(2,&Medicion1);}
      
            if (Medicion1<150) {
                Buffer[Contador_buffer]=0;
            }
            else /*if(Medicion1>0xbe){*/ Buffer[Contador_buffer]=1;  //}
            Contador_buffer++;
            }
            tienesquemedir=0;
  
  }
  
 };                

 tienesquemedir=0;
 
 if(Contador_buffer>=8) {
    
      (void)TI2_Disable();
      //(void)TI1_DisableEvent();
      primera_vez=1;
 
  //OR de los bytes del buffer

      dato=Buffer[0]<<7 | Buffer[1]<<6 |  Buffer[2]<<5 | Buffer[3]<<4 | Buffer[4]<<3 | Buffer[5]<<2 | Buffer[6]<<1 | Buffer[7];
   
         Contador_buffer=0;
         //estado=0; 
   //DETERMINAR INSTRUCCION
 
  switch(dato) {
    
  
  case 59:   //Avanzar carro hacia la derecha
        
          estado=2;
          break;
          
   
  case 41:   //Avanzar carro hacia la izquierda     

          estado=1;         
          break;
  
  case 78:  //Avanzar carro hacia adelante      

          estado=0;          
          break;        
           
  default: //continuar esperando por una instruccion
          estado=3;
          break;
  }
 }
 
  
 
   }        
   
   
   
   /*//////////////////////////////////////////////////////////
   //                                                        //
   //        Girar a la izquierda (Estado= 1)                //
   //                                                        //
   //                                                        //
   //////////////////////////////////////////////////////////*/
   
   
   if(estado==1){// estado Izqierda //
   
        PTAD_PTAD2=FALSE; 
        PTAD_PTAD3=FALSE;
        PTBD_PTBD1=FALSE;
        PTBD_PTBD0=FALSE;
        //Cpu_Delay100US(15000);
        for (i=0;i<4;i++){
            
            CW=1;
            CounterCW=0;
            if (CW==1){
                //PTDD=0xA0;
                
                PTAD_PTAD2=TRUE;
                PTAD_PTAD3=FALSE;
                PTBD_PTBD1=TRUE;
                PTBD_PTBD0=FALSE;
                CW=2;
                Cpu_Delay100US(100);

            }
            
            if (CW==2){
                //PTDD=0x60;
                PTAD_PTAD2=TRUE;
                PTAD_PTAD3=FALSE;
                PTBD_PTBD1=FALSE;
                PTBD_PTBD0=TRUE;
                CW=3;
                Cpu_Delay100US(100);
            }
            
            if (CW==3){
                //PTDD=0x50;
                PTAD_PTAD2=FALSE;
                PTAD_PTAD3=TRUE;
                PTBD_PTBD1=FALSE;
                PTBD_PTBD0=TRUE;
                CW=4;
                Cpu_Delay100US(100);
            }
            
            if (CW==4){
                //PTDD=0x90;
                PTAD_PTAD2=FALSE;
                PTAD_PTAD3=TRUE;
                PTBD_PTBD1=TRUE;
                PTBD_PTBD0=FALSE;
                CW=1;
                Cpu_Delay100US(100);
            }
         }
         i=0;
        
         
         Cpu_Delay100US(10000);

         
         for (j=0;j<8;j++){
         
         CW=0;
            CounterCW=1;
            if (CounterCW==1){
                //PTDD=0xA0;
                PTAD_PTAD2=TRUE;
                PTAD_PTAD3=FALSE;
                PTBD_PTBD1=TRUE;
                PTBD_PTBD0=FALSE;
                CounterCW=2;
                Cpu_Delay100US(100);
                               
            }
            
            if (CounterCW==2){
                //PTDD=0x90;
                PTAD_PTAD2=FALSE;
                PTAD_PTAD3=TRUE;
                PTBD_PTBD1=TRUE;
                PTBD_PTBD0=FALSE;
                CounterCW=3;
                Cpu_Delay100US(100);
            }
            
            if (CounterCW==3){
                //PTDD=0x50;
                PTAD_PTAD2=FALSE;
                PTAD_PTAD3=TRUE;
                PTBD_PTBD1=FALSE;
                PTBD_PTBD0=TRUE;
                CounterCW=4;
                Cpu_Delay100US(100);
            }
            
            if (CounterCW==4){
                //PTDD=0x60;
                PTAD_PTAD2=TRUE;
                PTAD_PTAD3=FALSE;
                PTBD_PTBD1=FALSE;
                PTBD_PTBD0=TRUE;
                CounterCW=1;
                Cpu_Delay100US(100);
            }
         
         }
         
         j=0;
         
         
         Cpu_Delay100US(10000);
         
         for (i=0;i<4;i++){
            
            CW=1;
            CounterCW=0;
            if (CW==1){
                //PTDD=0xA0;
                
                PTAD_PTAD2=TRUE;
                PTAD_PTAD3=FALSE;
                PTBD_PTBD1=TRUE;
                PTBD_PTBD0=FALSE;
                CW=2;
                Cpu_Delay100US(100);

            }
            
            if (CW==2){
                //PTDD=0x60;
                PTAD_PTAD2=TRUE;
                PTAD_PTAD3=FALSE;
                PTBD_PTBD1=FALSE;
                PTBD_PTBD0=TRUE;
                CW=3;
                Cpu_Delay100US(100);
            }
            
            if (CW==3){
                //PTDD=0x50;
                PTAD_PTAD2=FALSE;
                PTAD_PTAD3=TRUE;
                PTBD_PTBD1=FALSE;
                PTBD_PTBD0=TRUE;
                CW=4;
                Cpu_Delay100US(100);
            }
            
            if (CW==4){
                //PTDD=0x90;
                PTAD_PTAD2=FALSE;
                PTAD_PTAD3=TRUE;
                PTBD_PTBD1=TRUE;
                PTBD_PTBD0=FALSE;
                CW=1;
                Cpu_Delay100US(100);
            }
         }
         i=0;
         PTAD_PTAD2=FALSE;
        PTAD_PTAD3=FALSE;
        PTBD_PTBD1=FALSE;
        PTBD_PTBD0=FALSE;

    error=TI1_Disable();
    fueraneg=FALSE;
    error=TI1_Enable();
    error=PWM1_Disable();
    error=PWM2_Disable();      
      while(!fueraneg){                   
                                        error=PWM1_Enable();
                                            error=PWM2_Enable();
                                            Bit2_ClrVal();
                                            Bit1_ClrVal();
                                        error=bobinas_MeasureChan(1,0);
                                        while(error!=ERR_OK){error=bobinas_MeasureChan(1,0);}
                                        error=bobinas_MeasureChan(1,1); while(error!=ERR_OK){error=bobinas_MeasureChan(1,1);}
                                        error=bobinas_GetChanValue(0,&valori);  while(error!=ERR_OK){error=bobinas_GetChanValue(0,&valori);}
                                        error=bobinas_GetChanValue(1,&valord); while(error!=ERR_OK){error=bobinas_GetChanValue(1,&valord);}             
                                        if(valori>=valord){
                                                          actual=valori-valord;
                                                          actual=(unsigned char) actual;
                                                          if(primeramed){
                                                                         acumulado= actual;
                                                                         anterior=actual;
                                                                         primeramed=FALSE;
                                                                         
                                                          } else acumulado=(anterior+actual)/2;
                                                          anterior=actual;
                                                          if(acumulado<=75){                                                          
                                                          arestar=acumulado*multiplo;                                                         
                                                          dutyi=maxi+arestar;
                                                          error=PWM2_SetDutyUS(dutyi);
                                                          
                                                                error=PWM1_SetDutyUS(maxd);
                                                          }
                                                          if(acumulado>75 && acumulado<100){                                                          
                                                          arestar=acumulado*multiplo2;                                                         
                                                          dutyi=maxi+arestar;
                                                          error=PWM2_SetDutyUS(dutyi);
                                                          error=PWM1_SetDutyUS(maxd);
                                                          }
                                                          
                                                          if(acumulado>=100) error=PWM2_SetDutyUS(600); 
                                                          error=PWM1_SetDutyUS(maxd);
                                                          
                                                        
                                        } else {
                                                actual=valord-valori;
                                                actual=(unsigned char) actual;
                                                if(primeramed){
                                                               acumulado=actual;
                                                               anterior=actual;
                                                               primeramed=FALSE;
                                                               
                                                } else acumulado=(anterior+actual)/2;
                                                anterior=actual;
                                                if(acumulado<=75){                                                  
                                                arestar=acumulado*multiplo;
                                                dutyd=maxd+arestar;                                                
                                                error=PWM1_SetDutyUS(dutyd);
                                                error=PWM2_SetDutyUS(maxi);
                                                }
                                                if(acumulado>75 && acumulado<90){                                                  
                                                arestar=acumulado*multiplo2;
                                                dutyd=maxd+arestar;                                                
                                                error=PWM1_SetDutyUS(dutyd);
                                                error=PWM2_SetDutyUS(maxi);
                                                }
                                                
                                                if(acumulado>90) error=PWM1_SetDutyUS(600); 
                                                error=PWM2_SetDutyUS(maxi);
                                                
                                          
                                        }
                         
      }  
      error=TI1_Disable();

         
/*         error=PWM2_SetDutyUS(200);
               error=PWM1_SetDutyUS(900);
         PWM1_Enable();
         PWM2_Enable();
         Bit2_ClrVal();
             Bit1_ClrVal();*/ //derecha
         
         error=PWM2_SetDutyUS(700);
               error=PWM1_SetDutyUS(300);
         PWM1_Enable();
         PWM2_Enable();
         Bit2_SetVal();
             Bit1_ClrVal();
             
             Cpu_Delay100US(7000);
             
             do{
                error=bobinas_MeasureChan(1,0);while(error!=ERR_OK){error=bobinas_MeasureChan(1,0);}
            error=bobinas_MeasureChan(1,1);while(error!=ERR_OK){error=bobinas_MeasureChan(1,1);}
            error=bobinas_GetChanValue(0,&valori);  while(error!=ERR_OK){error=bobinas_GetChanValue(0,&valori);}
            error=bobinas_GetChanValue(1,&valord); while(error!=ERR_OK){error=bobinas_GetChanValue(1,&valord);}     
             }
             while( valord<50 || valori<50);


             //Cpu_Delay100US(7000);
             //CAMBIAR POR RESTA DE VAINAS!
             
    error=TI1_Disable();
    fueraneg=FALSE;
    error=TI1_Enable();
    error=PWM1_Disable();
    error=PWM2_Disable();      
      while(!fueraneg){                   
                                        error=PWM1_Enable();
                                            error=PWM2_Enable();
                                            Bit2_ClrVal();
                                            Bit1_ClrVal();
                                        error=bobinas_MeasureChan(1,0);
                                        while(error!=ERR_OK){error=bobinas_MeasureChan(1,0);}
                                        error=bobinas_MeasureChan(1,1); while(error!=ERR_OK){error=bobinas_MeasureChan(1,1);}
                                        error=bobinas_GetChanValue(0,&valori);  while(error!=ERR_OK){error=bobinas_GetChanValue(0,&valori);}
                                        error=bobinas_GetChanValue(1,&valord); while(error!=ERR_OK){error=bobinas_GetChanValue(1,&valord);}             
                                        if(valori>=valord){
                                                          actual=valori-valord;
                                                          actual=(unsigned char) actual;
                                                          if(primeramed){
                                                                         acumulado= actual;
                                                                         anterior=actual;
                                                                         primeramed=FALSE;
                                                                         
                                                          } else acumulado=(anterior+actual)/2;
                                                          anterior=actual;
                                                          if(acumulado<=75){                                                          
                                                          arestar=acumulado*multiplo;                                                         
                                                          dutyi=maxi+arestar;
                                                          error=PWM2_SetDutyUS(dutyi);
                                                          
                                                                error=PWM1_SetDutyUS(maxd);
                                                          }
                                                          if(acumulado>75 && acumulado<100){                                                          
                                                          arestar=acumulado*multiplo2;                                                         
                                                          dutyi=maxi+arestar;
                                                          error=PWM2_SetDutyUS(dutyi);
                                                          error=PWM1_SetDutyUS(maxd);
                                                          }
                                                          
                                                          if(acumulado>=100) error=PWM2_SetDutyUS(600); 
                                                          error=PWM1_SetDutyUS(maxd);
                                                          
                                                        
                                        } else {
                                                actual=valord-valori;
                                                actual=(unsigned char) actual;
                                                if(primeramed){
                                                               acumulado=actual;
                                                               anterior=actual;
                                                               primeramed=FALSE;
                                                               
                                                } else acumulado=(anterior+actual)/2;
                                                anterior=actual;
                                                if(acumulado<=75){                                                  
                                                arestar=acumulado*multiplo;
                                                dutyd=maxd+arestar;                                                
                                                error=PWM1_SetDutyUS(dutyd);
                                                error=PWM2_SetDutyUS(maxi);
                                                }
                                                if(acumulado>75 && acumulado<90){                                                  
                                                arestar=acumulado*multiplo2;
                                                dutyd=maxd+arestar;                                                
                                                error=PWM1_SetDutyUS(dutyd);
                                                error=PWM2_SetDutyUS(maxi);
                                                }
                                                
                                                if(acumulado>90) error=PWM1_SetDutyUS(600); 
                                                error=PWM2_SetDutyUS(maxi);
                                                
                                          
                                        }
                         
      }  
      error=TI1_Disable();
      /*
      fueraneg=FALSE;
    error=TI1_Enable();
    while(!fueraneg){
                  error=PWM1_Disable();
                      error=PWM2_Disable();
                      Bit1_ClrVal();
                      Bit2_ClrVal();
    }
        error=TI1_Disable();
        */
    
    estado=0;
    
   }
   
   /*//////////////////////////////////////////////////////////
   //                                                        //
   //        Girar hacia la derecha  (Estado = 2)            //
   //                                                        //
   //                                                        //
   //////////////////////////////////////////////////////////*/

   if(estado==2){// estado Dhaerecha //
   
        PTAD_PTAD2=FALSE; 
        PTAD_PTAD3=FALSE;
        PTBD_PTBD1=FALSE;
        PTBD_PTBD0=FALSE;
        //Cpu_Delay100US(15000);
        for (i=0;i<4;i++){
            
            CW=1;
            CounterCW=0;
            if (CW==1){
                //PTDD=0xA0;
                
                PTAD_PTAD2=TRUE;
                PTAD_PTAD3=FALSE;
                PTBD_PTBD1=TRUE;
                PTBD_PTBD0=FALSE;
                CW=2;
                Cpu_Delay100US(100);

            }
            
            if (CW==2){
                //PTDD=0x60;
                PTAD_PTAD2=TRUE;
                PTAD_PTAD3=FALSE;
                PTBD_PTBD1=FALSE;
                PTBD_PTBD0=TRUE;
                CW=3;
                Cpu_Delay100US(100);
            }
            
            if (CW==3){
                //PTDD=0x50;
                PTAD_PTAD2=FALSE;
                PTAD_PTAD3=TRUE;
                PTBD_PTBD1=FALSE;
                PTBD_PTBD0=TRUE;
                CW=4;
                Cpu_Delay100US(100);
            }
            
            if (CW==4){
                //PTDD=0x90;
                PTAD_PTAD2=FALSE;
                PTAD_PTAD3=TRUE;
                PTBD_PTBD1=TRUE;
                PTBD_PTBD0=FALSE;
                CW=1;
                Cpu_Delay100US(100);
            }
         }
         i=0;
        
         
         Cpu_Delay100US(10000);

         
         for (j=0;j<8;j++){
         
         CW=0;
            CounterCW=1;
            if (CounterCW==1){
                //PTDD=0xA0;
                PTAD_PTAD2=TRUE;
                PTAD_PTAD3=FALSE;
                PTBD_PTBD1=TRUE;
                PTBD_PTBD0=FALSE;
                CounterCW=2;
                Cpu_Delay100US(100);
                               
            }
            
            if (CounterCW==2){
                //PTDD=0x90;
                PTAD_PTAD2=FALSE;
                PTAD_PTAD3=TRUE;
                PTBD_PTBD1=TRUE;
                PTBD_PTBD0=FALSE;
                CounterCW=3;
                Cpu_Delay100US(100);
            }
            
            if (CounterCW==3){
                //PTDD=0x50;
                PTAD_PTAD2=FALSE;
                PTAD_PTAD3=TRUE;
                PTBD_PTBD1=FALSE;
                PTBD_PTBD0=TRUE;
                CounterCW=4;
                Cpu_Delay100US(100);
            }
            
            if (CounterCW==4){
                //PTDD=0x60;
                PTAD_PTAD2=TRUE;
                PTAD_PTAD3=FALSE;
                PTBD_PTBD1=FALSE;
                PTBD_PTBD0=TRUE;
                CounterCW=1;
                Cpu_Delay100US(100);
            }
         
         }
         
         j=0;
         
         
         Cpu_Delay100US(10000);
         
         for (i=0;i<4;i++){
            
            CW=1;
            CounterCW=0;
            if (CW==1){
                //PTDD=0xA0;
                
                PTAD_PTAD2=TRUE;
                PTAD_PTAD3=FALSE;
                PTBD_PTBD1=TRUE;
                PTBD_PTBD0=FALSE;
                CW=2;
                Cpu_Delay100US(100);

            }
            
            if (CW==2){
                //PTDD=0x60;
                PTAD_PTAD2=TRUE;
                PTAD_PTAD3=FALSE;
                PTBD_PTBD1=FALSE;
                PTBD_PTBD0=TRUE;
                CW=3;
                Cpu_Delay100US(100);
            }
            
            if (CW==3){
                //PTDD=0x50;
                PTAD_PTAD2=FALSE;
                PTAD_PTAD3=TRUE;
                PTBD_PTBD1=FALSE;
                PTBD_PTBD0=TRUE;
                CW=4;
                Cpu_Delay100US(100);
            }
            
            if (CW==4){
                //PTDD=0x90;
                PTAD_PTAD2=FALSE;
                PTAD_PTAD3=TRUE;
                PTBD_PTBD1=TRUE;
                PTBD_PTBD0=FALSE;
                CW=1;
                Cpu_Delay100US(100);
            }
         }
         i=0;
         PTAD_PTAD2=FALSE;
        PTAD_PTAD3=FALSE;
        PTBD_PTBD1=FALSE;
        PTBD_PTBD0=FALSE;

error=TI1_Disable();
    fueraneg=FALSE;
    error=TI1_Enable();
    error=PWM1_Disable();
    error=PWM2_Disable();      
      while(!fueraneg){                   
                                        error=PWM1_Enable();
                                            error=PWM2_Enable();
                                            Bit2_ClrVal();
                                            Bit1_ClrVal();
                                        error=bobinas_MeasureChan(1,0);
                                        while(error!=ERR_OK){error=bobinas_MeasureChan(1,0);}
                                        error=bobinas_MeasureChan(1,1); while(error!=ERR_OK){error=bobinas_MeasureChan(1,1);}
                                        error=bobinas_GetChanValue(0,&valori);  while(error!=ERR_OK){error=bobinas_GetChanValue(0,&valori);}
                                        error=bobinas_GetChanValue(1,&valord); while(error!=ERR_OK){error=bobinas_GetChanValue(1,&valord);}             
                                        if(valori>=valord){
                                                          actual=valori-valord;
                                                          actual=(unsigned char) actual;
                                                          if(primeramed){
                                                                         acumulado= actual;
                                                                         anterior=actual;
                                                                         primeramed=FALSE;
                                                                         
                                                          } else acumulado=(anterior+actual)/2;
                                                          anterior=actual;
                                                          if(acumulado<=75){                                                          
                                                          arestar=acumulado*multiplo;                                                         
                                                          dutyi=maxi+arestar;
                                                          error=PWM2_SetDutyUS(dutyi);
                                                          
                                                                error=PWM1_SetDutyUS(maxd);
                                                          }
                                                          if(acumulado>75 && acumulado<100){                                                          
                                                          arestar=acumulado*multiplo2;                                                         
                                                          dutyi=maxi+arestar;
                                                          error=PWM2_SetDutyUS(dutyi);
                                                          error=PWM1_SetDutyUS(maxd);
                                                          }
                                                          
                                                          if(acumulado>=100) error=PWM2_SetDutyUS(600); 
                                                          error=PWM1_SetDutyUS(maxd);
                                                          
                                                        
                                        } else {
                                                actual=valord-valori;
                                                actual=(unsigned char) actual;
                                                if(primeramed){
                                                               acumulado=actual;
                                                               anterior=actual;
                                                               primeramed=FALSE;
                                                               
                                                } else acumulado=(anterior+actual)/2;
                                                anterior=actual;
                                                if(acumulado<=75){                                                  
                                                arestar=acumulado*multiplo;
                                                dutyd=maxd+arestar;                                                
                                                error=PWM1_SetDutyUS(dutyd);
                                                error=PWM2_SetDutyUS(maxi);
                                                }
                                                if(acumulado>75 && acumulado<90){                                                  
                                                arestar=acumulado*multiplo2;
                                                dutyd=maxd+arestar;                                                
                                                error=PWM1_SetDutyUS(dutyd);
                                                error=PWM2_SetDutyUS(maxi);
                                                }
                                                
                                                if(acumulado>90) error=PWM1_SetDutyUS(600); 
                                                error=PWM2_SetDutyUS(maxi);
                                                
                                          
                                        }
                         
      }  
      error=TI1_Disable();
      
      
         error=PWM2_SetDutyUS(200);
               error=PWM1_SetDutyUS(700);
         PWM1_Enable();
         PWM2_Enable();
         Bit2_ClrVal();
             Bit1_SetVal();
             Cpu_Delay100US(7000);
             
                          do{
                error=bobinas_MeasureChan(1,0);while(error!=ERR_OK){error=bobinas_MeasureChan(1,0);}
            error=bobinas_MeasureChan(1,1);while(error!=ERR_OK){error=bobinas_MeasureChan(1,1);}
            error=bobinas_GetChanValue(0,&valori);  while(error!=ERR_OK){error=bobinas_GetChanValue(0,&valori);}
            error=bobinas_GetChanValue(1,&valord); while(error!=ERR_OK){error=bobinas_GetChanValue(1,&valord);}     
             }
             while( valord<70 || valori<70);

                               
             


             //Cpu_Delay100US(7000);
             
             // Cambiar por Resta!!!
             
    error=TI1_Disable();
    fueraneg=FALSE;
    error=TI1_Enable();
    error=PWM1_Disable();
    error=PWM2_Disable();      
      while(!fueraneg){                   
                                        error=PWM1_Enable();
                                            error=PWM2_Enable();
                                            Bit2_ClrVal();
                                            Bit1_ClrVal();
                                        error=bobinas_MeasureChan(1,0);
                                        while(error!=ERR_OK){error=bobinas_MeasureChan(1,0);}
                                        error=bobinas_MeasureChan(1,1); while(error!=ERR_OK){error=bobinas_MeasureChan(1,1);}
                                        error=bobinas_GetChanValue(0,&valori);  while(error!=ERR_OK){error=bobinas_GetChanValue(0,&valori);}
                                        error=bobinas_GetChanValue(1,&valord); while(error!=ERR_OK){error=bobinas_GetChanValue(1,&valord);}             
                                        if(valori>=valord){
                                                          actual=valori-valord;
                                                          actual=(unsigned char) actual;
                                                          if(primeramed){
                                                                         acumulado= actual;
                                                                         anterior=actual;
                                                                         primeramed=FALSE;
                                                                         
                                                          } else acumulado=(anterior+actual)/2;
                                                          anterior=actual;
                                                          if(acumulado<=75){                                                          
                                                          arestar=acumulado*multiplo;                                                         
                                                          dutyi=maxi+arestar;
                                                          error=PWM2_SetDutyUS(dutyi);
                                                          
                                                                error=PWM1_SetDutyUS(maxd);
                                                          }
                                                          if(acumulado>75 && acumulado<100){                                                          
                                                          arestar=acumulado*multiplo2;                                                         
                                                          dutyi=maxi+arestar;
                                                          error=PWM2_SetDutyUS(dutyi);
                                                          error=PWM1_SetDutyUS(maxd);
                                                          }
                                                          
                                                          if(acumulado>=100) error=PWM2_SetDutyUS(600); 
                                                          error=PWM1_SetDutyUS(maxd);
                                                          
                                                        
                                        } else {
                                                actual=valord-valori;
                                                actual=(unsigned char) actual;
                                                if(primeramed){
                                                               acumulado=actual;
                                                               anterior=actual;
                                                               primeramed=FALSE;
                                                               
                                                } else acumulado=(anterior+actual)/2;
                                                anterior=actual;
                                                if(acumulado<=75){                                                  
                                                arestar=acumulado*multiplo;
                                                dutyd=maxd+arestar;                                                
                                                error=PWM1_SetDutyUS(dutyd);
                                                error=PWM2_SetDutyUS(maxi);
                                                }
                                                if(acumulado>75 && acumulado<90){                                                  
                                                arestar=acumulado*multiplo2;
                                                dutyd=maxd+arestar;                                                
                                                error=PWM1_SetDutyUS(dutyd);
                                                error=PWM2_SetDutyUS(maxi);
                                                }
                                                
                                                if(acumulado>90) error=PWM1_SetDutyUS(600); 
                                                error=PWM2_SetDutyUS(maxi);
                                                
                                          
                                        }
                         
      }  
      error=TI1_Disable();
      /*
      fueraneg=FALSE;
    error=TI1_Enable();
        while(!fueraneg){
                  error=PWM1_Disable();
                      error=PWM2_Disable();
                      Bit1_ClrVal();
                      Bit2_ClrVal();
    }
        error=TI1_Disable();
        */
    
    estado=0;
    
   }


   }
  /*** Don't write any code pass this line, or it will be deleted during code generation. ***/
  /*** Processor Expert end of main routine. DON'T MODIFY THIS CODE!!! ***/
  for(;;){}
  /*** Processor Expert end of main routine. DON'T WRITE CODE BELOW!!! ***/
} /*** End of main routine. DO NOT MODIFY THIS TEXT!!! ***/

/* END Project */
/*
** ###################################################################
**
**     This file was created by Processor Expert 3.07 [04.34]
**     for the Freescale HCS08 series of microcontrollers.
**
** ###################################################################
*/