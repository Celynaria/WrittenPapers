#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

int sizepop;

#define MAXINPUT  100 /* max no. of input units */
#define MAXHIDDEN 50 /* max no. of hidden units */
#define MAXOUTPUT  3  /* max no. of output units */
#define MAXFITOBJS 10  /* max no. of objectives */
#define MAXPOPSIZE 501 /* mas size for EA population */
#define MAXPERF 10     /* max no. of NN performance measures */
#define MAXDATA 9000   /* no. of lines of data in a dataset */
#define MAXGENES 200  /* bumber of weights */

double CROSS_RATE = 0.7;

double trainpc, testpc, trainmse, testmse;

int numgenes;

/* these hold the filenames for training (tr), test (te) and
validation (v) cases, both positive (p) and negative (n) */
char tr[30], te[30], rout[30], nnfile[30], nnoutfile[30];

double bsftest = 0;
double bsfval =  0;
double mse;


double min[MAXINPUT], max[MAXINPUT];

double parent[MAXGENES], parent1[MAXGENES], parent2[MAXGENES], child[MAXGENES];

/* these are the pointers to the above files */
FILE *tr_p, *te_p, *va_p, *rout_p, *nn_out, *nnfile_p;

/* these are the numbers of lines of data in each file */
int tr_n, te_n; 

/* this structure holds an entire dataset */
typedef struct ds {
  double data[MAXDATA][MAXINPUT];  /* the data themselves */
  int nfields; /* no. of fields */
  int ndata; /* no. of lines of data */
  double out[MAXDATA][MAXOUTPUT]; /* for a given NN, this holds the ouput for
                       /* each corresponding line of data */
} DS;

/* the datasets that will be read in */
DS tr_d, te_d ;


/* this is a neural network node */
typedef struct node {
  double inweights[MAXINPUT + 1];  /* the extra weight is the bias weight */
  double output;  /* stores the output after squashing */
} NODE;

/* a neural network. This is what we evolve a population of */
typedef struct nn {
  NODE hiddenlayer[MAXHIDDEN];  /* the hidden layer */
  NODE outputlayer[MAXOUTPUT];  /* the output layer */
  double fitnessvector[MAXFITOBJS]; /* allow for a certain number of measures of fitness */
  double performance[MAXPERF];         /* allow for various integer performance indicators */
  int ni, no, nh;  /* number of inputs, no. of output units, no. of hidden units */
} NN;

/* the array that holds the EA's population of NNs */
NN nn_pop[MAXPOPSIZE+1];

NN *bestnnp;

/* this function reads data in from a file into a dataset
It takes in the no. of lines and no. of fields */
void readin(char *, DS *, int, int);

/* used for skipping stuff when reading the params file */
void skip(FILE *, char *);

/* this works out the logistic function of the input */
double logistic(double);

/* this does binary tournament selection (or maybe something
else, see the function itself) from the nn_pop, return index
in nnpop of the selected parent */
int select_nn(int);

/* this will copy the NN with the input index into mutant, mutate
and evaluate the mutant */
void mutate_nn(NN *, int);

/* this will decide whether or not the mutant should go into
the population, and put it there if so, replacing something */
void replace_nn(int);

/* this copys the first nn into the second */
void copy_nn(NN *, NN *);



/* this will scan the current population and print out
what's happening progresswise */
void report_best(int, int);

int arg; /* this keeps track of the current arg being read in */
long int seed; /* random number seed */

double alpha; /* the alpha parameter for line and box crossover */

double drand(void); /* a uniform random number in (0, 1) */

double grand(void); /* a gaussian random number with mean 0 and std 1 */

/* the function that does one forward pass of one line of data through an NN */
double forward(NN *, DS *, int);


int ntrials =  10;

int state = 0;  /* for debugging */
/* the main function reads in all the data and sets
everything off */

double bestvalidation = 100000.0;

int nout;

