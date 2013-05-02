#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "csv.h"
#include "SC.h"

struct fconverter fc;
static void _seq_not(struct probnum *a1,struct probnum *n);
static void _seq_direct_add(struct probnum *,struct probnum *,struct probnum *);
static void _seq_half_scale_add(struct probnum *a1,struct probnum *a2,
		struct probnum *s);
static void _seq_and(struct probnum *a1,struct probnum *a2,struct probnum *s);
static void _seq_multi(struct probnum *a1,int t);
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
	i=i/(4*range)+0.75;
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
	return (double)4*range*(_onesratio(s)-0.75);
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
	struct probnum t1,t2,t3,t4;
	assert(a1->len==a2->len);
	new_probnum(&t1);
	new_probnum(&t2);
	new_probnum(&t3);
	new_probnum(&t4);
	
	_seq_not(a1,&t1);
	_seq_not(a2,&t2);
	_seq_and(&t1,&t2,&t3);
	_seq_multi(&t3,3);
	_seq_and(a1,a2,&t4);
	_seq_direct_add(&t3,&t4,p);
	p->len=a1->len;
	p->num=a1->num*a2->num;
	seq_free(&t1);
	seq_free(&t2);
	seq_free(&t3);
	seq_free(&t4);

}

void add(struct probnum *a1,struct probnum *a2,struct probnum *s)
{
	assert(a1->len==a2->len);
	_seq_half_scale_add(a1,a2,s);
}	

void sub(struct probnum *a1,struct probnum *a2,struct probnum *s)
{
	assert(a1->len==a2->len);
	struct probnum t1;
	new_probnum(&t1);
	_seq_not(a2,&t1);
	_seq_half_scale_add(a1,&t1,s);
	seq_free(&t1);
}

void _seq_not(struct probnum *a1,struct probnum *n)
{
	if (n->seq!=NULL)
		seq_free(n);
	n->len=a1->len;
	n->seq=malloc(sizeof(int)*(n->len));
	n->num=0-a1->num;
	for(int i=0;i<n->len;i++)
		n->seq[i]=(a1->seq[i]==0);  //flip a1's every bits
}

static void _seq_and(struct probnum *a1,struct probnum *a2,struct probnum *s)
{
	if (s->seq!=NULL)
		seq_free(s);
	s->len=a1->len;
	s->seq=malloc(sizeof(int)*(a1->len));
	for(int i=0;i<a1->len;i++)
		s->seq[i]=(a1->seq[i])&(a2->seq[i]);
	

}


static void _seq_direct_add(struct probnum *a1,struct probnum *a2,struct probnum *s)
{
	int count=0;
	int i;
	if (s->seq!=NULL)
		seq_free(s);
	s->seq=malloc(sizeof(int)*(a1->len));
	s->len=a1->len;
	for(i=0;i<s->len;i++){
		if (a1->seq[i]==0 && a2->seq[i]==0){
			if (count){
				s->seq[i]=1;
				count--;
			}else
				s->seq[i]=0;
		}
		else if (a1->seq[i]==1 && a2->seq[i]==1){
			count++;
			s->seq[i]=1;
		}else 
			s->seq[i]=1;
	}	
}
static void _seq_half_scale_add(struct probnum *a1,struct probnum *a2,
		struct probnum *s)
{
	register int count=0;
	if (s->seq!=NULL)
		seq_free(s);
	s->seq=malloc(sizeof(int)*(a1->len));
	s->len=a1->len;
	for (int i=0;i<a1->len;i++){
		if(a1->seq[i]==a2->seq[i])
			s->seq[i]=a1->seq[i];
		else{
			s->seq[i]=count;
			count=(count+1)%2;
		}
	}

}
void seq_free(struct probnum *s)
{
	s->len=0;
	s->num=0;
	free(s->seq);
}
void new_probnum(struct probnum *s)
{
	s->len=0;
	s->seq=NULL;
	s->num=0.0;
}
static void _seq_multi(struct probnum *a1,int t)
{
	int count,i;
	for (i=0;i<a1->len;i++){
		if (a1->seq[i]) count+=t-1;
		else {
			if(count){
				count--;
				a1->seq[i]=1;
			}
		}
	}
	i--;
	while(count) {
		if (a1->seq[i]==0){
			a1->seq[i]=1;
			count--;
		}
		i--;
	}
}

