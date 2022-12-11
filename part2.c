#include "assembler.h"
enum flag partTwo(FILE *fd,char *buffer,char *sRead,struct symbol **sHead,struct symbol **sTail,struct command **cHead,struct command **cTail,struct guidance **gHead,
               struct guidance **gTail,struct entry **eHead,struct entry **eTail,struct external **xHead,struct external **xTail,struct operation op[],int *IC,int *DC)
{
	int j,k,l;
	int line=0;
	int countLabel=0;
	enum flag found=NO,branch=NO,err=NO;
	enum flag error2=NO;
	int countS=0;
	char *save[NSW]={"r0","r1","r2","r3","r4","r5","r6","r7","mov","cmp","add","sub","lea","clr","not"
					,"inc ","dec","jmp","bne","jsr","red","prn","rts","stop"};	
	
	struct symbol *sNode=NULL;
	struct command *cNode=NULL;
	struct entry *eNode=NULL;
	struct external *xNode=NULL;
	
	if(!(fd=fopen(sRead,"r")))
	{
		fprintf(stderr,"\ncannot open file\n");
		exit(0);
	}
	while(fgets(buffer,MAX+1,fd)!=NULL) /* 1 */
	{ 
		line++;
		branch=NO;
		found=NO;
		j=0;
		if(err==YES)
			error2=YES;		
		err=NO;	
		
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
			
			if((strncmp(buffer+j,"string",6)!=0) && (strncmp(buffer+j,"data",4)!=0) && (strncmp(buffer+j,"extern",4)!=0)) /* 3 */
			{
				if(strncmp(buffer+j,"entry",5)==0) /* 4 */
				{
					j+=5;
					while(isspace(buffer[j]))
						j++;		
					sNode=*sHead;
					countS=0;
					
					while(!isspace(buffer[j+countS])) /* count the length of the symbol */
						countS++;
					while(sNode!=NULL)
					{
						if(strncmp(buffer+j,(*sNode).name,countS)==0) /* 5 */
						{
							(*sNode).attribute2=malloc(5);
							strncpy((*sNode).attribute2,"entry",5);
							eNode=(struct entry *)malloc(sizeof(struct entry)); /* add the symbol to the entry table */
							(*eNode).name=calloc(sizeof((*sNode).name),1);
							(*eNode).address=(*sNode).address;
							strncpy((*eNode).name,(*sNode).name,strlen((*sNode).name));
							(*eNode).next=NULL;
							if(*eTail==NULL)
								*eHead=*eTail=eNode;
							else
							{	
								(*eTail)->next=eNode;
								*eTail=eNode;
							}	
							break;
						}
						sNode=(*sNode).next;
					}
					if(sNode==NULL)
					{
						printf("%s line %d: label has not been intialized\n",sRead,line);
						err=YES;
						continue;
					}
				}
			}
		}
		else /* 6 */
		{
			for(k=0;k<15;k++)
			{
				if(strncmp(buffer+j,op[k].name,3)==0)
				{
					j+=3;
					break;
				}
			}
			if(k==15)
			{
				if(strncmp(buffer+j,op[k].name,4)==0)
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
					
					cNode=*cHead;
					while(found==NO && cNode!=NULL)
					{
						if((*cNode).line==line)
						{
							sNode=*sHead;
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
											(*xNode).name=calloc(sizeof((*sNode).name),1);
											strncpy((*xNode).name,(*sNode).name,strlen((*sNode).name));
											(*xNode).address=(*cNode).address1+1;
											(*xNode).next=NULL;
											if(*xTail==NULL)	
												*xHead=*xTail=xNode;
											else
											{	
												(*xTail)->next=xNode;
												*xTail=xNode;
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
								printf("%s: line %d: label does not exist\n",sRead,line);
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
				
				/*(*cNode).are2='A';*/
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
						cNode=*cHead;
						while(found==NO && cNode!=NULL)
						{
							
							if((*cNode).line==line)
							{
								sNode=*sHead;
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
												(*xNode).name=calloc(sizeof((*sNode).name),1);
												strncpy((*xNode).name,(*sNode).name,strlen((*sNode).name));
												(*xNode).address=(*cNode).address1+2;
												(*xNode).next=NULL;
												if(*xTail==NULL)
													*xHead=*xTail=xNode;
												else
												{	
													(*xTail)->next=xNode;
													*xTail=xNode;
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
									printf("%s: line %d: label does not exist\n",sRead,line);
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
	fclose(fd);
	return error2;
}
