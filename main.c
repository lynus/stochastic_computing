#include "SC.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

int main(int argc,char **argv)
{
	double j;
	init_fc("rn7.csv",7,64);
	FILE *fp1,*fp2,*fp3,*fp4;
	struct probnum a1;
	struct probnum a2;
	struct probnum p;
	new_probnum(&p);
	new_probnum(&a1);
	new_probnum(&a2);
	srandom(23);
	fp1=fopen("add_analysis","w");
	fp2=fopen("sub_analysis","w");
	fp3=fopen("mul_analysis","w");
	fp4=fopen("reprent_analysis","w");
	for (int i=0;i<1024;i++){
		double k;
		fconv(k=(double)random()/RAND_MAX,&a1,1);
		fconv(j=(double)random()/RAND_MAX,&a2,1);
	
		add(&a1,&a2,&p);
		fprintf(fp1,"%lf,\t%lf,\t%lf,\t%lf,\t%lf,\t%lf\n",
				k,j,k+j,bconv(&p,2),k+j-bconv(&p,2),
				fabs(k+j-bconv(&p,2))/(k+j)*100);


		sub(&a1,&a2,&p);
		fprintf(fp2,"%lf,\t%lf,\t%lf,\t%lf,\t%lf,\t%lf\n",
				k,j,k-j,bconv(&p,2),k-j-bconv(&p,2),
				fabs(k-j-bconv(&p,2))/fabs(k+j)*100);

		mul(&a1,&a2,&p);
		fprintf(fp3,"%lf,\t%lf,\t%lf,\t%lf,\t%lf,\t%lf\n",
				k,j,k*j,bconv(&p,1),k*j-bconv(&p,1),
				fabs(k*+j-bconv(&p,1))/(k*j)*100);
		fprintf(fp4,"%lf,\t%lf,\t%lf,\t%lf\n",
				k,bconv(&a1,1),k-bconv(&a1,1),
				fabs(k-bconv(&a1,1))/k*100);
	}
	fclose(fp1);fclose(fp2);fclose(fp3);
	fclose(fp4);
	return 0;

}
