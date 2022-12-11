#include "assembler.h"
void openE(char *sE,struct entry **eHead)
{
	FILE *fdE;
	struct entry *eNode=NULL;
	strcat(sE,".ent");
	if(!(fdE=fopen(sE,"w")))
	{
		fprintf(stderr,"cannot creat file\n");
		exit(0);
	}
	eNode=*eHead;

	while (eNode!=NULL)
	{
		fprintf(fdE,"%s %d%d\n",(*eNode).name,0,(*eNode).address);
		eNode=(*eNode).next;
	}
	fclose(fdE);
	
}
