#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
struct operation /* a struct to store all the 16 functions our assembly has */
{
	char *name;
	int opcode;
	int funct;
	int dest;
	int source;
};
				
struct symbol
{
	char *name;
	int address;
	char *attribute1;
	char *attribute2;
	struct symbol *next;
		
};

struct command
{
	char *comm;
	int line;
	int address1;
	int address2;
	int address3;
	char are2; /* the A R E of each word */
	char are3;
	struct 
	{
		unsigned int b1_0:2;
		unsigned int b3_2:2;
		unsigned int b7_4:4;
		unsigned int b11_8:4;
	}bit;	
	int word2;
	int word3;
	struct command *next;
};

struct guidance
{
	int line;
	int address;
	char type; /* type of guidance,'D' for data and 'S' for string */
	char *str;	/* .string array */
	int sLength; /* length of the string */
	int *dat;	/* .data array */
	int n; /*number of data in the array */
	struct guidance *next;
};

struct entry /*entry table */
{
	char *name;
	int address;
	struct entry *next;
};

struct external /* external table */
{
	char *name;
	int address;
	struct external *next;
};

#define MAX 100
#define NOREGISTERS 8 /*  number of registers in the assembly */
#define MAXLABEL 31
#define NSW 24 /* Number of saved words in our assembler */
enum flag {NO,YES};
void decToHex(FILE *,int);
void writeCommand(FILE *,int);

enum flag partTwo(FILE *,char *,char *,struct symbol **,struct symbol **,struct command **,struct command **,struct guidance **,struct guidance **
			,struct entry **,struct entry **,struct external **,struct external **,struct operation [],int *, int *);
			
enum flag partOne(FILE *,char *,char *,struct symbol **,struct symbol **,struct command **,struct command **,struct guidance **,
                  struct guidance **,struct operation [],int *,int []);	
                  
void openObj(char *,struct command **,struct guidance **,int ,int );         		
void openX(char *,struct external **);	
void openE(char *,struct entry **);


