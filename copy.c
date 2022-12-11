#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
struct 
{
	char *name;
	int opcode;
	int funct;
	int dest;
	int source;
}operation[]={{"mov",0,0,0,0},{"cmp",1,0,0,0},{"add",2,10,0,0},{"sub",2,11,0,0},{"lea",4,0,0,0},{"clr",5,10,0,0}
				,{"not",5,11,0,0},{"inc",5,12,0,0},{"dec",5,13,0,0},{"jmp",9,10,0,0},{"bne",9,11,0,0},{"jsr",9,12,0,0}
				,{"red",12,0,0,0},{"prn",13,0,0,0},{"rts",14,0,0,0},{"stop",15,0,0,0}};
				
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
	char are2;
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

struct entry
{
	char *name;
	int address;
	struct entry *next;
};

struct external
{
	char *name;
	int address;
	struct external *next;
};

#define MAX 100
#define NSW 24 /* Number of saved words in our assembler */
enum flag {NO,YES};
void decToHex(FILE *,int ,char *);
void writeCommand(FILE *,int);

int main (int argc, char *argv[])
{
	FILE *fd;
	FILE *fdOb,*fdE,*fdX;
	enum flag sFlag=NO;
	enum flag err=NO;
	enum flag branch=NO;
	enum flag label_in_use=NO;
	enum flag found=NO;
	enum flag emptyLine=NO;
	struct symbol *sHead,*sNode,*sTail;
	struct command *cHead,*cNode,*cTail;
	struct guidance *gHead,*gNode,*gTail;
	struct external *xHead,*xNode,*xTail;
	struct entry *eHead,*eNode,*eTail;
	int IC=100,ICF;
	int DC=0;
	int line =0;
	int checkArg=0;
	int i,k,l,f;
	char minus=0;
	int word;
	int j=0;
	int firstHex,secondHex,thirdHex;
	char hex2[3];	/*to write the command in hexadecimal*/
	char hex3[3];	/*to write the command in hexadecimal*/
	char hex[3];
	char *s,*sRead,*sOb,*sE,*sX; /*file name*/
	int countLabel=0; /*count label length*/
	int countS=0; 	/* to count the length of the string in .string guidance */
	int countComma=0; /* to count commas in a data command,the number of numbers are countComma+1 */
	char buffer[MAX];
	char *save[NSW]={"r0","r1","r2","r3","r4","r5","r6","r7","mov","cmp","add","sub","lea","clr","not"
					,"inc ","dec","jmp","bne","jsr","red","prn","rts","stop"};
					
	sHead=sNode=sTail=NULL;
	cHead=cNode=cTail=NULL;
	gHead=gNode=gTail=NULL;
	xHead=xNode=xTail=NULL;
	eHead=eNode=eTail=NULL;				
	
	for(i=1;i<=argc-1;i++)
	{
		s=malloc(sizeof(*(argv+i)));
		sRead=malloc(sizeof(s)+3);
		sOb=malloc(sizeof(s)+3);
		sE=malloc(sizeof(s)+4);
		sX=malloc(sizeof(s)+4);
		strcpy(s,*(argv+i));
		strcpy(sRead,*(argv+i));
		strcpy(sOb,*(argv+i));
		strcpy(sE,*(argv+i));
		strcpy(sX,*(argv+i));
		
		strcat(sRead,".as");
		if(!(fd=fopen(sRead,"r")))
		{
			fprintf(stderr,"\ncannot open file\n");
			exit(0);
		}

		while(fgets(buffer,MAX+1,fd)!=NULL)/* 2 */
		{		
			line++;
			while (isspace(buffer[j]))
				j++;	
			if(buffer[j]=='\0' || buffer[j]==';')
				emptyLine=YES;	
				
			if(strchr(buffer,':')!=NULL && !emptyLine)	/* 3 */	
			{
				while(buffer[j+countLabel]!=':')
					countLabel++;		
				for(k=0;k<NSW;k++) /* check if label is not a saved word */
				{
				
					if((k<8) && (countLabel==2) && ((strncmp(buffer+j,save[k],2))==0))
					{
						printf("label in line %d uses a register name\n",line);
						err=YES;
						break;
					}	
					else if((k>=8) && (k!=NSW-1) && (countLabel==3) && (strncmp(buffer+j,save[k],3)==0))
					{
						printf("label in line %d use a command name\n",line);
						err=YES;
						break;
					}
					else if ((k==NSW-1) && (countLabel==4)&& (strncmp(buffer+j,save[k],4)==0))
					{
						printf("label in line %d use a command name\n",line);
						err=YES;
						break;
					}
				}
				if(k==NSW) /*means that the lable is not a safe word */ /* 3 */
				{
					sNode=sHead;
					while(sNode!=NULL)
					{
						if(strncmp((*sNode).name,buffer+j,countLabel)==0)
						{
							printf("line %d: label already in use\n",line);
							label_in_use=YES;
							break;
						}
						sNode=(*sNode).next;
					}
					
					if(label_in_use==NO)
					{
						sNode=(struct symbol*)malloc(sizeof(struct symbol));
						(*sNode).name=malloc(countLabel);
						strncpy((*sNode).name,buffer+j,countLabel);
						(*sNode).address=IC;
						(*sNode).next=NULL;
						if(sTail==NULL)
							sHead=sTail=sNode;
						else
						{	
							(*sTail).next=sNode;
							sTail=sNode;
						}	
						sFlag=YES;		/* 4 */
						
					}
				}		
							
			}		/* end 3 */
			else /*there is no label*/ 
				sFlag=NO;
			if(sFlag)
			{
				while(!isspace(buffer[j])) /* skip the lable */
					j++;
				while(isspace(buffer[j])) /* skip space */
					j++;	
			}		
					
			if(buffer[j]=='.' && !emptyLine)		/* 5 */
			{
				j++;
				gNode=(struct guidance *)malloc(sizeof(struct guidance));	
				(*gNode).address=DC;	
				(*gNode).line=line;	
				(*gNode).next=NULL;
				if(gTail==NULL)
					gHead=gTail=gNode;
				else
				{
					(*gTail).next=gNode;
					gTail=gNode;
				}
				if(strncmp(buffer+j,"string",6)==0)
				{
					j+=6;	
					if(!isspace(buffer[j]))
					{	
						printf("line %d: not a valid command\n",line);
						err=YES;
						break;
					}
					if(sFlag)
					{
						(*sTail).attribute1=malloc(6);
						(*sTail).attribute1="string"; /* 6 */
						(*sTail).address=DC;
					}
					while(isspace(buffer[j]))
						j++;
						
						
					if(buffer[j]!='\"')
					{
						err=YES;
						printf("missing \" in line %d\n",line);
					}
					else
					{	
						j++; /*skip the " */
						countS=0;
						while(buffer[j+countS]!='\"')
							countS++;
						(*gTail).str=malloc(countS);
						(*gTail).sLength=countS;
						(*gTail).type='S';
						for(l=0;l<countS;l++)
						{
							(*gTail).str[l]=buffer[j];
							j++;
						}	
						(*gTail).str[l]='\0';
						if(buffer[j]=='\0')
						{	
							err=YES;
							printf("missing \" in line %d\n",line);
						}
						DC+=countS+1;
					}
				}	
				else if(strncmp(buffer+j,"data",4)==0)
				{
					j+=4;
					if(!isspace(buffer[j]))
					{	
						printf("line %d: not a valid command\n",line);
						err=YES;
						break;
					}
					if(sFlag)
					{
						(*sTail).attribute1=malloc(4);
						(*sTail).attribute1="data"; /* 6 */
						(*sTail).address=DC;
					}
					while(isspace(buffer[j]))
						j++;
					countComma=0;
					for(l=0;buffer[j+l]!='\0';l++)
					{
						if(buffer[j+l]==',')
							countComma++;
					}	
					countComma++;
					(*gTail).n=countComma;
					(*gTail).type='D';
					(*gTail).dat=(int*)malloc(countComma*sizeof(int));
					
					for(f=0;f<countComma;f++)
					{
						if(buffer[j]=='-')
						{
							minus=1;
							j++;
						}	
						else if(buffer[j]=='+')
						{
							minus=0;
							j++;
						}	
						if(isdigit(buffer[j]))
						{
							(*gTail).dat[f]=atoi(buffer+j);
							if(minus)
								(*gTail).dat[f]*=(-1);
						}	
						else
						{
							printf("line %d: data including none number value\n",line);
							err=YES;
							break;
						}	
						while(isdigit(buffer[j]))
							j++;
						while(isspace(buffer[j]) && buffer[j]!='\0')
							j++;	
						if(buffer[j]!='\0' && buffer[j]!=',')
						{
							printf("data in line %d including none number value\n",line);
							err=YES;
							break;
						}
						if(buffer[j]!='\0')
							j++; /* skip the comma */
						while(isspace(buffer[j]) && buffer[j]!='\0')
							j++;
						minus=0;
						
						
					}
					
					DC+=countComma;	
				}
				else if(strncmp(buffer+j,"entry",5)==0) /* 9 */ 
				{
					j+=5;

					if(!isspace(buffer[j]))
					{	
						printf("line %d: !not a valid command\n",line);
						err=YES;
						break;
					}
				}
				else if(strncmp(buffer+j,"extern",6)==0) /* 10 */
				{
					j+=6;
					if(!isspace(buffer[j]))
					{	
						printf("line %d: not a valid command\n",line);
						err=YES;
						break;
					}
					countLabel=0;
					while(isspace(buffer[j]))
						j++;
					while(!isspace(buffer[j+countLabel]))
						countLabel++;
							
					for(k=0;k<NSW;k++) /* check if label is not a saved word */
					{
						if((k<8) && (countLabel==2) && (strncmp(buffer+j,save[k],2)==0))
						{
							printf("label in line %d uses a register name\n",IC);
							err=YES;
							break;
						}	
						else if((k>=8) && (k!=NSW-1) && (countLabel==3) && (strncmp(buffer+j,save[k],3)==0))
						{
							printf("label in line %d use a command name\n",IC);
							err=YES;
							break;
						}
						else if ((k==NSW-1) && (countLabel==4)&& (strncmp(buffer+j,save[k],4)==0))
						{
							printf("label in line %d use a command name\n",line);
							err=YES;
							break;
						}	
					 }
					 if(k==NSW) /*means that the lable isn't a safe word */ /* 3 */
					 {
						while(sNode!=NULL)
						{
							if(strncmp((*sNode).name,buffer+j,countLabel)==0)
							{
								printf("Line %d: label already in use\n",line);
								label_in_use=YES;
								break;
							}
							sNode=(*sNode).next;
						}
					
						if(label_in_use==NO)
						{
							sNode=(struct symbol*)malloc(sizeof(struct symbol));
							(*sNode).name=malloc(countLabel);
							strncpy((*sNode).name,buffer+j,countLabel);
							(*sNode).address=IC;
							(*sNode).attribute1=malloc(8);
							(*sNode).attribute2=malloc(1);
							(*sNode).attribute1="external"; 
							(*sNode).attribute2="0";
							(*sNode).next=NULL;
							if(sTail==NULL)
							{
								sHead=sTail=sNode;
							}	
							else
							{	
								(*sTail).next=sNode;
								sTail=sNode;
							}	
							sFlag=YES;	
						}					
					 }		
				}
				else 
				{ 
					printf("error in line %d\n",line);
					err=YES;
					break;
				}				
			}	/* 7 */	
			else if(!emptyLine) /* the line is a command */ /* 11 */
			{
				if(sFlag)
				{
					(*sTail).attribute1=malloc(4);
					(*sTail).attribute1="code";
					(*sTail).address=IC;
				}	
				for(k=0;k<15;k++)
				{
					if((strncmp(buffer+j,operation[k].name,3)==0) && (isspace(buffer[j+3])))
					{
						cNode=(struct command *)malloc(sizeof(struct command));
						(*cNode).comm=operation[k].name;
						(*cNode).line=line;
						(*cNode).address1=IC;
						(*cNode).bit.b11_8=operation[k].opcode;
						(*cNode).bit.b7_4=operation[k].funct;
						(*cNode).next=NULL;
						if(cTail==NULL)
							cHead=cTail=cNode;
						else
						{
							(*cTail).next=cNode;
							cTail=cNode;
						}	
						
						j+=3;
						break;
					}	
				}	/* commands */
				
				/* commands */
				if((k==15) && (strncmp(buffer+j,operation[15].name,4)==0)&& isspace(buffer[j+4]) )
				{
					cNode=(struct command *)malloc(sizeof(struct command));
					(*cNode).comm=operation[15].name;
					(*cNode).line=line;
					(*cNode).address1=IC;
					(*cNode).bit.b11_8=operation[k].opcode;
					(*cNode).bit.b7_4=operation[k].funct;
					(*cNode).next=NULL;
					if(cTail==NULL)
						cHead=cTail=cNode;
					else
					{
						(*cTail).next=cNode;
						cTail=cNode;
					}	
					
					j+=4;
				}
				else if(k==15)
				{
					printf("line %d: command is not valid\n",line);
					err=YES;
					break;
				}
				while(isspace(buffer[j]))
					j++;
					
				if(k<4)
				{
					
					if(buffer[j]=='#')
					{
						j++;
						if(buffer[j]=='-')
						{
							j++;
							(*cTail).word2=atoi(buffer+j)*(-1);
						}	
						else if(buffer[j]=='+')
						{
							j++;
							(*cTail).word2=atoi(buffer+j);
						}
						else if(!isdigit(buffer[j]))
						{
							printf("line %d: expected a number \n",line);
							err=YES;
						}
						else
							(*cTail).word2=atoi(buffer+j);	
						(*cTail).bit.b3_2=0;
						(*cTail).are2='A';
						while (isdigit(buffer[j]))
							j++; /*skip the first word */
					}
					else 
					{
						while(buffer[j+checkArg]!=',')
							checkArg++;
						for(l=0;l<8;l++)
						{
							if((checkArg==2) && (strncmp(buffer+j,save[l],2)==0))
							{
								(*cTail).bit.b3_2=3;
								(*cTail).are2='A';
								j++;	/*skip the character 'r'*/

								(*cTail).word2=1<<atoi(buffer+j);
								j++; /*skip the first word */
								break;
							}
						}
						
						if(l==8) /* the first argument is a label,skip it */
						{
						    for(f=0;f<NSW;f++) /* check if argument is not a saved word */
							{
								if((f>=8) && (f!=NSW-1) && (checkArg==3) && (strncmp(buffer+j,save[f],3)==0))
								{
									printf("line %d: argument is a command name\n",line);
									err=YES;
									break;
								}
								else if ((f==NSW-1) && (checkArg==4)&& (strncmp(buffer+j,save[f],4)==0))
								{
									printf("line %d argument is a command name\n",line);
									err=YES;
									break;
								}
							}	
							(*cTail).bit.b3_2=1;
							while(buffer[j]!=',')
								j++;	
						}
						checkArg=0;		
					}	
					/*while (isspace(buffer[j])) 
						j++;*/
					if(buffer[j]!=',')
					{
						err=YES;
						printf("line %d: missing argument\n",line); /*move to the next line */
						break;
					}
					else
					{
						j++;
						while(isspace(buffer[j]))
							j++;
						if(buffer[j]=='#' && k!=1)
						{
							printf("line %d: argument can not be a number\n",line);
							err=YES;
						}	
						while(!isspace(buffer[j+checkArg]))
							checkArg++;
						for(l=0;l<8;l++)
						{
							if((checkArg==2) && (strncmp(buffer+j,save[l],2)==0))
							{
								(*cTail).bit.b1_0=3;
								(*cTail).are3='A';
								j++;	/*skip the character 'r'*/
								(*cTail).word3=1<<atoi(buffer+j);
								j++; /*skip the second word */
								break;
							}
						}
						if((l==8) && (k!=1)) /*means that the second word is a label, skip it */
						{
							(*cTail).bit.b1_0=1;
							while(!isspace(buffer[j]))
								j++;	
						}
						if((k==1) && (l==8))	/* the command is cmp and the second argument might be a number */
						{
							if(buffer[j]=='#')
							{
								j++;
								if(buffer[j]=='-')
								{
									j++;
									(*cTail).word3=atoi(buffer+j)*(-1);
								}
								else if (buffer[j]=='+')
								{
									j++;
									(*cTail).word3=atoi(buffer+j);
								}
								else
									(*cTail).word3=atoi(buffer+j);
								(*cTail).bit.b1_0=0;
								(*cTail).are3='A';
								while (isdigit(buffer[j]))
									j++; /*skip the second argument */
							}
						}	
						while(isspace(buffer[j]))
							j++;
						if(buffer[j]!='\0') 	/* there is additional characters after the command */
						{
							printf("line %d: additional characters after the command\n",line);
							err=YES;
						}	
						
						(*cTail).address2=IC+1;
						(*cTail).address3=IC+2;
						IC+=3;
					}		
				}
				if(k==4)
				{
					countLabel=0;
					while(!isspace(buffer[j+countLabel]) && buffer[j+countLabel]!=',')
						countLabel++;
					if(buffer[j]=='#')
					{
						printf("line %d: argument can not be a number\n",line);
						err=YES;
					}		
					if(countLabel==2)
					{
						for(i=0;i<8;i++)
							if(strncmp(buffer+j,save[i],2)==0)
							{
								printf("line %d: label can't be named as a register \n",line);
								err=YES;
							}
					}	
					(*cTail).bit.b3_2=1;
					j=j+countLabel;
					while(isspace(buffer[j]))
						j++;
						
					if(buffer[j]!=',')
					{
						err=YES;
						printf("line %d: missing argument\n",line); /*move to the next line */
						break;
					}
					j++; /* skip comma */
					checkArg=0;
					while(isspace(buffer[j]))
						j++;
					if(buffer[j]=='#')
					{
						printf("line %d: argument can not be a number\n",line);
						err=YES;
					}		
					while(!isspace(buffer[j+checkArg]))
						checkArg++;
					if(checkArg==2)
					{		
						for(l=0;l<8;l++)
						{
							if(strncmp(buffer+j,save[l],2)==0)
							{
								(*cTail).bit.b1_0=3;
								(*cTail).are3='A';
								j++;	/*skip the character 'r'*/
								(*cTail).word3=1<<atoi(buffer+j);
								j++; /*skip the second word */
								break;
							}
						}
						
					}
					if((l==8)||(checkArg!=2)) /* second argument is a label */
					{
						j+=checkArg;	
						(*cTail).bit.b1_0=1;
					}	
					
					
						
					while(isspace(buffer[j]))
						j++;
					if(buffer[j]!='\0') 	/* there is additional characters after the command */
					{
						printf("line %d: additional characters after the command\n",line);
						err=YES;
					}	
					(*cTail).address2=IC+1;
					(*cTail).address3=IC+2;
					IC=IC+3;
				}
				if(k>4 && k<9) /* one operand */
				{
					
					(*cTail).bit.b3_2=0;
					checkArg=0;
					(*cTail).are2='A';
					if(buffer[j]=='#')
					{
						printf("line %d: argument can not be a number\n",line);
						err=YES;
					}
					while(!isspace(buffer[j+checkArg]))
						checkArg++;
					if(checkArg==2)
					{	
						for(l=0;l<8;l++)
						{
						
							if(strncmp(buffer+j,save[l],2)==0)
							{
								
								(*cTail).bit.b1_0=3;
								j++;	/*skip the character 'r'*/
								(*cTail).word2=1<<atoi(buffer+j);
								j++; /*skip the second word */
								break;
							}
						}
						
					}
					if((l==8)|| checkArg!=2)  /* argument is a label */
					{
						for(f=0;f<NSW;f++) /* check if argument is not a saved word */
						{
							if((f>=8) && (f!=NSW-1) && (checkArg==3) && (strncmp(buffer+j,save[f],3)==0))
							{
								printf("line %d: argument is a command name\n",line);
								err=YES;
								break;
							}
							else if ((f==NSW-1) && (checkArg==4)&& (strncmp(buffer+j,save[f],4)==0))
							{
								printf("line %d: argument is a command name\n",line);
								err=YES;
								break;
							}
						}
						j+=checkArg;
						(*cTail).bit.b1_0=1;
						
					}	
					
					while(isspace(buffer[j]) && buffer[j]!='\0')
						j++;
						
					if(buffer[j]!='\0') 	/* there is additional characters after the command */
					{
						printf("line %d: additional characters after the command\n",line);
						err=YES;
					}
					
					(*cTail).address2=IC+1;
					(*cTail).address3=0;
					IC=IC+2;	
				}
				
				if(k>8 && k<12)
				{
					(*cTail).bit.b3_2=0;
					if(buffer[j]!='%')
					{
						printf("line %d: missing precentage symbol \n",line);
						err=YES;
						break;
					}	
					j++;
					if(buffer[j]=='#')
					{
						printf("line %d: argument can not be a number\n",line);
						err=YES;
					}
					(*cTail).bit.b1_0=2;
					countLabel=0;
					while(!isspace(buffer[j+countLabel])) 
						countLabel++;
					for(f=0;f<NSW;f++) /* check if label is not a saved word */
					{
				
						if((f<8) && (countLabel==2) && ((strncmp(buffer+j,save[f],2))==0))
						{
							printf("line %d:argument can not be a register\n",line);
							err=YES;
							break;
						}	
						else if((f>=8) && (f!=NSW-1) && (countLabel==3) && (strncmp(buffer+j,save[f],3)==0))
						{
							printf("line %d:argument can not be a command name\n",line);
							err=YES;
							break;
						}
						else if ((f==NSW-1) && (countLabel==4)&& (strncmp(buffer+j,save[f],4)==0))
						{
							printf("line %d: argument can not use a command name\n",line);
							err=YES;
							break;
						}
					}

					j+=countLabel; /*skip the label */
								
					while(isspace(buffer[j]))
						j++;
					if(buffer[j]!='\0') 	/* there is additional characters after the command */
					{
						printf("line %d: additional characters after the command\n",line);
						err=YES;
					}
					(*cTail).address2=IC+1;
					(*cTail).address3=0;
					IC+=2;		 		
				}
				if(k==12)
				{
					(*cTail).bit.b3_2=0;
					if(buffer[j]=='#')
					{
						printf("line %d: argument can not be a number\n",line);
						err=YES;
					}
					checkArg=0;
					while(!isspace(buffer[j+checkArg]))
						checkArg++;
					if(checkArg==2)
					{
						for(l=0;l<8;l++)
						{
							if(strncmp(buffer+j,save[l],2)==0)
							{
								(*cTail).bit.b1_0=3;
								(*cTail).are3='A';
								j++;	/*skip the character 'r'*/
								(*cTail).word3=1<<atoi(buffer+j);
								j++; /*skip the second word */
								break;
							}
						}	
					}
					if((l==8) || (checkArg!=2))	/* the argument is a label */
					{
						for(f=0;f<NSW;f++) /* check if argument is not a saved word */
						{
							if((f>=8) && (f!=NSW-1) && (checkArg==3) && (strncmp(buffer+j,save[f],3)==0))
							{
								printf("line %d: argument is a command name\n",line);
								err=YES;
								break;
							}
							else if ((f==NSW-1) && (checkArg==4)&& (strncmp(buffer+j,save[f],4)==0))
							{
								printf("line %d: argument is a command name\n",line);
								err=YES;
								break;
							}
						}
						(*cTail).bit.b1_0=1;
						while(!isspace(buffer[j])) /*skip the label */
							j++;
								
						while(isspace(buffer[j]))
							j++;
						if(buffer[j]!='\0') 	/* there is additional characters after the command */
						{
							printf("line %d: additional characters after the command\n",line);
							err=YES;
						}	
					}
					(*cTail).address2=IC+1;
					(*cTail).address3=0;
					IC+=2;	
				}
				if(k==13)
				{
					(*cTail).bit.b3_2=0;
					if(buffer[j]=='#')
					{
						j++;
						if(buffer[j]=='-')
						{
							j++;
							(*cTail).word2=atoi(buffer+j)*(-1);
						}
						else if (buffer[j]=='+')
						{
							j++;
							(*cTail).word2=atoi(buffer+j);
						}
						else
							(*cTail).word2=atoi(buffer+j);
						(*cTail).bit.b1_0=0;
						(*cTail).are3='A';
						
						while(isdigit(buffer[j]))
							j++;
					}
					else	/* argument is a label or a register */
					{
						checkArg=0;
						while(!isspace(buffer[j+checkArg]))
							checkArg++;
						if(checkArg==2)
						{
							for(l=0;l<8;l++)
							{
								if(strncmp(buffer+j,save[l],2)==0)
								{
									(*cTail).bit.b1_0=3;
									(*cTail).are3='A';
									j++;	/*skip the character 'r'*/
									(*cTail).word3=1<<atoi(buffer+j);
									j++; /*skip the second word */
									break;
								}
							}	
						}	
						if((l==8)|| (checkArg!=2))	/* the argument is a label */
						{
							for(f=0;f<NSW;f++) /* check if argument is not a saved word */
							{
								if((f>=8) && (f!=NSW-1) && (checkArg==3) && (strncmp(buffer+j,save[f],3)==0))
								{
									printf("line %d: argument is a command name\n",line);
									err=YES;
									break;
								}
								else if ((f==NSW-1) && (checkArg==4)&& (strncmp(buffer+j,save[f],4)==0))
								{
									printf("line %d: argument is a command name\n",line);
									err=YES;
									break;
								}
							}
							(*cTail).bit.b1_0=1;
							while(!isspace(buffer[j])) /*skip the label */
								j++;
						}	
					}	
					while(isspace(buffer[j]))
							j++;
					if(buffer[j]!='\0') 	/* there is additional characters after the command */
					{
						printf("line %d: additional characters after the command\n",line);
						err=YES;
					}	
					(*cTail).address2=IC+1;
					(*cTail).address3=0;
					IC+=2;
				}
				if((k==14)||(k==15))
				{
					(*cTail).address2=0;
					(*cTail).address3=0;
					IC++;
					(*cTail).bit.b1_0=0;
					(*cTail).bit.b3_2=0;
				}
			}
			j=0;	
			label_in_use=NO;
			sFlag=NO;
			countLabel=0;
			checkArg=0;
			emptyLine=NO;
			
		}	/* while */
		if(err==YES)
			exit(0);
		ICF=IC-100;
		sNode=sHead;
		while(sNode!=NULL) 
		{
			if(strncmp((*sNode).attribute1,"data",4)==0)
				(*sNode).address=(*sNode).address+IC;
				
			else if(strncmp((*sNode).attribute1,"string",6)==0)
				(*sNode).address=(*sNode).address+IC;	
			sNode=(*sNode).next;	
		}	
		gNode=gHead;
		while(gNode!=NULL)
		{
			(*gNode).address+=IC;
			gNode=(*gNode).next;
		}
	
	/* -----------------------------------part-2-------------------------------*/
		rewind(fd);
		j=0;
		line=0;
		countLabel=0;
		
		while(fgets(buffer,MAX+1,fd)!=NULL) /* 1 */
		{ 
			line++;

			j=0;
			countLabel=0;
			if(strchr(buffer,':')) /*the line has a label,skip it*/  /* 2 */
			{
				while(buffer[j]!=':')
					j++;
				j++;	
			}
			
			while(isspace(buffer[j]))
					j++;

			if(buffer[j]=='.')
			{
				j++;
				
				if((strncmp(buffer+j,"string",6)!=0) && (strncmp(buffer+j,"data",4)!=0) && (strncmp(buffer+j,"extern",4)!=0))
				{
					if(strncmp(buffer+j,"entry",5)==0) /* 4*/
					{
						j+=5;
						while(isspace(buffer[j]))
							j++;		
						sNode=sHead;
						countS=0;
						
						while(!isspace(buffer[j+countS])) /* count the length of the symbol */
							countS++;
						while(sNode!=NULL)
						{
							if(strncmp(buffer+j,(*sNode).name,countS)==0)
							{
								(*sNode).attribute2=malloc(5);
								strncpy((*sNode).attribute2,"entry",5);
								eNode=(struct entry *)malloc(sizeof(struct entry)); /* add the symbol to the entry table */
								(*eNode).name=malloc(sizeof((*sNode).name));
								(*eNode).address=(*sNode).address;
								strncpy((*eNode).name,(*sNode).name,strlen((*sNode).name));
								(*eNode).next=NULL;
								if(eTail==NULL)
									eHead=eTail=eNode;
								else
								{	
									(*eTail).next=eNode;
									eTail=eNode;
								}	
								break;
							}
							sNode=(*sNode).next;
						}
						if(sNode==NULL)
						{
							printf("line %d: label has not been intialized\n",line);
							err=YES;
							break;
						}
					}
				}
			}
			else /* 6 */
			{
				for(k=0;k<15;k++)
				{
					if(strncmp(buffer+j,operation[k].name,3)==0)
					{
						j+=3;
						break;
					}
				}
				if(k==15)
				{
					if(strncmp(buffer+j,operation[k].name,4)==0)
					{
						j+=4;
						break;
					}
				}	
				countLabel=0;
				while(isspace(buffer[j]))
					j++;
				if(buffer[j]=='%')
				{
					j++;	
					branch=YES;
				}	
				if(buffer[j]!='#')	
				{
					while(buffer[j+countLabel]!=',' && !isspace(buffer[j+countLabel]))
						countLabel++;
					if(countLabel==2)
					{
						for(l=0;l<8;l++)
						{
							if(strncmp(buffer+j,save[l],2)==0)
								break; /* not a label */
						}
					}
					if(((countLabel==2) && (l==8)) || (countLabel!=2)) /* a label */
					{
						
						cNode=cHead;
						while(found==NO && cNode!=NULL)
						{
							if((*cNode).line==line)
							{
								sNode=sHead;
								while(found==NO && sNode!=NULL)
								{
									if(strlen((*sNode).name)==countLabel) 
									{
										
										if(strncmp((*sNode).name,buffer+j,countLabel)==0)
										{
											found=YES;
											
											if(branch)
											{
												(*cNode).word2=(*sNode).address-((*cNode).address1+1);
											}
											else
												(*cNode).word2=(*sNode).address;
											if(strncmp((*sNode).attribute1,"external",8)==0)
											{
												(*cNode).are2='E';
												(*cNode).word2=0;
												xNode=(struct external *)malloc(sizeof(struct external));
												(*xNode).name=malloc(sizeof((*sNode).name));
												strncpy((*xNode).name,(*sNode).name,strlen((*sNode).name));
												(*xNode).address=(*cNode).address1+1;
												(*xNode).next=NULL;
												if(xTail==NULL)	
													xHead=xTail=xNode;
												else
												{	
													(*xTail).next=xNode;
													xTail=xNode;
												}
											}
											else
												(*cNode).are2='R';
											break;
										}
									}	
									sNode=(*sNode).next;	
								}
								if(sNode==NULL) /* the label not in the symbel table */
								{
									printf("line %d: 1label does not exist\n",line);
									err=YES;
									break;
								}
							}
							cNode=(*cNode).next;
						}
						found=NO;
						branch=NO;
					}
				}
				else /* the first argument is a number, we skip it */
				{
					(*cNode).are2='A';
					while(!isspace(buffer[j]) && buffer[j]!=',')
						j++;
				}
				if(strchr(buffer+j,',')) /* there is another argument */
				{
					while(buffer[j]!=',')
						j++;
					j++; /* skip the comma */
					
					while(isspace(buffer[j]))
						j++;
					countLabel=0;
					
					if(buffer[j]!='#')	
					{
						while(!isspace(buffer[j+countLabel]))
							countLabel++;
							
						if(countLabel==2)
						{
							for(l=0;l<8;l++)
							{
								if(strncmp(buffer+j,save[l],2)==0)
									break; /* not a label */
							}
						}
						
						if(((countLabel==2) && (l==8)) || (countLabel!=2)) /* a label */
						{
							cNode=cHead;
							while(found==NO && cNode!=NULL)
							{
								
								if((*cNode).line==line)
								{
									sNode=sHead;
									while(found==NO && sNode!=NULL)
									{
										
										if(strlen((*sNode).name)==countLabel) 
										{
											

											if(strncmp((*sNode).name,buffer+j,countLabel)==0)
											{	
												
												found=YES;
												(*cNode).word3=(*sNode).address;
												if(strncmp((*sNode).attribute1,"external",8)==0)
												{
													(*cNode).are3='E';
													(*cNode).word3=0;
													xNode=(struct external *)malloc(sizeof(struct external));
													(*xNode).name=malloc(sizeof((*sNode).name));
													strncpy((*xNode).name,(*sNode).name,strlen((*sNode).name));
													(*xNode).address=(*cNode).address1+2;
													(*xNode).next=NULL;
													if(xTail==NULL)
														xHead=xTail=xNode;
													else
													{	
														(*xTail).next=xNode;
														xTail=xNode;
													}
												}
												else
													(*cNode).are3='R';
												break;	
	
											}	
										}
										sNode=(*sNode).next;
									}
									if(sNode==NULL) /* the label not in the symbel table */
									{
										printf("line %d: label does not exist\n",line);
										err=YES;
										break;
									}
									
								}
								cNode=(*cNode).next;
							}
							found=NO;
						}	
						
					}	
						
				}/* end of check of another argument */	
			}					
		}
		/* the file has been read completely*/
		if(err==YES)
			exit(0);
		/*---------------------------object file --------------------*/	
		strcat(sOb,".ob");
		if(!(fdOb=fopen(sOb,"w")))
		{
			fprintf(stderr,"cannot creat file\n");
			exit(0);
		}
		cNode=cHead;
		fprintf(fdOb,"%d\t%d",ICF,DC);
		fprintf(fdOb,"%c",'\n');
		while(cNode!=NULL)
		{
			fprintf(fdOb,"%d%d ",0,(*cNode).address1);
			firstHex=(*cNode).bit.b11_8;
			secondHex=(*cNode).bit.b7_4;
			thirdHex=(*cNode).bit.b1_0+((*cNode).bit.b3_2<<2);
			writeCommand(fdOb,firstHex);
			writeCommand(fdOb,secondHex);
			writeCommand(fdOb,thirdHex);
			
			fprintf(fdOb," %c",'A');
			fprintf(fdOb,"%c",'\n');
			if((*cNode).address2!=0)
			{
				
				word=(*cNode).word2;
				fprintf(fdOb,"%d%d ",0,(*cNode).address2);
			    decToHex(fdOb,word,hex2);
			    fprintf(fdOb," %c\n",(*cNode).are2);
				/*fprintf(fdOb,"%d\t%s\t%c",(*cNode).address2,hex2,(*cNode).are2);*/
			}
			if((*cNode).address3!=0)
			{
				word=(*cNode).word3;
				fprintf(fdOb,"%d%d ",0,(*cNode).address3);
				decToHex(fdOb,word,hex3);
				/*fprintf(fdOb,"%d\t%s\t%c",(*cNode).address3,hex3,(*cNode).are3);*/
				fprintf(fdOb," %c\n",(*cNode).are3);
			}
			cNode=(*cNode).next;
		}
		gNode=gHead;
		while (gNode!=NULL)
		{
			
			if((*gNode).type=='D')
			{
				for(l=0;l<(*gNode).n;l++)
				{
					fprintf(fdOb,"%d%d ",0,(*gNode).address+l);
					decToHex(fdOb,(*gNode).dat[l],hex);
					fprintf(fdOb," %c\n",'A');
				}
			}
			else if ((*gNode).type=='S')
			{
				for(l=0;l<(*gNode).sLength+1;l++)
				{
					fprintf(fdOb,"%d%d ",0,(*gNode).address+l);
					decToHex(fdOb,(int)((*gNode).str[l]),hex);
					fprintf(fdOb," %c\n",'A');
				}
			}
			gNode=(*gNode).next;
		}
		if(eHead!=NULL)
		{
			strcat(sE,".ent");
			if(!(fdE=fopen(sE,"w")))
			{
				fprintf(stderr,"cannot creat file\n");
				exit(0);
			}
			eNode=eHead;
		
			while (eNode!=NULL)
			{
				fprintf(fdE,"%s %d%d\n",(*eNode).name,0,(*eNode).address);
				eNode=(*eNode).next;
			}
			fclose(fdE);
		}
		if(xHead!=NULL)
		{
			strcat(sX,".ext");
			if(!(fdX=fopen(sX,"w")))
			{
				fprintf(stderr,"cannot creat file\n");
				exit(0);
			}
			xNode=xHead;
			while (xNode!=NULL)
			{
				fprintf(fdE,"%s %d%d\n",(*xNode).name,0,(*xNode).address);
				xNode=(*xNode).next;
			}
			fclose(fdX);
		}
		
		fclose(fd);
		fclose(fdOb);					
	}
	return 0;
}		
void writeCommand(FILE *fdOb,int hex) /* the method writes a part of the mechine code in hexadecimal */	
{
	
	switch(hex)
	{
		case 10: fprintf(fdOb,"%c",'A');
			break;
		case 11: fprintf(fdOb,"%c",'B');
			break;
		case 12: fprintf(fdOb,"%c",'C');
			break;
		case 13: fprintf(fdOb,"%c",'D');
			break;
		case 14: fprintf(fdOb,"%c",'E');
			break;
		case 15: fprintf(fdOb,"%c",'F');
			break;
		default:fprintf(fdOb,"%d",hex);
			break;
	}
}

void decToHex(FILE *fdOb,int word,char hex[]) /* convert word to hexadecimal and store the string in hex */
{
	unsigned int left=word>>8 & 15;
	unsigned int middle=(word>>4) & 15; 
	unsigned int right= word & 15;
	unsigned int x;
	int i;
	for(i=0;i<3;i++)
	{
		if(i==0)
		{
			x=left;
		}	
			
		if(i==1)
		{
			x=middle;
		}
		if(i==2)
		{
			x=right;
		}
		
		switch (x)
		{
			case 10:fprintf(fdOb,"%c",'A');
				break;
			case 11:fprintf(fdOb,"%c",'B');
				break;
			case 12:fprintf(fdOb,"%c",'C');
				break;
			case 13:fprintf(fdOb,"%c",'D');
				break;
			case 14:fprintf(fdOb,"%c",'E');
				break;
			case 15:fprintf(fdOb,"%c",'F');
				break;
			default:fprintf(fdOb,"%d",x);
				break;
		}	
	}
	
}



