void  main(int argc, char **argv)
{
  int i, j, k, trial, popsize, nin, nhid, niter;
FILE *params;
char paramfile[50], instring[50];
double dd;

 double RESULTS[100];

strcpy(paramfile,argv[1]);
params = fopen(paramfile,"r");

skip(params,instring); seed = atoi(instring);
skip(params,instring); popsize = atoi(instring);
skip(params,instring); nin = atoi(instring);
skip(params,instring); nhid = atoi(instring);
skip(params,instring); nout = atoi(instring);
skip(params,instring); niter = atoi(instring);
skip(params,instring); alpha = atof(instring);
skip(params,instring); strcpy(tr,instring);
skip(params,instring); tr_n = atoi(instring);
skip(params,instring); strcpy(te,instring);
skip(params,instring); te_n = atoi(instring);
skip(params,instring); strcpy(rout,instring);

 sizepop = popsize;


fclose(params); /* we no longer need the params file to be open */


/* a bit of test code */
printf("%d %d %d", seed, popsize, niter);

srand(seed);      /* seed the random number generator */

readin(tr, &tr_d, tr_n, nin+nout); /* read the trp data into the trp dataset */
readin(te, &te_d, te_n, nin+nout); /* read the tep data into the trp dataset */

 // normalise(&tr_d, &te_d);



printf("%g %g %d\n",
          tr_d.data[0][0], tr_d.data[1][0], tr_d.ndata);



initpop(popsize, nin, nhid, nout);


/* now evolve the NNs */


evolve_nns(popsize, nin, nhid, nout, niter, &tr_d, &te_d);

}
   


int evolve_nns(int npop, int in, int hid, int out, int gens, DS *tp, DS *ep)
{
int i, j, k;



/* first we have to evaluate everything in the initial pop */
for(i=0;i<=npop;i++)
{ eval_nn(&nn_pop[i], tp, ep);
  }

for(i=0;i<gens;i++)
 {
   if(drand() < CROSS_RATE)
     {
   j = select_nn(npop);
   k = select_nn(npop);
   crossover(&nn_pop[j], &nn_pop[k], npop);
     }
   else 
     {
   j = select_nn(npop);
   mutate_nn(&nn_pop[j], npop);
     }

   eval_nn(&nn_pop[npop], tp, ep);
   replace_nn(npop);
   if((i%1000)==0)report_best(npop, i);
 }

 print_nn(bestnnp);

}

int print_nn(NN *nnp)
{
 int i, j, k;
 
 nn_out = fopen(rout,"w");


 for(i=0;i<nnp->nh;i++)
    { 
      for(j=0;j<nnp->ni;j++)
	fprintf(nn_out,"input %d to hidden %d:  %g\n", j, i, nnp->hiddenlayer[i].inweights[j]);
      fprintf(nn_out,"bias for hidden %d: %g\n", i, nnp->hiddenlayer[i].inweights[nnp->ni]);

   }
 for(i=0;i<nnp->no;i++)
    { 
      for(j=0; j<nnp->nh; j++)
	fprintf(nn_out,"hidden %d to output %d:  %g\n",j, i, nnp->outputlayer[i].inweights[j]);
      fprintf(nn_out,"bias for output %d: %g\n", i, nnp->outputlayer[i].inweights[nnp->nh]);
    }

 fclose(nn_out);
}


/* generate a pop of random NNs */

