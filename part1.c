#include "assembler.h"

enum flag partOne(FILE *fd,char *sRead,char *buffer,struct symbol **sHead,struct symbol **sTail,struct command **cHead,struct command **cTail,struct guidance **gHead,	struct 						guidance **gTail,struct operation op[],int *IC,int *DC)
{
	enum flag err=NO,sFlag=NO,emptyLine=NO;
	enum flag label_in_use=NO,NOT_VALID=NO;
	enum flag error1=NO; 
	int line =0;
	int checkArg=0;
	int i,k,l,f,j=0;
	char minus=0;
	int countLabel=0; /*count label length*/
	int countS=0; 	/* to count the length of the string in .string guidance */
	int countComma=0; /* to count commas in a data command,the number of numbers are countComma+1 */
	struct symbol *sNode=NULL;
	struct command *cNode=NULL;
	struct guidance *gNode=NULL;
	
	char *save[NSW]={"r0","r1","r2","r3","r4","r5","r6","r7","mov","cmp","add","sub","lea","clr","not"
					,"inc ","dec","jmp","bne","jsr","red","prn","rts","stop"};	
	
	strcat(sRead,".as");
	
	if(!(fd=fopen(sRead,"r")))
	{
		fprintf(stderr,"\ncannot open file\n");
		exit(0);
	}
	
