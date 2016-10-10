#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/select.h>
#include <termios.h>

void set_terminal_mode(int want_key)
{
	static struct termios old_term, new_term;
	
	if (!want_key) {
		tcsetattr(STDIN_FILENO, TCSANOW, &old_term);
		return;
	}
 
	tcgetattr(STDIN_FILENO, &old_term);
	new_term = old_term;

  new_term.c_lflag &= ~ECHO;
  new_term.c_lflag &= ~ICANON;
  
/*
	new_term.c_oflag &= ~ONLCR;
  new_term.c_oflag &= ~OCRNL;
  new_term.c_oflag &= ~ONLRET; **/

//	new_term.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &new_term);
	
	return;
}

int DetectHitKBoard() {
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(0, &fds);
    return select(1, &fds, NULL, NULL, NULL);
}

int getCharacter()
{
    int r;
    unsigned char c;
    if ((r = read(0, &c, sizeof(c))) < 0) 
        return r;
    else
        return c;
}
int main(int argc, char *argv[])
{
		unsigned char C;
		char CmdLine[80];
		int i, Crypt;
		
	 	set_terminal_mode(1);

		i = 0;
		
		Crypt = 0;
		memset(CmdLine, 0, 80);
    while (C != 10) {
    	DetectHitKBoard();
	    C = getCharacter();
	    if (C == 27) {
	    	C = getCharacter();
	    	if (C == 91) {
		    	C = getCharacter();
	    		switch(C) {
	    			case 65:
				    	printf("Key Up\n"); 
				    break;
	    			case 66:  
				    	printf("Key Down\n"); 
				    break;
	    			case 67:  
	    				if (Crypt)
	    					Crypt = 0;
	    				else 
	    					Crypt = 1;  
				    	printf("^"); 
				    	fflush(stdout);
				    break;
	    			case 68:  
				    	while (i >= 0) {
					    	CmdLine[i] = 0;
				    		printf("\b \b");
					    	fflush(stdout); 
				    		i--;
				    	} 
				    break;
				    default:
				    	printf("%c\n", C); 
				    break;
	    		}
	    	} else
					printf("27 -> %c\n", C); 
	    }
	    else {
	    	if (C != 127) {
		    	CmdLine[i] = C;
		    	i++;
		    	if (Crypt)
		    		printf("*");
					else
		    		printf("%c", C);
		    	fflush(stdout); 
		    }
		    else {
		    	i--;
		    	CmdLine[i] = 0;
	    		printf("\b \b");
		    	fflush(stdout); 
		    }
	  	}
    }
    
    printf("CmdLine %s\n", CmdLine);
    
    set_terminal_mode(0);
    
}