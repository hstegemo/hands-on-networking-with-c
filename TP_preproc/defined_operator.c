#include <stdio.h>
char *MESSAGE = "You don't wisk!";  // this works

#if !defined (MESSAGE)
    #define MESSAGE "You wish!"
#endif

int main(void){
    //char *MESSAGE = "You don't say?"; // Error, but works if it's before the define
    printf("Here is the message: %s\n", MESSAGE);

    return 0;
}
