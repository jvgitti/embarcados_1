/* ###################################################################
**     Filename    : main.c
**     Project     : Projeto 1 - Parte 2
**     Processor   : MKL25Z128VLK4
**     Version     : Driver 01.01
**     Compiler    : GNU C Compiler
**     Date/Time   : 2020-12-12, 12:15, # CodeGen: 0
**     Abstract    :
**         Main module.
**         This module contains user's application code.
**     Settings    :
**     Contents    :
**         No public methods
**
** ###################################################################*/
/*!
** @file main.c
** @version 01.01
** @brief Main do Projeto
** @mainpage EA076 - Projeto 1 : Infra-estrutura baseada no Protocolo MQTT (Codigo do Item 7)
** @authors Guilherme Bithencourt Martinelli (168923) e Joao Victor Gitti Aredes (170715) - Grupo 06       
**  @addtogroup main_module main module documentation
**  @{
*/         
/* MODULE main */


/* Including needed modules to compile this module/procedure */
#include "Cpu.h"
#include "Events.h"
#include "PC.h"
#include "ASerialLdd1.h"
#include "ESP.h"
#include "ASerialLdd2.h"
#include "LED_VERM.h"
#include "LEDpin1.h"
#include "BitIoLdd1.h"
#include "MCUC1.h"
#include "LED_VERDE.h"
#include "LEDpin2.h"
#include "BitIoLdd2.h"
#include "LED_AZUL.h"
#include "LEDpin3.h"
#include "BitIoLdd3.h"
#include "UTIL1.h"
/* Including shared modules, which are used for whole project */
#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"

/* User includes (#include below this line is not maintained by Processor Expert) */

	/*!
	** @brief buffer de comando a ser escrito
	*/
	volatile uint8_t cmd[100] = {0}; 
	
	/*!
	** @brief buffer da mensagem recebida
	*/
	volatile uint8_t rcv[100] = {0}; 
	
	/*!
	** @brief posicao no buffer rcv
	*/
	volatile uint8_t posit = 0; 
	
	/*!
	** @brief flag para sinalizar que a mensagem foi recebida
	*/
	volatile char message = 0; 

	/*!
	** @brief string do topico
	*/
	uint8_t top[50] = {0};
	
	/*!
	** @brief string do parametro do topico
	*/
	uint8_t par[10] = {0};
	
	/*!
	** @brief scanner 1 de string
	*/
	uint8_t scan = 0; 
	
	/*!
	** @brief scanner 2 de string
	*/
	uint8_t scan_2 = 0; 
	
	/*!
	** @brief Variavel para indicar se houve inscricao nos 3 topicos dos LEDs
	*/
	uint8_t count_sub = 3; 
	
	/*!
	** @brief Enviar comando para o ESP e recebe o caracter da string como argumento
	*/
	void SendCmd(uint8_t * s){ 
			while(*s){ //Permanece dentro do while até o caracter da string de comando não for 0 (fim da string)
				while(ESP_SendChar(*s)); //Envia o caracter para o ESP
				s++; //incrementa a posição em que o ponteiro está apontando
			}
	}
	
	/*!
	** @brief Enviar string para o PC e recebe o caracter da string como argumento - Mesma ideia da funcao SendCmd 
	*/
	void SendPC(uint8_t * s){
			while(*s){
				while(PC_SendChar(*s));	
				s++;
			}
	}
	