	while(fgets(buffer,MAX+1,fd)!=NULL)		/* 2 */
	{		
		line++;
		j=0;	
		label_in_use=NO;
		sFlag=NO;
		countLabel=0;
		checkArg=0;
		countS=0;
		countComma=0;
		minus=0;
		emptyLine=NO;
		NOT_VALID=NO;
		if(err==YES)
			error1=YES;	
		err=NO;	
		
		while (isspace(buffer[j]))
			j++;	
		if(buffer[j]=='\0' || buffer[j]==';')
			emptyLine=YES;	
			
		if(strchr(buffer,':')!=NULL && !emptyLine)	/* 3 */	
		{
				
            if(isdigit(buffer[j]))
			{
				printf("%s line %d: label can not start with a digit\n",sRead,line);
				err=YES;
				continue;
			} 		
			while(buffer[j+countLabel]!=':')
			{
				if(!isalpha(buffer[j+countLabel]) && !isdigit(buffer[j+countLabel]))
					NOT_VALID=YES;
				countLabel++;	
			}
			if(countLabel>MAXLABEL)
			{
				printf("%s line %d: max length of label is %d characters\n",sRead,line ,MAXLABEL);
				err=YES;
				continue;
			}	
			if(NOT_VALID==YES)
			{
				printf("%s line %d: label can not use none alphabetical or digit character\n",sRead,line);
				err=YES;
				continue;
			}
			
			for(k=0;k<NSW;k++) /* check if label is not a saved word */
			{
		
				if((k<NOREGISTERS) && (countLabel==2) && ((strncmp(buffer+j,save[k],2))==0))
				{
					printf("%s line %d:label uses a register name\n",sRead,line);
					err=YES;
					break;
				}	
				else if((k>=NOREGISTERS) && (k!=NSW-1) && (countLabel==3) && (strncmp(buffer+j,save[k],3)==0))
				{
					printf("%s line %d:label uses a command name\n",sRead,line);
					err=YES;
					break;
				}
				else if ((k==NSW-1) && (countLabel==4)&& (strncmp(buffer+j,save[k],4)==0))
				{
					printf("%s line %d:label uses a command name\n",sRead,line);
					err=YES;
					break;
				}
			}
			if(err==YES)
				continue;
				
			if(k==NSW) /*means that the lable is not a safe word */ /* 3 */
			{
				sNode=*sHead;
				while(sNode!=NULL)
				{
					if(strncmp((*sNode).name,buffer+j,countLabel)==0)
					{
						printf("%s line %d: label already in use\n",sRead,line);
						label_in_use=YES;
						err=YES;
						sFlag=YES;
						break;
					}
					sNode=(*sNode).next;
				}
				if(err==YES)
				continue;
				
				if(label_in_use==NO)
				{
					sNode=(struct symbol*)malloc(sizeof(struct symbol));
					(*sNode).name=calloc(countLabel,1);
					strncpy((*sNode).name,buffer+j,countLabel);
					(*sNode).address=*IC;
					(*sNode).attribute1=NULL;
					(*sNode).attribute2=NULL;
					(*sNode).next=NULL;
					if(*sTail==NULL)
						*sHead=*sTail=sNode;
					else
					{	
						(*sTail)->next=sNode;
						*sTail=sNode;
					}	
					sFlag=YES;		/* 4 */
				}
			 }		
					
		}		/* end 3 */
		else /*there is no label*/ 
			sFlag=NO;
		if(sFlag==YES)
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
			(*gNode).address=*DC;	
			(*gNode).line=line;	
			(*gNode).type=NULL;
			(*gNode).str=NULL;
			(*gNode).dat=NULL;
			(*gNode).next=NULL;
			if(*gTail==NULL)
				*gHead=*gTail=gNode;
			else
			{
				(*gTail)->next=gNode;
				*gTail=gNode;
			}
			if(strncmp(buffer+j,"string",6)==0) /* 7 */
			{
				j+=6;	
				if(!isspace(buffer[j]))
				{	
					printf("%s line %d: not a valid command\n",sRead,line);
					err=YES;
					continue;
				}
				if(sFlag)
				{
					(*sTail)->attribute1=malloc(6);
					(*sTail)->attribute1="string"; /* 6 */
				}
				while(isspace(buffer[j]))
					j++;
				
				if(buffer[j]!='\"')
				{
					err=YES;
					printf("%s line %d: missing \" \n",sRead,line);
					continue;
				}
				else
				{	
					j++; /* skip the " */
					countS=0;
					if(!strchr(buffer+j,'\"'))
					{	
						err=YES;
						printf("%s line %d: missing \" \n",sRead,line);
						continue;
					}
					while(buffer[j+countS]!='\"')
						countS++;
					(*gTail)->str=malloc(countS);
					(*gTail)->sLength=countS;
					(*gTail)->type='S';
					for(l=0;l<countS;l++)
					{
						(*gTail)->str[l]=buffer[j];
						j++;
					}	
					(*gTail)->str[l]='\0';
					j++;
					while(isspace(buffer[j]))
						j++;
					if(buffer[j]!='\0')
					{
						printf("%s line %d: extra characters after the command\n",sRead,line);
					}	
					*DC=*DC+countS+1;
				}
			}	
			else if(strncmp(buffer+j,"data",4)==0)	/* 7 */
			{
				j+=4;
				if(!isspace(buffer[j]))
				{	
					printf("%s line %d: not a valid command\n",sRead,line);
					err=YES;
					continue;
				}
				if(sFlag)
				{
					(*sTail)->attribute1=malloc(4);
					(*sTail)->attribute1="data"; /* 6 */
					(*sTail)->address=*DC;
				}
				while(isspace(buffer[j]))
					j++;
				countComma=0;
				for(l=0;buffer[j+l]!='\0';l++)
				{
					if(buffer[j+l]==',')
						countComma++;
				}	
				countComma++; /* the number of numbers is countComma +1 */
				(*gTail)->n=countComma;
				(*gTail)->type='D';
				(*gTail)->dat=(int*)malloc(countComma*sizeof(int));
			
				for(k=0;k<countComma;k++)
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
						(*gTail)->dat[k]=atoi(buffer+j);
						if(minus==1)
							(*gTail)->dat[k]*=(-1);
					}	
					else
					{
						printf("%s line %d: including none number values or missing arguments\n",sRead,line);
						err=YES;
						break;
					}	
					while(isdigit(buffer[j]))
						j++;
					while(isspace(buffer[j]) && buffer[j]!='\0')
						j++;	
					
					if(isdigit(buffer[j]) || buffer[j]=='-' || buffer[j]=='+')
					{
						printf("%s line %d: missing comma\n",sRead,line);
						err=YES;
						break; 
					}	
					if(buffer[j]!='\0' && buffer[j]!=',')
					{
						printf("%s line %d: including none natural numbers \n",sRead,line);
						err=YES;
						break;
					}
					if(buffer[j]!='\0')
						j++; /* skip the comma */
					while(isspace(buffer[j]) && buffer[j]!='\0')
						j++;
					minus=0;
				}
				if(err==YES)
					continue;
			
