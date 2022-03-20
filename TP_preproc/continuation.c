#include <stdio.h>
#define message_for(a, b) \
    printf(#a " and " #b ": We love (you!\n")

int main()
{
    message_for(Helge, Eivør);
    message_for(6, 123);
    message_for("Sexy women", "beautiful cars");

    return 0;
}