int initpop(int size, int ni, int nh, int no)
{
int i, j, k;
NN *nnp;
NODE *np;
 char instring[50];


 /* count the weights */

 k = 0;
 for(i=0;i<nh; i++)
   for(j=0;j<(ni + 1); j++)
     k++;
 for(i=0;i<no; i++)
   for(j=0;j<(nh + 1); j++)
     k++;

 numgenes = k;

 if(numgenes > MAXGENES)
   {printf("Maxinmum allowed number of weights is %d, but your networks have %d\n", MAXGENES, numgenes);
     printf("Perhaps reduce the number of hidden layer nodes. \n");
     exit(0);
   }
 else {printf("Your networks have a total of %d weights\n\n", numgenes);
 }

for(i=0;i<=size; i++)
{
  nnp = &nn_pop[i];
  nnp->ni = ni;
  nnp->nh = nh;
  nnp->no = no;



  /* initialise the hiddens */
  for(j=0;j<nh;j++)
  {

    np = &nnp->hiddenlayer[j];
    for(k=0;k<(ni+1); k++)
      np->inweights[k] = (2.0*drand()) - 1.0;;

  }
  /* initialise the outs */


  
 for(j=0;j<no;j++)
  {

    np = &nnp->outputlayer[j];
    for(k=0;k<(nh+1); k++)
      np->inweights[k] = (2.0*drand())-1.0;
  }

 }
}

double drand()
{
  double dr;
  int r;

  r = rand();
  dr = (double)(r)/(double)(RAND_MAX);
  return(dr);
}

double grand(void)
{ double x1, x2, y1, pi;

 pi = acos(-1.0);

//	 y1 = sqrt( - 2 ln(x1) ) cos( 2 pi x2 )
//         y2 = sqrt( - 2 ln(x1) ) sin( 2 pi x2 )

x1 = drand();
 x2 = drand();
 y1 = sqrt( -2.0 * log(x1)) * cos(2.0 * pi * x2);
return y1;
 }


/* dummy before I write it */

int eval_nn(NN *nnp, DS *tp, DS *ep)
{
  int i, j, k, zz, trial;
  NODE *np;
  double thr, inc, val, tmp, meant, meanv, mse;
  int highest, desired, correct;

  meant = 100000;
  meanv = 100000;

  /* run each line of data through each dataset */

  for(i=0;i<tp->ndata;i++) forward(nnp,tp,i);
  for(i=0;i<ep->ndata;i++) forward(nnp,ep,i);

  /* the outputs are recorded in all the respective datasets */

  // get accuracy metrics on the training set
   
  mse = 0.0; correct = 0;
   for(i=0;i<tp->ndata;i++) 
     {
       for(j = 0; j < nout;j++)
	 {  val = tp->out[i][j] -  tp->data[i][(tp->nfields-nout)+j];  
         val *= val;  mse += val;
	 }
       //  what is the highest output?
       highest = 0; if(tp->out[i][1] > tp->out[i][0]) highest = 1;  if(tp->out[i][2] > tp->out[i][highest]) highest = 2;
       //  what is the desired output?
       desired = 0; if(tp->data[i][(tp->nfields-nout)+1] > tp->data[i][(tp->nfields-nout)+0]) desired = 1;
           if(tp->data[i][(tp->nfields-nout)+2] > tp->data[i][(tp->nfields-nout)+desired]) desired = 2;
	   if(desired==highest) correct++;
     }

   mse /= tp->ndata;  mse /= 3.0; nnp->performance[0] = mse;
   nnp->performance[2] = 100.0 * (double)(correct)/(double)(tp->ndata);

   mse = 0.0; correct = 0;
   for(i=0;i<ep->ndata;i++)
     {
       for(j = 0; j <nout; j++)
	 {
	   val = ep->out[i][j] -  ep->data[i][(ep->nfields-nout)+j];  
         val *= val; ; mse += val;
	 }
       //  what is the highest output?
       highest = 0; if(ep->out[i][1] > ep->out[i][0]) highest = 1;  if(ep->out[i][2] > ep->out[i][highest]) highest = 2;
       //  what is the desired output?
       desired = 0; if(ep->data[i][(ep->nfields-nout)+1] > ep->data[i][(ep->nfields-nout)+0]) desired = 1;
           if(ep->data[i][(ep->nfields-nout)+2] > ep->data[i][(ep->nfields-nout)+desired]) desired = 2;
	   if(desired==highest) correct++;

     }
       mse /= ep->ndata;  mse /= 3.0; nnp->performance[1] = mse;
   nnp->performance[3] = 100.0 * (double)(correct)/(double)(ep->ndata);

   nnp->performance[4] = nnp->performance[2] - nnp->performance[0];
   if(nnp->performance[4] > bsftest) 
         {bsftest = nnp->performance[4]; testpc = nnp->performance[3]; 
	   trainmse =  nnp->performance[0]; trainpc = nnp->performance[2];
           testmse =  nnp->performance[1]; 
   printf("new best: train mse(acc) %g (%g\%) - test mse(acc) %g (%g\%) - fitness %g \n", nnp->performance[0], 
	  nnp->performance[2], nnp->performance[1], nnp->performance[3], nnp->performance[4]);
   bestnnp = nnp;
	 }
}




