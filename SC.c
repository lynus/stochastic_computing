#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "csv.h"
#include "SC.h"

struct fconverter fc;
static void cb1(void *n,size_t len,void *data)
{
	double *p=fc.gold[fc.count];
	static int c=0;
	//printf("%s count:%d  c:%d  \n",(char *)n,fc.count,c);
	sscanf((const char *)n,"%lf",p+c);
	c++;
	if (c==1<<(fc.bit))
		c=0;
}

static void cb2(int c,void *data)
{
	fc.count++;
	fc.gold[fc.count]=malloc(sizeof(double)*(1<<fc.bit));
}

int init_fc(char *fname,int bit,int len)
{
	char buf[1024];
	FILE *fp;
	struct csv_parser p;
	size_t bytes_read;
	fc.bit=bit;
	fc.count=0;
	fc.len=len;
	fc.gold=malloc(sizeof(double*)*len);
	fc.gold[0]=malloc(sizeof(double)*(1<<bit));
	csv_init(&p,CSV_APPEND_NULL);
	if((fp=fopen(fname,"rb"))==NULL) {
		puts("goldcode file open faild\n");
		return -1;
	}
	while ((bytes_read=fread(buf,1,1024,fp))>0)
			if(csv_parse(&p,buf,bytes_read,cb1,cb2,NULL)!=bytes_read){
				fprintf(stderr,"Error while parsing file:%s\n",csv_strerror(csv_error(&p)));
				return(-1);
			}
	csv_fini(&p,cb1,cb2,NULL);
	fc.count=0;
	fclose(fp);
	csv_free(&p);
	return 0;
}


void fconv(double i,struct probnum *s,int range)
{
	int j;
	i=i/(2*range)+0.5;
	s->num=i;
	s->len=1<<fc.bit;
	/*
		ok,ok i know it's waste of memory.right now i want keep algrithm as
		simple as possible.maybe later i will reimplement this part using 
		bitwise operation.
	 */
	s->seq=malloc(sizeof(int)*(1<<fc.bit));
	for (j=0;j<s->len;j++){
		if (i<=*(fc.gold[fc.count]+j))
				s->seq[j]=0;
		else
			s->seq[j]=1;
	}
	fc.count=(fc.count+1)%fc.len;
}
double bconv(struct probnum *s,int range)
{
	return (double)2*range*(_onesratio(s)-0.5);
}

double _onesratio(struct probnum *s)
{
	int sum=0,i;
	for (i=0;i<s->len;i++)
		sum+=s->seq[i];

	return (double)sum/(s->len);
}
void mul(struct probnum *a1,struct probnum *a2,struct probnum *p)
{
	int i;
	assert(a1->len==a2->len);
	p->seq=malloc(sizeof(int)*a1->len);
	for (i=0;i<a1->len;i++)
			p->seq[i]=(a1->seq[i])&(a2->seq[i]);
	p->len=a1->len;
	p->num=a1->num*a2->num;
}

void add(struct probnum *a1,struct probnum *a2,struct probnum *s)
{
	register int count=0;
	int i;
	assert(a1->len==a2->len);
	s->seq=malloc(sizeof(int)*a1->len);
	for (i=0;i<a1->len;i++){
		if(a1->seq[i]==a2->seq[i])
			s->seq[i]=a1->seq[i];
		else{
			s->seq[i]=count;
			count=(count+1)%2;
		}
	}
	s->len=a1->len;
	s->num=(a1->num+a2->num)/2;	// pz=(px+py)/2
}

void sub(struct probnum *a1,struct probnum *a2,struct probnum *s)
{
	int i;
	assert(a1->len==a2->len);
	for(i=0;i<a1->len;i++)
		a2->seq[i]=(a2->seq[i]==0); // a2= -a2
	a2->num=0-a2->num;
	add(a1,a2,s);
	for(i=0;i<a1->len;i++)
		a2->seq[i]=(a2->seq[i]==0);
	a2->num=0-a2->num;
}

