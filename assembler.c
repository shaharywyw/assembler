#include "assembler.h"
int main (int argc, char *argv[])
{
	FILE *fd;
	FILE *fdOb,*fdE,*fdX;
	
	enum flag err1=NO,err2=NO,OPENFILES=NO;
	struct symbol *sHead,*sTail,*sNode;
	struct command *cHead,*cTail,*cNode;
	struct guidance *gHead,*gTail,*gNode;
	struct external *xHead,*xTail,*xNode;
	struct entry *eHead,*eTail,*eNode;
	struct operation op[]={{"mov",0,0,0,0},{"cmp",1,0,0,0},{"add",2,10,0,0},{"sub",2,11,0,0},{"lea",4,0,0,0},{"clr",5,10,0,0}
				,{"not",5,11,0,0},{"inc",5,12,0,0},{"dec",5,13,0,0},{"jmp",9,10,0,0},{"bne",9,11,0,0},{"jsr",9,12,0,0}
				,{"red",12,0,0,0},{"prn",13,0,0,0},{"rts",14,0,0,0},{"stop",15,0,0,0}};
				
	int IC[1]={100},ICF,DC[1]={0};		
	int i;
	char *s,*sRead,*sOb,*sE,*sX; /*file name*/
	char buffer[MAX];

	
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
		
		IC[0]=100; /* step 1 of part 1 */
		DC[0]=0;
		
		sHead=sTail=sNode=NULL;
		cHead=cTail=cNode=NULL;
		gHead=gTail=gNode=NULL;
		xHead=xTail=xNode=NULL;
		eHead=eTail=eNode=NULL;
		err1=err2=OPENFILES=NO;
	/* -------------------------------part-1--------------------*/
	
		err1=partOne(fd,sRead,buffer,&sHead,&sTail,&cHead,&cTail,&gHead,&gTail,op,IC,DC);
		
		ICF=IC[0]-100;

	/* -------------------------------part-2--------------------*/
	
		if(err1==NO)	/* step 17 of part 1 */
			err2=partTwo(fd,buffer,sRead,&sHead,&sTail,&cHead,&cTail,&gHead,&gTail,&eHead,&eTail,&xHead,&xTail,op,IC,DC); /* 20 */
		
		if(err1==NO && err2==NO) /* step 7 of part 2 */
			OPENFILES=YES;
				
	/*--------------------------- files --------------------*/	

	if(OPENFILES==YES)
	{	
		openObj(sOb,&cHead,&gHead,ICF,*DC); /* step 8 of part 2 */

		if(xHead!=NULL)
		{
			openX(sX,&xHead);
			while(xHead!=NULL)
			{
				xNode=xHead;
				
				xHead=(*xHead).next;
				free((*xNode).name);
				free(xNode);
			}
			free(sX);
		}	
		
		if(eHead!=NULL)
		{
			openE(sE,&eHead);
			while(eHead!=NULL)
			{
				eNode=eHead;
				eHead=(*eHead).next;
				free((*eNode).name);
				free(eNode);
			}
			free(sE);
		}
	}	
	/*------------------free-all-the-allocations------------*/
		
		free(s);
		free(sRead);
		free(sOb);
		
		while(sHead!=NULL)
		{
			sNode=sHead;
			sHead=(*sHead).next;
			free((*sNode).name);
			free(sNode);
		}
		
		while(gHead!=NULL)
		{
			gNode=gHead;
			gHead=(*gHead).next;
			if((*gNode).type=='D')
				free((*gNode).dat);
					
			else if((*gNode).type=='S')	
				free((*gNode).str);
			free(gNode);	
		}
		while(cHead!=NULL)
		{
			cNode=cHead;
			cHead=(*cHead).next;
			free(cNode);
		}			 		
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

void decToHex(FILE *fdOb,int word) /* convert 'word' to hexadecimal in string and store in hex */
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
	