/* this will give a forward pass through the network for one line of data */
/* where desired output is des */
/* there is built in assumption of one output node */

double forward(NN *nnp, DS *d, int line)
{
  int i, j, k, fditer;
  NODE *np;
  double wsum;
  double wsumbest;

  /* we have to first take the inputs and feed them through the
    hidden layer.  So, each hidden unit adds its weighted inputs
    and sends the result to the output unit, which then does a
    logistic squash of it */

  for(i=0;i<nnp->nh;i++)
     {  np = &nnp->hiddenlayer[i];   /* get pointer to hidden node i */

        wsum = 0.0;                  /* init weighted sum to zero */
        
            for(j=0;j<nnp->ni;j++)     /* add weighted inputs */
                   wsum += (np->inweights[j] *  d->data[line][j]);
 
        wsum += np->inweights[nnp->ni];  /* add the bias input */
	  
	//        np->output = logistic(wsum);  /* squash to get output of this node */
	np->output = logistic(wsum);

 /*if(line==1) {printf("1  %g\n", np->output); getchar();    }*/
     }

 /* now take the output units -- we calculate their weighted inputs
    (which come from the hiddens' outputs) */

 for(i=0;i<nnp->no;i++)
      { np = &nnp->outputlayer[i];   /* get pointer to output node */

        wsum = 0.0;                  /* init weighted sum to zero */

        for(j=0;j<nnp->nh; j++)      /* add weighted inputs from each h */
          wsum += (np->inweights[j] * nnp->hiddenlayer[j].output);

        wsum += np->inweights[nnp->nh];  /* add the bias input */

	//        np->output = logistic(wsum);  /* squash to get its output */
	np->output = logistic(wsum);

        /* store the output in the dataset structure */
      d->out[line][i] = nnp->outputlayer[i].output;

      }




 /* return the output from the 1st output node */

 /*printf("%g\n", nnp->outputlayer[0].output);  */

return(nnp->outputlayer[0].output);

}





/* binary tournament selection based on specificity at 80% sensitivity */
int select_nn(int n)
{
  int i, j, k;

  i = rand()%n;
  j = rand()%n;

  if(nn_pop[j].performance[4] > nn_pop[i].performance[4]) {i = j;}


 return i;
}


int crossover(NN *p1, NN *p2, int n)
{

int i, j, k;
NN *mnn;

 mnn = &nn_pop[n]; // mutant is here

 copy_nn(p1,mnn);  // it starts off as a copy of parent1

 write_string(p1, parent1); // the string of numgenes genes is now in the parent1[] array
 write_string(p2, parent2); // the string of numgenes genes is now in the parent2[] array

 COURSEWORK_CROSSOVER();

 write_back(child, mnn);

}
/* mutate until I write it */
void mutate_nn(NN *nnp, int n)
{
int i, j, k;
NN *mnn;

 mnn = &nn_pop[n]; // mutant is here

 copy_nn(nnp,mnn);  // it starts off as a copy of the parent

 write_string(mnn, parent); // the string of numgenes genes is now in the parent[] array

 COURSEWORK_MUTATE();

 write_back(child, mnn);
}