				*DC=*DC+countComma;	
			}
			else if(strncmp(buffer+j,"entry",5)==0) /* 9 */ 
			{
				j+=5;

				if(!isspace(buffer[j]))
				{	
					printf("%s line %d: not a valid command\n",sRead,line);
					err=YES;
					continue;
				}
				while(isspace(buffer[j])) /*skip spaces*/
					j++;
				while(!isspace(buffer[j]))/* skip label */
					j++;
				while(isspace(buffer[j]))
					j++;		
				if(buffer[j]!='\0')
				{
					printf("%s line %d: extra characters in the command\n",sRead,line);
					err=YES;
					continue;
				}	
			}
			else if(strncmp(buffer+j,"extern",6)==0) /* 10 */
			{
				j+=6;
				
				if(!isspace(buffer[j]))
				{	
					printf("%s line %d: not a valid command\n",sRead,line);
					err=YES;
					continue;
				}
				countLabel=0;
				while(isspace(buffer[j]))
					j++;
				while(!isspace(buffer[j+countLabel]))
				{
					if(!isalpha(buffer[j+countLabel]) && !isdigit(buffer[j+countLabel]))
						NOT_VALID=YES;
					countLabel++;
				}	
				if(isdigit(buffer[j]))
				{
					printf("%s line %d: label can not start with a digit\n",sRead,line);
					err=YES;
					continue;
				} 		
				if(countLabel>MAXLABEL)
				{
					printf("%s line %d: max length of label is %d characters\n",sRead,line ,MAXLABEL);
					err=YES;
					continue;
				}	
				if(NOT_VALID==YES)
				{
					printf("%s line %d: label can not use none alphabetical or digit character\n",sRead,line);
					err=YES;
					continue;
				}
				for(k=0;k<NSW;k++) /* check if label is not a saved word */
				{
					if((k<NOREGISTERS) && (countLabel==2) && (strncmp(buffer+j,save[k],2)==0))
					{
						printf("%s line %d:label uses a register name\n",sRead,line);
						err=YES;
						break;
					}	
					else if((k>=NOREGISTERS) && (k!=NSW-1) && (countLabel==3) && (strncmp(buffer+j,save[k],3)==0))
					{
						printf("%s line %d:label uses a command name\n",sRead,line);
						err=YES;
						break;
					}
					else if ((k==NSW-1) && (countLabel==4)&& (strncmp(buffer+j,save[k],4)==0))
					{
						printf("%s line %d:label uses a command name\n",sRead,line);
						err=YES;
						break;
					}	
				 }
				 if(err==YES)
					continue;
				 if(k==NSW) /*means that the lable isn't a safe word */ 
				 {
				 	
					while(sNode!=NULL)
					{
						if(strncmp((*sNode).name,buffer+j,countLabel)==0)
						{
							printf("%s line %d: label already in use\n",sRead,line);
							label_in_use=YES;
							break;
						}
						sNode=(*sNode).next;
					}
					if(err==YES)
						continue;
					
					if(label_in_use==NO)
					{
						
						sNode=(struct symbol*)malloc(sizeof(struct symbol));
						(*sNode).name=calloc(countLabel,1);
						strncpy((*sNode).name,buffer+j,countLabel);
						(*sNode).address=*IC;
						(*sNode).attribute1=malloc(8);
						(*sNode).attribute2=malloc(1);
						(*sNode).attribute1="external"; 
						(*sNode).attribute2="0";
						(*sNode).next=NULL;
						if(*sTail==NULL)
						{
							*sHead=*sTail=sNode;
						}	
						else
						{	
							(*sTail)->next=sNode;
							*sTail=sNode;
						}	
						sFlag=YES;	
					}
					j+=countLabel;
					while(isspace(buffer[j]))
						j++;
					if(buffer[j]!='\0')
					{
						printf("%s line %d: extra characters in the command\n",sRead,line);
						err=YES;
						continue;
					}						
				 }		
			}
			else 
			{ 
				printf("%s line %d: not a valid command \n",sRead,line);
				err=YES;
				continue;
			}				
		}	
		else if(!emptyLine) /* the line is a command */ /* 11 */
		{
				
			if(sFlag)
			{
				(*sTail)->attribute1=malloc(4);
				(*sTail)->attribute1="code";
				(*sTail)->address=*IC;
			}	
			for(k=0;k<15;k++)
			{
				if((strncmp(buffer+j,op[k].name,3)==0) && (isspace(buffer[j+3]))) /* 12 */
				{
					cNode=(struct command *)malloc(sizeof(struct command));
					(*cNode).comm=op[k].name;
					(*cNode).line=line;
					(*cNode).address1=*IC;
					(*cNode).bit.b11_8=op[k].opcode;
					(*cNode).bit.b7_4=op[k].funct;
					(*cNode).next=NULL;
					if(*cTail==NULL)
						*cHead=*cTail=cNode;
					else
					{
						(*cTail)->next=cNode;
						*cTail=cNode;
					}	
				
					j+=3;
					break;
				}	
			}	
		
			/* commands */
			if((k==15) && (strncmp(buffer+j,op[15].name,4)==0)&& isspace(buffer[j+4]) )
			{
				cNode=(struct command *)malloc(sizeof(struct command));
				(*cNode).comm=op[15].name;
				(*cNode).line=line;
				(*cNode).address1=*IC;
				(*cNode).bit.b11_8=op[k].opcode;
				(*cNode).bit.b7_4=op[k].funct;
				(*cNode).next=NULL;
				if(*cTail==NULL)
					*cHead=*cTail=cNode;
				else
				{
					(*cTail)->next=cNode;
					*cTail=cNode;
				}	
			
				j+=4;
			}
			else if(k==15)
			{
				printf("%s line %d: command is not valid\n",sRead,line);
				err=YES;
				continue;
			}
			while(isspace(buffer[j]))
				j++;
			if(buffer[j]=='\0' && k<14)
			{
				err=YES;
				printf("%s line %d: missing argument\n",sRead,line); /*move to the next line */
				continue;
			}	
				
			/* ------ 13 -------*/
			
			if(k<4)
			{
			
				if(buffer[j]=='#')
				{
					j++;
					if(buffer[j]=='-')
					{
						j++;
						(*cTail)->word2=atoi(buffer+j)*(-1);
					}	
					else if(buffer[j]=='+')
					{
						j++;
						(*cTail)->word2=atoi(buffer+j);
					}
					else if(!isdigit(buffer[j]))
					{
						printf("%s line %d: expected a number \n",sRead,line);
						err=YES;
						continue;
					}
					else
						(*cTail)->word2=atoi(buffer+j);		
					(*cTail)->bit.b3_2=0;
					(*cTail)->are2='A';
					while (isdigit(buffer[j]))
						j++; /*skip the first word */
				}
				else if (isdigit(buffer[j]))
				{
					printf("%s line %d: missing #",sRead,line);
					err=YES;
					continue;
				}
				else 
				{
					checkArg=0;
					
					if(!strchr(buffer,','))
					{
						printf("%s line %d: missing comma \n",sRead,line);
						err=YES;
						continue;
					}
					while(buffer[j+checkArg]!=',' && !isspace(buffer[j+checkArg]))
						checkArg++;				
					for(l=0;l<NOREGISTERS;l++)
					{
						if((checkArg==2) && (strncmp(buffer+j,save[l],2)==0))
						{
							(*cTail)->bit.b3_2=3;
							(*cTail)->are2='A';
							j++;	/*skip the character 'r'*/
							(*cTail)->word2=1<<atoi(buffer+j);
							j++; /*skip the first word */
							break;
						}
					}
					
					
					if(l==NOREGISTERS) /* the first argument is a label,skip it */
					{
						for(f=0;f<NSW;f++) /* check if argument is not a saved word */
						{
							if((f>=NOREGISTERS) && (f!=NSW-1) && (checkArg==3) && (strncmp(buffer+j,save[f],3)==0))
							{
								printf("%s line %d: argument is a command saved name\n",sRead,line);
								err=YES;
								break;
							}
							else if ((f==NSW-1) && (checkArg==4)&& (strncmp(buffer+j,save[f],4)==0))
							{
								printf("%s line %d argument is a command saved name\n",sRead,line);
								err=YES;
								break;
							}
						}	
						if(err==YES)
							continue;
						(*cTail)->bit.b3_2=1;
						while(buffer[j]!=',')
							j++;
						
					}	
						
				}		
				while (isspace(buffer[j])) 
					j++;
				if(buffer[j]=='\0')
				{
					err=YES;
					printf("%s line %d: missing argument\n",sRead,line); /*move to the next line */
					continue;
				}		
				if(buffer[j]!=',')
				{
					err=YES;
					printf("%s line %d: missing argument\n",sRead,line); /*move to the next line */
					continue;
				}
				else
				{
					j++; /* skip comma */
					if(buffer[j]=='#')
					{
						err=YES;
						printf("%s line %d: destination operand can not be a number\n",sRead,line);
						continue;
					}
					checkArg=0;
					while(isspace(buffer[j]) && buffer[j] != ',')
						j++;
					if(buffer[j]=='\0')
					{
						err=YES;
						printf("%s line %d: missing argument\n",sRead,line); /*move to the next line */
						continue;
					}		
					while(!isspace(buffer[j+checkArg]))
						checkArg++;
					if(checkArg==0)
					{
						err=YES;
						printf("%s line %d: missing argument\n",sRead,line); /*move to the next line */
						continue;
					}	
					for(l=0;l<NOREGISTERS;l++)
					{
						if((checkArg==2) && (strncmp(buffer+j,save[l],2)==0))
						{
							(*cTail)->bit.b1_0=3;
							(*cTail)->are3='A';
							j++;	/*skip the character 'r'*/
							(*cTail)->word3=1<<atoi(buffer+j);
							j++; /*skip the second word */
							break;
						}
					}
					if((l==NOREGISTERS) && (k!=1)) /*means that the second word is a label, skip it */
					{
						(*cTail)->bit.b1_0=1;
						while(!isspace(buffer[j]))
							j++;	
					}
					if((k==1) && (l==NOREGISTERS))	/* the command is cmp and the second argument might be a number */
					{
						if(buffer[j]=='#')
						{
							j++;
							if(buffer[j]=='-')
							{
								j++;
								(*cTail)->word3=atoi(buffer+j)*(-1);
							}
							else if (buffer[j]=='+')
							{
								j++;
								(*cTail)->word3=atoi(buffer+j);
							}
							else
								(*cTail)->word3=atoi(buffer+j);
							(*cTail)->bit.b1_0=0;
							(*cTail)->are3='A';
							while (isdigit(buffer[j]))
								j++; /*skip the second argument */
						}
						else if (isdigit(buffer[j]))
						{
							printf("%s line %d: missing #",sRead,line);
							err=YES;
							continue;
						}
					}	
					while(isspace(buffer[j]))
						j++;
					if(buffer[j]!='\0') 	
					{
						printf("%s line %d: wrong input for the command \n",sRead,line);
						err=YES;
						continue;
					}	
				
					(*cTail)->address2=*IC+1;
					(*cTail)->address3=*IC+2;
					*IC+=3; /* 16 */
				}		
			}
			if(k==4)
			{
				countLabel=0;
				while(!isspace(buffer[j+countLabel]) && buffer[j+countLabel]!=',')
					countLabel++;
				if(countLabel==2)
				{
					for(i=0;i<NOREGISTERS;i++)
						if(strncmp(buffer+j,save[i],2)==0)
						{
							printf("%s line: %d label can't be named as a register \n",sRead,line);
							err=YES;
							continue;
						}
				}	
				(*cTail)->bit.b3_2=1;
				j=j+countLabel;
				while(isspace(buffer[j]))
					j++;
				
				if(buffer[j]!=',')
				{
					err=YES;
					printf("%s line %d: missing argument\n",sRead,line); /*move to the next line */
					continue;
				}
				j++; /* skip comma */
				checkArg=0;
				while(isspace(buffer[j]))
					j++;
				if(buffer[j]=='\0')
				{
					err=YES;
					printf("%s line %d: missing argument\n",sRead,line); /*move to the next line */
					continue;
				}		
				while(!isspace(buffer[j+checkArg]))
					checkArg++;
				if(checkArg==2)
				{		
					for(l=0;l<NOREGISTERS;l++)
					{
						if(strncmp(buffer+j,save[l],2)==0)
						{
							(*cTail)->bit.b1_0=3;
							(*cTail)->are3='A';
							j++;	/*skip the character 'r'*/
							(*cTail)->word3=1<<atoi(buffer+j);
							j++; /*skip the second word */
							break;
						}
					}
					if(l==NOREGISTERS) /* second argument is a label */
					{
						j+=checkArg;	
						(*cTail)->bit.b1_0=1;
					}	
				}	
				while(isspace(buffer[j]) && buffer[j]!=',')
					j++;
				if(buffer[j]!='\0') 	
				{
					printf("%s line %d: wrong input for the command \n",sRead,line);
					err=YES;
					continue;
				}	
				(*cTail)->address2=*IC+1;
				(*cTail)->address3=*IC+2;
				*IC=*IC+3;		/* 16 */
			}
			if(k>4 && k<9) /* one operand */
			{
			
				(*cTail)->bit.b3_2=0;
				if(buffer[j]=='#')
				{
					printf("%s line %d: argument can not be a number\n",sRead,line);
					err=YES;
					continue;
				}
				checkArg=0;
				(*cTail)->are2='A';
				while(!isspace(buffer[j+checkArg]))
					checkArg++;
				if(checkArg==2)
				{	
					for(l=0;l<NOREGISTERS;l++)
					{
				
						if(strncmp(buffer+j,save[l],2)==0)
						{
						
							(*cTail)->bit.b1_0=3;
							j++;	/*skip the character 'r'*/
							(*cTail)->word2=1<<atoi(buffer+j);
							j++; /*skip the second word */
							break;
						}
					}
					if(l==NOREGISTERS) /* argument is a label */
					{
						j+=checkArg;
						(*cTail)->bit.b1_0=1;
					
					}
				}	
				
				if((l==NOREGISTERS) || checkArg!=2)  /* argument is a label */
				{
					for(f=0;f<NSW;f++) /* check if argument is not a saved word */
					{
						if((f>=NOREGISTERS) && (f!=NSW-1) && (checkArg==3) && (strncmp(buffer+j,save[f],3)==0))
						{
							printf("%s line %d: argument is a command name\n",sRead,line);
							err=YES;
							break;
						}
						else if ((f==NSW-1) && (checkArg==4)&& (strncmp(buffer+j,save[f],4)==0))
						{
							printf("%s line %d: argument is a command name\n",sRead,line);
							err=YES;
							break;
						}
					}
					if(err==YES)
						continue;
					j+=checkArg;
					(*cTail)->bit.b1_0=1;
					
				}	
				while(isspace(buffer[j]) && buffer[j]!='\0')
					j++;
				if(buffer[j]!='\0') 	
				{
					printf("%s line %d: wrong input for the command\n",sRead,line);
					err=YES;
					continue;
				}
			
				(*cTail)->address2=*IC+1;
				(*cTail)->address3=0;
				*IC=*IC+2;			/* 16 */
			}
		
			if(k>NOREGISTERS && k<12)
			{
				(*cTail)->bit.b3_2=0;
				if(buffer[j]!='%')
				{
					printf("%s line %d: missing precentage symbol \n",sRead,line);
					err=YES;
					continue;
				}	
				j++;
				if(buffer[j]=='#')
				{
					printf("%s line %d: argument can not be a number\n",sRead,line);
					err=YES;
					continue;
				}
				(*cTail)->bit.b1_0=2;
				countLabel=0;
				while(!isspace(buffer[j+countLabel])) 
					countLabel++;
				for(f=0;f<NSW;f++) /* check if label is not a saved word */
				{
			
					if((f<NOREGISTERS) && (countLabel==2) && ((strncmp(buffer+j,save[f],2))==0))
					{
						printf("%s line %d:argument can not be a register\n",sRead,line);
						err=YES;
						break;
					}	
					else if((f>=NOREGISTERS) && (f!=NSW-1) && (countLabel==3) && (strncmp(buffer+j,save[f],3)==0))
					{
						printf("%s line %d:argument can not be a command name\n",sRead,line);
						err=YES;
						break;
					}
					else if ((f==NSW-1) && (countLabel==4)&& (strncmp(buffer+j,save[f],4)==0))
					{
						printf("%s line %d: argument can not use a command name\n",sRead,line);
						err=YES;
						break;
					}
				}
				if(err==YES)
					continue;

				j+=countLabel; /*skip the label */
						
				while(isspace(buffer[j]))
					j++;
				if(buffer[j]!='\0') 	
				{
					printf("%s line %d: wrong input for the command\n",sRead,line);
					err=YES;
					continue;
				}
				(*cTail)->address2=*IC+1;
				(*cTail)->address3=0;
				*IC+=2;		/* 16 */ 		
			}
			if(k==12)
			{
				(*cTail)->bit.b3_2=0;
				if(buffer[j]=='#')
				{
					printf("%s line %d: argument can not be a number\n",sRead,line);
					err=YES;
					continue;
				}
				if(buffer[j]=='%')
				{
					printf("%s line %d: invalid argument\n",sRead,line);
					err=YES;
					continue;
				}
				checkArg=0;
				while(!isspace(buffer[j+checkArg]))
					checkArg++;
				if(checkArg==2)
				{
					for(l=0;l<NOREGISTERS;l++)
					{
						if(strncmp(buffer+j,save[l],2)==0)
						{
							(*cTail)->bit.b1_0=3;
							(*cTail)->are3='A';
							j++;	/*skip the character 'r'*/
							(*cTail)->word3=1<<atoi(buffer+j);
							j++; /*skip the second word */
							break;
						}
					}	
				}
				if((l==NOREGISTERS) || (checkArg!=2))	/* the argument is a label */
				{
					for(f=0;f<NSW;f++) /* check if argument is not a saved word */
					{
						if((f>=NOREGISTERS) && (f!=NSW-1) && (checkArg==3) && (strncmp(buffer+j,save[f],3)==0))
						{
							printf("%s line %d: argument is a command name\n",sRead,line);
							err=YES;
							break;
						}
						else if ((f==NSW-1) && (checkArg==4)&& (strncmp(buffer+j,save[f],4)==0))
						{
							printf("%s line %d: argument is a command name\n",sRead,line);
							err=YES;
							break;
						}
					}
					if(err==YES)
						continue;
					(*cTail)->bit.b1_0=1;
					j+=checkArg;
						
					while(isspace(buffer[j]))
						j++;
					if(buffer[j]!='\0') 	
					{
						printf("%s line %d: wrong input for the command\n",sRead,line);
						err=YES;
						continue;
					}	
				}
				(*cTail)->address2=*IC+1;
				(*cTail)->address3=0;
				*IC+=2;		/* 16 */
			}
			if(k==13)
			{
				(*cTail)->bit.b3_2=0;
				if(buffer[j]=='#')
				{
					j++;
					if(buffer[j]=='-')
					{
						j++;
						(*cTail)->word2=atoi(buffer+j)*(-1);
					}
					else if (buffer[j]=='+')
					{
						j++;
						(*cTail)->word2=atoi(buffer+j);
					}
					else
						(*cTail)->word2=atoi(buffer+j);
					(*cTail)->bit.b1_0=0;
					(*cTail)->are3='A';
				
					while(isdigit(buffer[j]))
						j++;
				}
				else if (isdigit(buffer[j]))
				{
					printf("%s line %d: missing #\n",sRead,line);
					err=YES;
					continue;
				}
				else if(buffer[j]=='%')
				{
					printf("%s line %d: invalid argument \n",sRead,line);
					err=YES;
					continue;
				}	
				else	/* argument is a label or a register */
				{
					checkArg=0;
					while(!isspace(buffer[j+checkArg]))
						checkArg++;
					if(checkArg==2)
					{
						for(l=0;l<NOREGISTERS;l++)
						{
							if(strncmp(buffer+j,save[l],2)==0)
							{
								(*cTail)->bit.b1_0=3;
								(*cTail)->are3='A';
								j++;	/*skip the character 'r'*/
								(*cTail)->word3=1<<atoi(buffer+j);
								j++; /*skip the second word */
								break;
							}
						}	
					}	
					if((l==NOREGISTERS)|| (checkArg!=2))	/* the argument is a label */
					{
						for(f=0;f<NSW;f++) /* check if argument is not a saved word */
						{
							if((f>=NOREGISTERS) && (f!=NSW-1) && (checkArg==3) && (strncmp(buffer+j,save[f],3)==0))
							{
								printf("%s line %d: argument is a command name\n",sRead,line);
								err=YES;
								break;
							}
							else if ((f==NSW-1) && (checkArg==4)&& (strncmp(buffer+j,save[f],4)==0))
							{
								printf("%s line %d: argument is a command name\n",sRead,line);
								err=YES;
								break;
							}
						 }
						 if(err==YES)
							continue;
						(*cTail)->bit.b1_0=1;
						while(!isspace(buffer[j])) /*skip the label */
							j++;
					}	
				}	
				while(isspace(buffer[j]))
						j++;
				if(buffer[j]!='\0') 	
				{
					printf("%s line %d: wrong input for the command\n",sRead,line);
					err=YES;
					continue;
				}	
				(*cTail)->address3=*IC+1;
				(*cTail)->address2=0;
				*IC+=2;		/* 16 */
			}
			if((k==14)||(k==15))
			{
				while(isspace(buffer[j]))
						j++;
				if(buffer[j]!='\0') 	
				{
					printf("%s line %d: command has no arguments\n",sRead,line);
					err=YES;
					continue;
				}	
				(*cTail)->address2=0;
				(*cTail)->address3=0;
				*IC=*IC+1;		/* 16 */
				(*cTail)->bit.b1_0=0;
				(*cTail)->bit.b3_2=0;
			}
		}/* ----- 13 ------ */
		
	}	
	sNode=*sHead;
	while(sNode!=NULL) /* 19 */
	{
		if((*sNode).attribute1 !=NULL && strncmp((*sNode).attribute1,"data",4)==0)
			(*sNode).address=(*sNode).address+*IC;
			
		else if((*sNode).attribute1 !=NULL && strncmp((*sNode).attribute1,"string",6)==0)
			(*sNode).address=(*sNode).address+*IC;	
		sNode=(*sNode).next;	
	}	
	gNode=*gHead;
	while(gNode!=NULL)
	{
		(*gNode).address+=*IC;
		gNode=(*gNode).next;
	}
	
	fclose(fd);
	return error1;
}