/*lint -save  -e970 Disable MISRA rule (6.3) checking. */
/*!
** @brief A main trata a string de comando inserida pelo usuario 
*/
int main(void)
/*lint -restore Enable MISRA rule (6.3) checking. */
{
  /* Write your local variable definition here */	

  /*** Processor Expert internal initialization. DON'T REMOVE THIS CODE!!! ***/
  PE_low_level_init();
  /*** End of Processor Expert internal initialization.                    ***/

  /* Write your code here */
  /* For example: for(;;) { } */
  strcpy(cmd, "CONNWIFI \"nome da rede\",\"senha da rede\"\r\n"); // Comando automático para conexão com a rede wifi
  SendCmd(cmd); //Envia o comando da conexão wifi para o ESP
  while(message == 0); //Certificar-se que o ESP recebeu a mensagem, pois o código sairá desse while quando a flag message = 1 (mensagem recebida)
  message = 0; //Zera a flag
  
  if (UTIL1_strcmp(rcv, "CONNECT WIFI\r\n")){ //Comparação da string de resposta do ESP com a CONNECT WIFI (0- VERDADEIRO, 1- FALSO)
	  SendPC("WIFI NAO CONECTA\r\n"); //Escreve no PC que não conectou no WIFI
  } else { //Caso as duas strings sejam iguais
  	  strcpy(cmd, "CONNMQTT \"137.135.83.217\",1883,\"client_ID\"\r\n"); // Comando automático para conexão com o servidor broker MQTT
  	  SendCmd(cmd); // Envia o comando da conexão broker MQTT para o ESP
  	  while(message == 0);
  	  message = 0;
  	  if(UTIL1_strcmp(rcv, "CONNECT MQTT\r\n")) { //Comparação da string de resposta do ESP com a CONNECT MQTT (0- VERDADEIRO, 1- FALSO)
  		  SendPC("MQTT NAO CONECTA\r\n"); //Escreve no PC que não conectou no MQTT
  	  } else {
  	  		  
  		  strcpy(cmd, "SUBSCRIBE \"EA076/168923/LEDR\"\r\n"); //Comando para assinar o tópico do LED VERMELHO
  		  SendCmd(cmd); //Envia o comando da assinatura de tópico para o ESP
  		  while(message == 0);
  		  message = 0;
  		  if(UTIL1_strcmp(rcv, "OK SUBSCRIBE\r\n")) { //Comparação da string de resposta do ESP com o OK SUBSCRIBE (0- VERDADEIRO, 1- FALSO)
  			  SendPC("ERRO NO SUBSCRIBE DO LED VERMELHO\r\n"); //Escreve no PC que ocorreu erro no subscribe
  			  count_sub--; //1 dos 3 tópicos para subscribe não aconteceu
  		  } 
  	  		 
  		  strcpy(cmd, "SUBSCRIBE \"EA076/168923/LEDG\"\r\n"); //Comando para assinar o tópico do LED VERDE
  	  	  SendCmd(cmd); //Envia o comando da assinatura de tópico para o ESP
  	  	  while(message == 0);
  	  	  message = 0;
  	  	  if(UTIL1_strcmp(rcv, "OK SUBSCRIBE\r\n")) { //Comparação da string de resposta do ESP com o OK SUBSCRIBE (0- VERDADEIRO, 1- FALSO)
  	  	  	  SendPC("ERRO NO SUBSCRIBE DO LED VERDE\r\n"); //Escreve no PC que ocorreu erro no subscribe
  	  	  	  count_sub--; //1 dos 3 tópicos para subscribe não aconteceu
  	  	  }
  	  		 
  	  	  strcpy(cmd, "SUBSCRIBE \"EA076/168923/LEDB\"\r\n"); //Comando para assinar o tópico do LED AZUL
  	  	  SendCmd(cmd); //Envia o comando da assinatura de tópico para o ESP
  	  	  while(message == 0);
  	  	  message = 0;
  	  	  if(UTIL1_strcmp(rcv, "OK SUBSCRIBE\r\n")) { //Comparação da string de resposta do ESP com o OK SUBSCRIBE (0- VERDADEIRO, 1- FALSO)
  	  	   	  SendPC("ERRO NO SUBSCRIBE DO LED AZUL\r\n"); //Escreve no PC que ocorreu erro no subscribe
  	  	   	  count_sub--; //1 dos 3 tópicos para subscribe não aconteceu
  	  	  }
  	  		 
  	  	  if (count_sub == 3) {
  	  		  SendPC("Concluido\r\n"); //Procedimento de SUBSCRIBE foi concluido com êxito
  	  	  }
  	  	  count_sub = 3;	
  	  }
  }
  
  for (;;) {
  	  if(message){ //Assim que receber uma mensagem, ele entra no if 
  		  message = 0;
  		  if(UTIL1_strFind(rcv, "MESSAGE") == -1) { //Verificar se recebeu uma string contendo MESSAGE
  			  SendPC(rcv); //Se não for uma string de MESSAGE, retorna para o PC justamente o que ele recebeu (provavelmente um erro)
  	  	  } else { //Caso seja uma MESSAGE, ele entre nesse else para separar o tópico de mensagem
  	  		  scan = 0;
  	  		  
  	  		  //Scan de Inicio de Tópico
  	  		  while(rcv[scan] != '['){ //Enquanto o caracter da posição do scan não for o [, o scan é incrementado
  	  			  scan++;
  	  		  }
  	  		  scan++; //O scan apontará para a posição logo depois do [ = Primeiro caracter do tópico
  	  		  scan_2 = 0;
  	  		  
  	  		  //Scan até o Fim do tópico
  	  		  while(rcv[scan] != ']'){ //Enquanto o caracter da posição do scan na mensagem recebida não for o ] :
  	  			  top[scan_2] = rcv[scan]; //Coloca na string de tópico o caracter da posição do scan
  	  			  scan++; //Incrementa os dois ponteiros scans
  	  			  scan_2++;
  	  		  }
  	  		  top[scan_2] = 0; //A String de tópico é finalizada com o 0
  	  		  
  	  		  //Scan de início do parametro do topico - Mesmo procedimento anterior
  	  		  while(rcv[scan] != '['){
  	  			  scan++;
  	  		  }
  	  		  scan++;
  	  		  scan_2 = 0;
  	  		  
  	  		//Scan até o fim do parametro do topico - Mesmo procedimento anterior
  	  		  while(rcv[scan] != ']'){
  	  			  par[scan_2] = rcv[scan];//Coloca na string de parametro do tópico o caracter da posição do scan
  	  			  scan++;
  	  			  scan_2++;
  	  		  }
  	  		  par[scan_2] = 0; //A String de parametro do tópico é finalizada com o 0
  		  
  	  		  if(UTIL1_strcmp(top, "EA076/168923/LEDR") == 0){ //Se o tópico extraido da mensagem recebida for sobre o LED VERMELHO
  	  			  if(UTIL1_strcmp(par, "ON")==0){ //Se o parametro extraido da mensagem recebida for ON
  	  				  LED_VERM_On(); // Liga o LED vermelho
  	  				  strcpy(cmd, "PUBLISH \"EA076/168923/reply\",\"LEDR ON\"\r\n"); //Comando de publicação de resposta ao comando de acender o LED VERMELHO
  	  				  SendCmd(cmd); // Envia o comando para o ESP publicar
  	  				  while(message == 0); 
  	  				  message = 0;
  	  			  } else { //Caso o parametro não seja ON (i.e. seja OFF)
  	  				  LED_VERM_Off(); //Desliga o LED VERMELHO
  	  				  strcpy(cmd, "PUBLISH \"EA076/168923/reply\",\"LEDR OFF\"\r\n"); //Comando de publicação de resposta ao comando de apagar o LED VERMELHO
  	  				  SendCmd(cmd); // Envia o comando para o ESP publicar
  	  				  while(message == 0);
  	  				  message = 0;
  	  			  }
  	  		  }
  	  		  
  		  if(UTIL1_strcmp(top, "EA076/168923/LEDG") == 0){ //Se o tópico extraido da mensagem recebida for sobre o VERDE
  			  if(UTIL1_strcmp(par, "ON")==0){ // Mesma lógica para os demais LEDs
  				  LED_VERDE_On();
  				  strcpy(cmd, "PUBLISH \"EA076/168923/reply\",\"LEDG ON\"\r\n");
  				  SendCmd(cmd);
  				  while(message == 0);
  				  message = 0;
  			  } else {
  				  LED_VERDE_Off();
  				  strcpy(cmd, "PUBLISH \"EA076/168923/reply\",\"LEDG OFF\"\r\n");
  				  SendCmd(cmd);
  				  while(message == 0);
  				  message = 0;
  			  }
  		  }
  		  if(UTIL1_strcmp(top, "EA076/168923/LEDB") == 0){ //Se o tópico extraido da mensagem recebida for sobre o LED AZUL
  			  if(UTIL1_strcmp(par, "ON")==0){ // Mesma lógica para os demais LEDs
  				  LED_AZUL_On();
  				  strcpy(cmd, "PUBLISH \"EA076/168923/reply\",\"LEDB ON\"\r\n");
  				  SendCmd(cmd);
  				  while(message == 0);
  				  message = 0;
  			  } else {
  				  LED_AZUL_Off();
  				  strcpy(cmd, "PUBLISH \"EA076/168923/reply\",\"LEDB OFF\"\r\n");
  				  SendCmd(cmd);
  				  while(message == 0);
  				  message = 0;
  			  }
  		  }
  	  	  }
  	  }
  }
  
  /*** Don't write any code pass this line, or it will be deleted during code generation. ***/
  /*** RTOS startup code. Macro PEX_RTOS_START is defined by the RTOS component. DON'T MODIFY THIS CODE!!! ***/
  #ifdef PEX_RTOS_START
    PEX_RTOS_START();                  /* Startup of the selected RTOS. Macro is defined by the RTOS component. */
  #endif
  /*** End of RTOS startup code.  ***/
  /*** Processor Expert end of main routine. DON'T MODIFY THIS CODE!!! ***/
  for(;;){}
  /*** Processor Expert end of main routine. DON'T WRITE CODE BELOW!!! ***/
} /*** End of main routine. DO NOT MODIFY THIS TEXT!!! ***/

/* END main */
/*!
** @}
*/
/*
** ###################################################################
**
**     This file was created by Processor Expert 10.3 [05.09]
**     for the Freescale Kinetis series of microcontrollers.
**
** ###################################################################
*/
