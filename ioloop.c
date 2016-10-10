#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>


/*-------------------------------------------------------*/
static struct termios initial_settings, new_settings;
static int peek_character = -1;
/*-------------------------------------------------------*/
void init_keyboard()
{
  tcgetattr( 0, &initial_settings );
  new_settings = initial_settings;
  new_settings.c_lflag &= ~ICANON;
  new_settings.c_lflag &= ~ECHO;
  new_settings.c_lflag &= ~ISIG;
  new_settings.c_cc[VMIN] = 1;
  new_settings.c_cc[VTIME] = 0;
  tcsetattr( 0, TCSANOW, &new_settings );
}

void close_keyboard()
{
  tcsetattr( 0, TCSANOW, &initial_settings );
}

int kbhit()
{
  char ch;
  int nread;
 
  if( peek_character != -1 )
    return( 1 );
  new_settings.c_cc[VMIN] = 0;
  tcsetattr( 0, TCSANOW, &new_settings );
  nread = read( 0, &ch, 1 );  
  new_settings.c_cc[VMIN] = 1;  
  tcsetattr( 0, TCSANOW, &new_settings );
  if( nread == 1 ) {
    peek_character = ch;
    return( 1 );
  }
  return( 0 );
}

int readch()
{
  char ch;
  init_keyboard();
  if( peek_character != -1 ) {
    ch = peek_character;
    peek_character = -1;
    return( ch );
  }
  read( 0, &ch, 1 );
  close_keyboard();
  return( ch );
}

int main()
{
    char c,ext;

     c=readch();  //       <---- getch()
    if(c!=27)    
        printf("Caracter: %c - ASCII: %d\n\n",c,c);
    else {
        ext=readch();
        printf("Caracteres: %d - %d\n\n",c,ext);
        ext=readch();
        printf("Caracteres: %d - %d\n\n",c,ext);
    }
    
}
