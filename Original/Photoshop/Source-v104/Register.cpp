#include <stdio.h>
#include <stdlib.h>
#include <string.h>

unsigned long int encr(unsigned long int in)
{		printf("%d",in); srand(in^0xafbabafa);
					
				for(int i=0;i!=10;i++)
					{ in^=0x12577345; in^=(unsigned long int)rand();
					  in^=(unsigned long int)(rand()<<16);
					}

				return in;
		}

unsigned long int main(void)
{	printf("GENESIS REGISTRATION CODE GENERATOR (c)1997 Andrew Cross\n");
	printf("--------------------------------------------------------\n");
	printf("THIS CODE WILL ONLY BE VALID FOR GENESIS 1.00\n");
	printf("--------------------------------------------------------\n");
	printf("\n");
	printf("*******************************\n");
	printf("DO NOT DISTRIBUTE THIS PROGRAM.\n");
	printf("*******************************\n");
	printf("\n");

	printf("Enter Registration Code (FULL code) : ");
	char s[10];
	unsigned int ser,serialnum; scanf("%[A-Z]-%d-%d",s,&serialnum,&ser);
	serialnum^=0x26542276;
	if( strncmp("MAXREG",s,6)==0 ) {
		if( !( (serialnum>=200000000 && serialnum<300000000) ||
				(serialnum>=600000000 && serialnum<700000000) ) )
			printf("THIS IS NOT A VALID MAX SERIAL NUMBER!!! %u\n",serialnum);
	} else if( strncmp("PSREG",s,5)==0 ) {
		if( !( (serialnum>=400000000 && serialnum<500000000) ||
				(serialnum>=600000000 && serialnum<700000000) ) )
				printf("THIS IS NOT A VALID PS SERIAL NUMBER!!! %u\n",serialnum);
	} else printf("THIS IS NOT VALID: does not begin with PSREG or MAXREG\n");

	unsigned int ser2=encr(ser);

	printf("Registration  : %10.10u\n",ser);
	printf("Authorisation : %10.10u\n",ser2);

	return ser;
}
