#include "SerialCMD.h"

SerialCMD::SerialCMD(){}

void SerialCMD::begin(Stream &mySerial, const __FlashStringHelper *sPrompt, pfWelcome welcome){
	_serial = &mySerial;
	_prompt = sPrompt;

	welcome();
	println();
	unregister_all();
	initialized = true;
	prompt();
}

bool SerialCMD::testLong(const char *string, long min_value, long max_value, long *value){
	long p;
	p = strtol(string,NULL,0);
	if(p > max_value || p < min_value){
      printf(F("ERROR: Parameter out range. Must be between %ld "), min_value);
	  printf(F("and %ld.\r\n"), max_value);
      return(false);
    }
    *value = p;
	return(true);
}

bool SerialCMD::testBoolean(const char *string, bool *value){
	if(strcmp(string,"on") == 0){
		*value = true;
    }else if(strcmp(string,"off") == 0){
		*value = false;
	}else{
		println(F("ERROR: Parameter out range. Must be on|off."));
		return(false);		
	}
	return(true);
}

bool SerialCMD::testDate(char *string, byte *day, byte *month, int *year){
    bool value, error;
    byte d , m;
	int y;
    if (strlen(string) == 10 && string[2] =='/' && string[5] =='/' ) {
        y = atoi(&string[6]);
        string[5] = '\0';
        m=atoi(&string[3]);
        string[2]= '\0';
        d=atoi(&string[0]);
            
        if(m < 1 || m > 12){
            error = true;
        }else if( d < 1 || (d > 31 && (m == 1 || m == 3 || m == 5 || m == 7 || m == 8 || m == 10|| m == 12))){
            error = true;
        }else if(d < 1 || (d > 30 && (m == 4 || m == 6 ))){
            error = true;
        }else if( d < 1 || (d > 28 && m == 2)){
            error = true;
        }else{
            *day = d;
            *month = m;
            *year = y;
            error = false;
            value = true;
        }
        if(error) println(F("ERROR: out of range"));
        
    }else {
        println(F("ERROR: Format not correct"));
        value = false;
    }
    return(value);
}

bool SerialCMD::testTime(char *string, byte *hour, byte *minutes, byte *seconds){
    bool value, error;
    byte h , m , s;
    
    if (strlen(string) == 8 && string[2] ==':' && string[5] ==':' ) {
        s = atoi(&string[6]);
        string[5] = '\0';
        m=atoi(&string[3]);
        string[2]= '\0';
        h=atoi(&string[0]);
        
        if(h < 0 || h > 24){
			error = true;
		}else if( m < 0 || m > 60){
			error = true;
		}else if(s < 0 || s > 60){
			error = true;
		}else{
			*hour = h;
			*minutes = m;
			*seconds = s;
			error = false;
			value = true;
        }
        if(error) println(F("ERROR: out of range"));
    }else {
        println(F("ERROR: Format not correct"));
        value = false;
    }
    return(value);
}

//*********************************************************************************************************

bool SerialCMD::addCommand(program_t program, const __FlashStringHelper *string, const __FlashStringHelper *help_string){
	unsigned char i;
	if(strlen_P((PGM_P)string) > CMD_MAX_COMMANDS_LENGHT){
		println(F("ERROR: Command too long."));
		return false;
	}
	for (i = 0; i < CMD_MAX_COMMANDS; i++) {
		if (list[i].program != 0 || list[i].command_string != 0)
			continue;
		list[i].program = program;
		list[i].command_string = string;
		list[i].help_string = help_string;
		return true;
	}
	return false;
}

void SerialCMD::printCommands(){
	unsigned char i;
	unsigned char n;
	unsigned char c;
	println();
	for (i = 0; i < CMD_MAX_COMMANDS; i++) {
		if (list[i].program != 0 || list[i].command_string != 0) {
			print(list[i].command_string);
			n = strlen_P((PGM_P)list[i].command_string);
			for(c = 0;c < 2 + CMD_MAX_COMMANDS_LENGHT - n;c++) print(" ");
			println(list[i].help_string);
		}
	}
}

