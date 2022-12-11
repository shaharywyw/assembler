#include "assembler.h"
void openObj(char *sOb,struct command **cHead,struct guidance **gHead,int ICF,int DC)
{
	FILE *fdOb;
	int firstHex,secondHex,thirdHex;
	int word;
	int l;
	struct command *cNode=NULL;
	struct guidance *gNode=NULL;
	
	strcat(sOb,".ob");
	if(!(fdOb=fopen(sOb,"w")))
	{
		fprintf(stderr,"cannot creat file\n");
		exit(0);
	}
	cNode=*cHead;
	
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
		    decToHex(fdOb,word);
		    fprintf(fdOb," %c\n",(*cNode).are2);
		}
		if((*cNode).address3!=0)
		{
			word=(*cNode).word3;
			fprintf(fdOb,"%d%d ",0,(*cNode).address3);
			decToHex(fdOb,word);
			fprintf(fdOb," %c\n",(*cNode).are3);
		}
		cNode=(*cNode).next;
	}
	gNode=*gHead;
	while (gNode!=NULL)
	{
		
		if((*gNode).type=='D')
		{
			for(l=0;l<(*gNode).n;l++)
			{
				fprintf(fdOb,"%d%d ",0,(*gNode).address+l);
				decToHex(fdOb,(*gNode).dat[l]);
				fprintf(fdOb," %c\n",'A');
			}
		}
		else if ((*gNode).type=='S')
		{
			for(l=0;l<(*gNode).sLength+1;l++)
			{
				fprintf(fdOb,"%d%d ",0,(*gNode).address+l);
				decToHex(fdOb,(int)((*gNode).str[l]));
				fprintf(fdOb," %c\n",'A');
			}
		}
		gNode=(*gNode).next;
	}
	fclose(fdOb);
}