int write_string(NN *this, double *p)
{

  int i, j, ind;

  ind = 0;

  for(i=0;i<this->nh; i++)
    for(j=0;j < this->ni+1; j++)
      p[ind++] = this->hiddenlayer[i].inweights[j]; 

for(i=0;i<this->no;i++)
  for(j=0;j<(this->nh+1);j++)
    p[ind++] = this->outputlayer[i].inweights[j];

}


int write_back(double *p, NN *this)
{

  int i, j, ind;

  ind = 0;

  for(i=0;i<this->nh; i++)
    for(j=0;j < this->ni+1; j++)
      this->hiddenlayer[i].inweights[j] = p[ind++];

for(i=0;i<this->no;i++)
  for(j=0;j<(this->nh+1);j++)
    this->outputlayer[i].inweights[j] = p[ind++];

}




void copy_nn(NN *f, NN *t)
{
int i, j, k;
NODE *fn, *tn;

t->ni = f->ni;
t->no = f->no;
t->nh = f->nh;

for(i=0;i<MAXPERF;i++) t->performance[i] = f->performance[i];
for(i=0;i<MAXFITOBJS;i++) t->fitnessvector[i] = f->fitnessvector[i];

for(i=0;i<f->nh;i++)
 { fn = &f->hiddenlayer[i];
   tn = &t->hiddenlayer[i];
   for(j=0;j<(f->ni + 1) ;j++)
      tn->inweights[j] = fn->inweights[j];
   tn->output = fn->output;
 }

for(i=0;i<f->no;i++)
 {
  fn = &f->outputlayer[i];
  tn = &t->outputlayer[i];
  for(j=0;j<(f->nh+1);j++)
     tn->inweights[j] = fn->inweights[j];
  tn->output = fn->output;
 }

}

/* dummy replace function */
void replace_nn(int n)
{
int i, j;
double w;

/* find the worst in the pop */

w = nn_pop[0].performance[4];
j = 0;

for(i=1;i<n;i++)
{if (nn_pop[i].performance[4] < w)
   {w = nn_pop[i].performance[4]; j = i;}
}
if(nn_pop[n].performance[4] >= w) {copy_nn(&nn_pop[n],&nn_pop[j]);}

}

double logistic(double w)
{
  double x;

 x = 1.0 / (1.0 + exp((-1.0 * w))) ;

 return (x);
 }

/* read in n lines of data (c columns) into array a from file f */
void readin(char *fname, DS *dsp, int nlines, int ncols)
{
  int i, j, k;
  char instring[20];
  FILE *fp;
  double nsum, lowest;

  dsp->nfields = ncols;
  dsp->ndata = nlines;

  fp = fopen(fname,"r");


  for(i = 0; i < nlines; i++)
    {

     for(j = 0; j < ncols; j++)
       {fscanf(fp,"%s", instring);
        dsp->data[i][j] = atof(instring);
       }

     /* now normalise this column */
     /*     nsum = 0;
     lowest = 100000000.0;

     for(j = 0; j < ncols; j++)
	 if(dsp->data[i][j] <lowest) lowest = dsp->data[i][j];

     for(j = 0; j < ncols; j++)        dsp->data[i][j] -= lowest;

     for(j = 0; j < ncols; j++)     nsum += dsp->data[i][j];

     for(j = 0; j < ncols; j++)    dsp->data[i][j] /= nsum;
     */

    }
fclose(fp);
}

