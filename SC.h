struct probnum {
	int len;	//length of sequence
	int *seq;	//stochastic seq
	double num;	//the number that the seq represent
};

struct fconverter {
	int bit;	
	int count;
	double **gold;
	int len;	//the number of rows of gold table
};

void fconv(double i,struct probnum *s,int range); //forward converter
double bconv(struct probnum *s,int range); //backward converter

void add(struct probnum *a1,struct probnum *a2,struct probnum *s);
void mul(struct probnum *a1,struct probnum *a2,struct probnum *p);
void sub(struct probnum *a1,struct probnum *a2,struct probnum *p);
void not(struct probnum *a1,struct probnum *n);

int init_fc(char *fname,int bit,int len);	//init fconverter struct

double _onesratio(struct probnum *s);	//calculate one's ratio in given probnum
void seq_free(struct probnum *a);
