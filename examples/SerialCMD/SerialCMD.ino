/**************************************************************************
  @FILE:         SerialCMD.ino
  @AUTHOR:       Raymundo Alfonso
  @COMPANY:      Ray Ingeniería Electronica, S.L.
  @DESCRIPTION:  Ejemplo básico de uso de la libreria SerialCMD

  @LICENCE DETAILS:
  Este sketch está basada en software libre. Tu puedes redistribuir
  y/o modificar esta sketch bajo los términos de licencia GNU.

  Esta programa se distribuye con la esperanza de que sea útil,
  pero SIN NINGUNA GARANTÍA, incluso sin la garantía implícita de
  COMERCIALIZACIÓN O PARA UN PROPÓSITO PARTICULAR.
  Consulte los términos de licencia GNU para más detalles:

  http://www.gnu.org/licenses/gpl-3.0.txt

  This sketch is based on free software. You can redistribute
  and/or modify this library under the terms of the GNU license.

  This software is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY, even without the implied warranty of
  MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU license terms for more details:

  http://www.gnu.org/licenses/gpl-3.0.txt

  @VERSIONS:
  08-01-2019 - v1.00 : Primera versión. Incluir comentarios acerca de la nueva versión

**************************************************************************/

#include "SerialCMD.h"    // Libreria para implementación y utilización de comandos personalizados

// Istancia de comandos personalizados
SerialCMD cmd;      


/*******************************************************************************
  VARIABLES GLOBALES
*******************************************************************************/
long testNumber = 5;
bool testBoolean = false;


/*******************************************************************************
  SETUP (solo se ejecuta una vez)
*******************************************************************************/
void setup()
{
  // Inicializamos puerto serie
  Serial.begin(9600);

  // Inicializamos la shell personalizada
  cmd.begin(Serial, F("test>"), welcome);

  // Añadimos lo comandos con el puntero a la función, el nombre del comando y una pequeña descripción o ayuda
  cmd.addCommand(command_test,     F("test"),     F("Test de comandos y argumentos."));
  cmd.addCommand(command_number,   F("number"),   F("[-20...100000] Test de numeros"));
  cmd.addCommand(command_boolean,  F("boolean"),  F("[on|off] Test de boleanos"));
  cmd.addCommand(command_help,     F("?"),        F("Ayuda"));
}


/*******************************************************************************
  LOOP (se ejecuta continuamente)
*******************************************************************************/
void loop()
{
  // Siempre debe ser llamado para procesar las entradas de los usuarios
  cmd.task();
}



/*******************************************************************************
  FUNCIONES IMPLEMENTADAS
*******************************************************************************/
// Comprueba si el valor introducido se encuentra dentro del rango
int command_number(int argc, char** argv){
  bool ok = false;
  
  if(argc == 1) ok = true;
  if(argc >= 2){

    if(cmd.testStr(argv[1], -20, 100000, &testNumber)) ok = true;
  }  
  if(ok) cmd.printf(F("stime = %ld m\r\n"),testNumber);
  return(ok);
}

// 
bool command_boolean(int argc, char** argv){
  bool ok = false;
  
  if(argc == 1) ok = true;
  if(argc >= 2){

    if(cmd.testStr(argv[1],&testBoolean)) ok = true;
  }  
  if(ok){
    if(testBoolean)
      cmd.println(F("debug = on"));
    else
      cmd.println(F("debug = off"));
  }
  return(ok);
}

// Mustra una lista de todos los comandos disponibles
bool command_help(int argc, char** argv){
  cmd.println(F("Command list:"));
  cmd.printCommands();
}



bool command_test(int argc, char** argv){
  int i;

  cmd.println("-----------------------------------------------");
  cmd.println("SHELL DEBUG / TEST UTILITY");
  cmd.println("-----------------------------------------------");
  cmd.println("");
  cmd.printf("Received %d arguments for test command\r\n",argc);


  for(i=0; i<argc; i++){
    // Print formatted text to terminal
    cmd.printf(F("%d - \"%s\" [len:%d]\r\n"), i, argv[i], strlen(argv[i]) );
  }
  return(true);
}

// Mensaje de bienvenida
void welcome(void){
  cmd.println(F("Ray Ingenieria Electronica, S.L."));
  cmd.println(F("Test program"));
  cmd.println(F("Press '?' for help")); 
}
