
/*  a program for evolving rulesets 

you may use it as long as you acknowledge me:   David Corne  dwcorne@gmail.com
in the datasets,  assumes 
  - space separated
  - last field is target class field
  - if nclasses = N, then the class values in the last field are 0, 1, ..., N-1

 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define MAXPOP 100
#define MAXFIELDS 200
#define MAXROWS 10000
#define MAXNAME 100
#define MAXRULES 500


double drandin(double low, double high);

FILE *trainfile, *testfile;

long seed;

int nfields, nrows_train, nrows_test, nclasses, iterations;

int popsize = 20;

int rulesperclass = 3;
int intervalsperrule = 10;

typedef struct row {
  double field[MAXFIELDS];
} ROW;

typedef struct dataset {
  ROW rows[MAXROWS];
} DATASET;


DATASET train_data, test_data;



typedef struct interval {

  double low, high;
  int field;

} INTERVAL;

typedef struct rule {
  INTERVAL intervals[MAXFIELDS-1];
  int nintervals;
  int class;
} RULE;
 

typedef struct ruleset {

  int rulesperclass;

RULE rules[MAXRULES];

  double acc;

} RULESET;


RULESET population[MAXPOP+1];


int main(int argc,char **argv){

  int i, j, k;

  nfields = atoi(argv[1]);
  nrows_train = atoi(argv[2]);
  nrows_test = atoi(argv[3]);
  nclasses = atoi(argv[4]);
  trainfile = fopen(argv[5],"r");
  testfile = fopen(argv[6],"r");

  intervalsperrule = atoi(argv[7]);
  rulesperclass = atoi(argv[8]);
  iterations = atoi(argv[9]);

  randomize();

  read_data(trainfile, nrows_train, nfields, &train_data);

  read_data(testfile, nrows_test, nfields, &test_data);

  generate_pop();

  evolve();
  // for(i=0;i<10;i++)  printf("%g\n", drand48());
 
}

int read_data(FILE *f, int rows, int fields, DATASET *data)
{

  int i, j, iin;
  float fin;

  for(i=0;i<rows; i++)
    {
      for(j = 0; j < fields; j++)
	{
	  fscanf(f,"%f", &fin);  data->rows[i].field[j] = (double)(fin);
	}
    }

  /*
  for(i=0;i<rows; i++)
    {
      for(j = 0; j < fields; j++)
	{
          printf("%g ", data->rows[i].field[j]);
	}
      printf("\n");
    }
  */

}


int getranddat(int c)
{
  int i, dat;

  dat = randint(0,nrows_train); 
  while ( fabs( train_data.rows[dat].field[64] - (double)(c)) > 0.1)
    {  dat = randint(0,nrows_train); }
    
  return(dat);
}

int generate_pop(void)
{

  int i, j, k, l, rnum, dat;
  RULESET *r;
  RULE *ru;
  INTERVAL *iv;

  for(i=0;i<popsize;i++)
    {
      r = &population[i];
      rnum = 0;
      for(j=0;j<nclasses; j++)
	for(k=0;k< rulesperclass; k++)
	  {ru = &r->rules[rnum++];
	    ru->nintervals = intervalsperrule;
            ru->class = j;
	    // get a random training sample of this class
            dat = getranddat(j);

            for(l=0;l<intervalsperrule; l++)
	      {  iv = &ru->intervals[l];
		iv->field = randint(0, 63);
		iv->low = ceil(drandin(0.0, train_data.rows[dat].field[iv->field]-1.0));
		iv->high = ceil(drandin(train_data.rows[dat].field[iv->field]-0.1, 16.0));
	      }
	  }
      
      evaluate(r);
      //      print_rule(r);
    }


}

int evolve (void)
{

  int i, j, k, s1, s2, worstinpop, bestinpop;

  for(i=0; i< iterations; i++)
    {

      // select a rule 

      s1 = randint(0, popsize);
      s2 = randint(0, popsize);

      if (population[s1].acc < population[s2].acc)  s1 = s2;

      copyrule(&population[s1], &population[popsize]);
      mutate(&population[popsize]);
      evaluate(&population[popsize]);

      worstinpop = getworst();
      if(population[popsize].acc >= population[worstinpop].acc)
	{copyrule(&population[popsize],&population[worstinpop]);}
      //	  printf("new best on training set: %g\%\n", 100.0* population[worstinpop].acc);}

  bestinpop = getbest();
  if(i%100==0)printf("best on training set at iteration %d:  %g\%\n", i,100.0* population[bestinpop].acc);

    }

  // finish up  -- test it?

  bestinpop = getbest();

  print_rule(&population[bestinpop]);

  test_evaluate(&population[bestinpop]);
}


int mutate (RULESET *r)
{

  int i, j, k, rr, ri, rd;
  INTERVAL *iv;

  // choose a random rule 

  rr = randint(0, nclasses*rulesperclass - 1);

  // choose a random interval 

  ri = randint(0, intervalsperrule-1); 

  iv = &r->rules[rr].intervals[ri];  

  rd = getranddat(r->rules[rr].class);

		iv->field = randint(0, 63);
		iv->low = ceil(drandin(0.0, train_data.rows[rd].field[iv->field]-1.0));
		iv->high = ceil(drandin(train_data.rows[rd].field[iv->field]-0.1, 16.0));
}