int normalise(DS *trdata, DS *tedata)
{

  double v;
  int i, j;

  for(i=0;i<trdata->nfields; i++)
    {
      min[i] = trdata->data[0][i]; 
      max[i] = trdata->data[0][i]; 

      for(j=0;j<trdata->ndata;j++)
	{
          if(trdata->data[j][i] < min[i]) min[i] = trdata->data[j][i];
          if(trdata->data[j][i] > max[i]) max[i] = trdata->data[j][i];
	}
    }

  for(i=0;i<tedata->nfields; i++)
    {
      for(j=0;j<tedata->ndata;j++)
	{
          if(tedata->data[j][i] < min[i]) min[i] = tedata->data[j][i];
          if(tedata->data[j][i] > max[i]) max[i] = tedata->data[j][i];
	}
    }

  //  now we have a min and a max for each field.
  // normalie all data to 0 1
 

  for(i=0;i<trdata->nfields; i++)
    {

      for(j=0;j<trdata->ndata;j++)
	{
	  v = trdata->data[j][i];
          trdata->data[j][i] = (v - min[i] ) / (max[i] - min[i]);
	}

    }

  for(i=0;i<tedata->nfields; i++)
    {
      for(j=0;j<tedata->ndata;j++)
	{

	  v = tedata->data[j][i];
          tedata->data[j][i] = (v - min[i] ) / (max[i] - min[i]);
	}

    }


  // and do vald

}

    
/* dummy report function until I write it */
void report_best(int n, int g)
{
int i, j;
double b;

/* find the best in the pop */



printf("best at %d evals: training: %g percent - test:  %g percent - fitness %g\n",
g+sizepop,
       trainpc, testpc, bsftest);

}

/* this function is to scan past the text bit in the params
file and get to the number */
void skip(FILE *f, char *s)
{
  fscanf(f,"%s",s); fscanf(f,"%s", s);
}


int nbp(int n)
{
  printf("got here %d\n", n);
  fflush(stdout);
}






/* 
    YOU WILL WRITE THIS FUNCTION

    numgenes  is the number of genes
   the array 'child' contains numgenes real numbers. 
   Mutate it!

   Change this code to achieve the Gaussian mutation mentioned in the coursework

  Currently it simply replaces two randomly chosen  genes with a uniform random new number between
  -1 and 1.
 */

int COURSEWORK_MUTATE (void)
{

  int i, g;
  double gaussian_rand_not_actually_used;


  // first make the child a copy of the parent
  for(i=0;i<numgenes;i++) child[i] = parent[i];


  // showing how to generate and use a gaussian random number with mean 0 and standard deviation 1:

  gaussian_rand_not_actually_used = grand();

  // now mutate the child
  // this sill example simply adds a UNIFORM random number between 2 and 3 to gene 25


  child[25] += (2.0 + drand());

    
}

/*   YOU WILL WRITE THIS FUNCTION

   numgenes  is the number of genes
   the arrays 'parent1' and 'parent2' contain numgenes real numbers. 


   Change this code to achieve the crossover mentioned in the coursework


  the result of the crossover should be put in the child array

   the illustration below does 1-point crossover

   the parameter alpha is defined. E.g. you could set it here with "alpha = 0.1" or something.
*/

int COURSEWORK_CROSSOVER (void)
{

  int i, j, g, cp;
  double silly_parameter, u;
  

  // first make the child a copy of the first parent
  // you don't actually need to do this. This is just for illustration
  // note that if you don't do this, the child[] array will probably
  // be all zeroes to start with
  for(i=0;i<numgenes;i++) child[i] = parent1[i];

  // illustrating making use of alpha and a uniform random number.
  // this makes silly_parameter into alpha + a uniform random number between 10 and 12.5

  silly_parameter = alpha +  10  + 2.5 * drand(); 

  // now make child[] a box or line crossover of parent1 and parent2
  // this illustration just makes the child a one-point crossover of the parents, 
  //  and adds a uniform random number between -1 and 1 to a random gene;
 
      u = -1.0 +   2.0 * drand();

      cp = rand()%numgenes;

  for(i=0;i<cp; i++)  
    {
      child[i] = parent1[i];
    }

  for(i=cp; i < numgenes; i++)  
    {
      child[i] = parent2[i];
    }

  child[rand()%numgenes] += u;


}

  
 
