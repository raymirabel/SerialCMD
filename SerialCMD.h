#ifndef SERIAL_CMD_H
#define SERIAL_CMD_H

#include <string.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdbool.h>


#ifdef ARDUINO
	#include <Arduino.h>
#else
	#include <avr/pgmspace.h>
#endif

#define CMD_MAX_COMMANDS		20		// Máximo numero de comandos permitidos
#define CMD_MAX_COMMANDS_LENGHT	10		// Longitud máxima de caracteres por cada comando
#define CMD_MAX_COMMANDS_ARGS	3		// Número máximo de argumentos por comando
#define CMD_MAX_INPUT_LENGHT	50		// Tamaño del buffer de entrada de comandos y argumentos

#define CMD_ASCII_LF			0x0A
#define CMD_ASCII_CR			0x0D

typedef void (*pfWelcome) (void);
typedef bool (*program_t) (int, char **);

struct command_entry {
	program_t program;
	const __FlashStringHelper *command_string;
	const __FlashStringHelper *help_string;
};

class SerialCMD{
    
public:
	SerialCMD();
	void begin(Stream &mySerial, const __FlashStringHelper *prompt, pfWelcome welcome);
	bool addCommand(program_t program, const __FlashStringHelper *string, const __FlashStringHelper *help_string);
	void unregister_all(void);
	void task(void);
	void printCommands(void);	
	void print(const char *string);
	void print(const __FlashStringHelper *string);
	void println(void);
	void println(const char *string);
	void println(const __FlashStringHelper *string);
	void printf(const char *fmt, ...);
	void printf(const __FlashStringHelper * fmt, ...);
	bool testLong(const char *string, long min_value, long max_value, long *value);
	bool testBoolean(const char *string, bool *value);
    bool testDate(char *string, byte *day, byte *month, int *year);
    bool testTime(char *string, byte *hour, byte *minutes, byte *seconds);
	
private:

	struct command_entry list[CMD_MAX_COMMANDS];
	char * argv_list[CMD_MAX_COMMANDS_ARGS];
	char shellbuf[CMD_MAX_INPUT_LENGHT];
	Stream* _serial;
	const __FlashStringHelper* _prompt;
	bool initialized = false;
	void prompt(void);
	void writer(char data);
	int reader(char * data);
	int parse(char * buf, char ** argv, unsigned short maxargs);
	void print_pm(const char * string);
};

#endif