int getworst(void)
{
  int i, w;
  double wacc;
    
  w = 0;
  wacc = population[0].acc;

  for(i=1;i<popsize;i++)
    {  if(population[i].acc < wacc) { wacc = population[i].acc; w = i;}}

  return(w);
}


int getbest(void)
{
  int i, w;
  double wacc;
    
  w = 0;
  wacc = population[0].acc;

  for(i=1;i<popsize;i++)
    {  if(population[i].acc > wacc) { wacc = population[i].acc; w = i;}}

  return(w);
}

int copyrule(RULESET *from, RULESET *to)
{
  int i, j, k;
  RULE *r;
  INTERVAL *iv;

  for(i=0; i< rulesperclass * nclasses; i++)
    {
      r = &to->rules[i];
      for(j=0;j<intervalsperrule; j++)
	{ iv = &r->intervals[j];
  
          iv->low = from->rules[i].intervals[j].low;
          iv->high = from->rules[i].intervals[j].high;
          iv->field = from->rules[i].intervals[j].field;
	}
      r->class = from->rules[i].class;
    }
      to->acc = from->acc;
}

int print_rule(RULESET *r)
{
  int i, j;

  printf("\n");

  for(i=0;i<rulesperclass*nclasses;i++)
    {
      for(j=0;j<intervalsperrule;j++)
	printf(" %d-[%g  %g] ", r->rules[i].intervals[j].field,r->rules[i].intervals[j].low,r->rules[i].intervals[j].high);
      printf("-> %d\n", r->rules[i].class);
    }
  printf("ACCURACY on training set %g\%\n\n", 100.0* r->acc);
}

int evaluate(RULESET *r)
{

  int i, j, k, votes[10], nmi, winner;
  int corr;
  RULE *ru;

  corr = 0;
 
  for(i=0;i<nrows_train; i++)
    {
      // initialise class votes    
      for(j=0;j<nclasses;j++) votes[j] = 0;
 
      for(j = 0; j < rulesperclass*nclasses; j++)
	{
          ru = &r->rules[j];

          nmi = 0;  // initialise non-matching intervals;

          for(k=0;k<intervalsperrule;k++)
	    { if (ru->intervals[k].low >  train_data.rows[i].field[ru->intervals[k].field])
                nmi++;
		if(nmi>0) break;
                if (ru->intervals[k].high <  train_data.rows[i].field[ru->intervals[k].field])
		  nmi++;
		if(nmi>0) break;
	    }
          if(nmi==0) votes[ru->class]++;
	}
      winner = winclass(votes);
      if( fabs((double)(winner) -  train_data.rows[i].field[64]) < 0.1) corr++;
    }

  r->acc = (double)(corr)/(double)(nrows_train);
}



int test_evaluate(RULESET *r)
{

  int i, j, k, votes[10], nmi, winner;
  int corr;
  RULE *ru;

  corr = 0;
 
  for(i=0;i<nrows_test; i++)
    {
      // initialise class votes    
      for(j=0;j<nclasses;j++) votes[j] = 0;
 
      for(j = 0; j < rulesperclass*nclasses; j++)
	{
          ru = &r->rules[j];

          nmi = 0;  // initialise non-matching intervals;

          for(k=0;k<intervalsperrule;k++)
	    { if (ru->intervals[k].low >  test_data.rows[i].field[ru->intervals[k].field])
                nmi++;
                if (ru->intervals[k].high <  test_data.rows[i].field[ru->intervals[k].field])
		  nmi++;
	    }
          if(nmi==0) votes[ru->class]++;
	}
      winner = winclass(votes);
      if( fabs((double)(winner) -  test_data.rows[i].field[64]) < 0.1) corr++;
    }

  r->acc = (double)(corr)/(double)(nrows_test);

  printf("TEST ACCURACY %g\%\n", 100.0* r->acc);
}



int winclass(int *v)
{

  int wc, i, nwc, nnwc;

  wc = 0; nwc = v[0];

  for(i=1;i<nclasses; i++)
    {  if(v[i] > nwc) {nwc = v[i]; wc = i;}}

  nnwc = 0;
  for(i=0;i<nclasses; i++)
    if(v[i] == nwc) nnwc++;

  if(nnwc>1) return(-1);
  else return(wc);
}


double drandin(double low, double high)
{
  return (low + drand48()* (high - low));
}

int randint(int low, int high)
{
  int r;

 r =   low + (int)  (ceil (  ((double)(high - low))*drand48()));

  return(r);
}  
  
int randomize(void)
{

  int stime;
  long ltime;

  ltime = time(NULL);
  stime = (unsigned) ltime/2;
  srand48(stime);
}
 

int nbp(int here)
{printf("got here %d\n", here);
  fflush(stdout);
}

