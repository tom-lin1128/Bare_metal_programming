#define MAX_LENGTH 64 //maximum string length

typedef enum control_char{
	back_space_1 = 8,
	back_space_2 = 127,
	line_feed = 10,
	carriage_return = 13,
 
	unknowns = -1,
	ENTER_line = 1001,
  	INPUT = 1000,
  
} Control;

void shell_s();
Control Parse(char);
void list_command(Control, char, char [], int *);