void SerialCMD::unregister_all(void){
	unsigned char i;

	for (i = 0; i < CMD_MAX_COMMANDS; i++) {
		list[i].program = 0;
		list[i].command_string = 0;
	}
}

void SerialCMD::print(const char * string){
	while (* string != '\0')
		writer(* string++);
}

void SerialCMD::print(const __FlashStringHelper *string){
	print_pm((const PROGMEM byte *)string);
}

void SerialCMD::println(void){
	print(F("\r\n"));
}

void SerialCMD::println(const char * string){
	print(string);	
	print(F("\r\n"));
}

void SerialCMD::println(const __FlashStringHelper *string){
	print_pm((const PROGMEM byte *)string);
	print(F("\r\n"));
}

void SerialCMD::printf(const char * fmt, ...){
	char string[60];
	
	va_list argl;
	va_start(argl, fmt);
	vsnprintf(string,sizeof(string), fmt, argl);
	print(string);
	va_end(argl);	
}

void SerialCMD::printf(const __FlashStringHelper * fmt, ...){
	char string[60];
	
	va_list argl;
	va_start(argl, (PGM_P)fmt);
	vsnprintf_P(string,sizeof(string), (PGM_P)fmt, argl);
	print(string);
	va_end(argl);	
}

void SerialCMD::print_pm(const char * string){
	uint8_t c;
	do {
		c = pgm_read_byte(string++);
		if (!c)
			break;
		writer(c);
	} while (1);
}

void SerialCMD::prompt(void){
	print(_prompt);
}

void SerialCMD::writer(char data){
  _serial->write(data);
}

int SerialCMD::reader(char * data){
  if (_serial->available()) {
    *data = _serial->read();
    return 1;
  }
  return 0;
}

int SerialCMD::parse(char * buf, char ** argv, unsigned short maxargs){
	int i = 0;
	int argc = 0;
	int length = strlen(buf) + 1; 
	char toggle = 0;

	argv[argc] = &buf[0];

	for (i = 0; i < length && argc < maxargs; i++) {
		switch (buf[i]) {
			case '\0':
				i = length;
				argc++;
				break;
			case ' ':
				if (toggle == 0) {
					buf[i] = '\0';
					argc++;
					argv[argc] = &buf[i + 1];
				}
				break;
		}
	}
	return argc;
}


void SerialCMD::task(void){
	unsigned int i = 0;
	int argc = 0;
	char rxchar = 0;
	char finished = 0;
	static unsigned short count = 0;

	if (!initialized)
		return;

	// Procesa cada uno de los caracteres recibidos...
	if (reader(&rxchar)) {
		switch (rxchar) {
			case CMD_ASCII_CR: 	// Cuando se pulsa "ENTER"...
				shellbuf[count] = '\0';
				println("");
				finished = 1;
				break;
			default:			// Cuando se pulsan las demás teclas...
				if (count < CMD_MAX_INPUT_LENGHT && rxchar >= 0x20 && rxchar < 0x7F) {
					shellbuf[count] = rxchar;
					writer(rxchar);
					count++;
				}
		}

		// Chequea si hay disponible algún comando en el buffer para procesar...
		if (finished) {
			argc = parse(shellbuf, argv_list, CMD_MAX_COMMANDS_ARGS);
			// Busqueda secuencial de comandos...
			for (i = 0; i < CMD_MAX_COMMANDS; i++) {
				if (list[i].program == 0)
					continue;
				// Si algún comando coincide con la lista...
				if (!strcmp_P(argv_list[0], (PGM_P)list[i].command_string)) {
					// Ejecuta la función correspondiente al comando...
					if(list[i].program(argc, argv_list)) println(F("OK"));
					finished = 0;
				}
			}
			// Si no ha encontrado comando...
			if (finished != 0 && count != 0) 
				println(F("ERROR: Command not found."));
			count = 0;
			println("");
			prompt();
		}
	}
}
