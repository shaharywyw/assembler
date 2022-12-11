#include "assembler.h"
void openX(char *sX,struct external **xHead)
{
	FILE *fdX;
	struct external *xNode=NULL;
	
	strcat(sX,".ext");
	if(!(fdX=fopen(sX,"w")))
	{
		fprintf(stderr,"cannot creat file\n");
		exit(0);
	}
	xNode=*xHead;
	while (xNode!=NULL)
	{
		fprintf(fdX,"%s %d%d\n",(*xNode).name,0,(*xNode).address);
		xNode=(*xNode).next;
	}
	fclose(fdX);
	
}
