#include<stdio.h>

double distance(double ct[12], double cr[12]) //return Tohkura Distance(A Weighted Cepstral Distance) between the set of cepstral coefficients "ct" and "cr"
{
    int i,w[12] = {1,3,7,13,23,33,45,64,80,96,110,130};
    double D = 0 ;
    for(i=0; i<12; i++)
        D = w[i]*(ct[i]-cr[i])*(ct[i]-cr[i]) ;
    return D;
}

int main()
{
int i,j,k;

printf("\n------------------------------------------ INPUT -----------------------------\n");

int n;
printf("Input Text File => No. of Rows : ");scanf("%d",&n);

    double x[n][12] ;
    char filename[256];
    printf("Input Text File Name : ");
    scanf("%s",filename);
    printf("\n");

FILE *file;
file = fopen(filename,"r");

if(file==NULL)
    printf("file not found!");

for(i=0; i<n; i++)
{
    for(j=0; j<12; j++)
    {
        fscanf(file,"%lf ",&x[i][j]);
        //fprintf(output,"%lf ",x[i][j]);
    }//fprintf(output,"\n");
}
fclose(file);

FILE *output;
output = fopen("KMEANS_output.txt","w");

fprintf(output,"\n----------------------------------- K-Means Algorithm Implementation -------------------\n");
printf("\nNo. of Means(k) : ");scanf("%d",&k);
double y[k][12];
for(i=0; i<k; i++)
{
    fprintf(output,"\nInitial Mean Y[%d] = ( ",j);
    for(j=0; j<12; j++)
    {
        y[i][j] = x[(int)(i+0.5)*n/k][j];
        //scanf("%f",&y[i][j]);
        fprintf(output,"%f",y[i][j]);
    }fprintf(output,")\n");
}
fprintf(output,")\n");

//double thresold; fprintf(output,"\nThresold : ");scanf("%f",&thresold);

int t, p[k], c[k][n];
double sum,min, min_distortion, overall_distortion=0;

do{

//fprintf(output,"\n---------------------------------------- making  clusters -----------------------------\n");
    for(j=0; j<k; j++)
    {
        p[j]=0;
        //for(i=0; i<n; i++)
        //c[j][i]=0;
    }
    min_distortion = overall_distortion;
    overall_distortion = 0;

    for(i=0; i<n; i++)
    {
        min = distance(x[i],y[0]);
        t = 0;
        for(j=1; j<k; j++)
            if(min>distance(x[i],y[j]))
                {
                    t = j;
                    min = distance(x[i],y[j]);
                }

        overall_distortion += min;
        c[t][p[t]] = i;
        //fprintf(output,"\nc[%d][%d] : X[%d]",t,p[t],c[t][p[t]]);
        p[t] = p[t] + 1;
    //fprintf(output,"\n%d : %f : %f",t,min,overall_distortion);

    }
    fprintf(output,"\nOverall Distortion : %f\n",overall_distortion);

//fprintf(output,"\n---------------------------------------------- updating means -------------------------\n");
    for(j=0; j<k; j++)
    {
        //fprintf(output,"\nMean Y[%d] = ( ",j);
        for(i=0; i<12; i++)
        {
            sum=0;
            for(t=0; t<p[j]; t++)
            sum += x[c[j][t]][i];

            y[j][i] = sum/p[j];
            //fprintf(output,"%f",y[j][i]);
        }//fprintf(output,")\n");
    }

}while(overall_distortion != min_distortion);

fprintf(output,"\n------------------------------------------------ OUTPUT -------------------------------\n");
fprintf(output,"\n");

for(j=0; j<k; j++)
{
    fprintf(output,"\nRegion C[%d] : ",j);
    for(t=0; t<p[j]; t++)
        fprintf(output,"X[%d] ",c[j][t]);

    fprintf(output,"\nMean Y[%d] = ( ",j);
    for(i=0; i<12; i++)
        fprintf(output,"%f ",y[j][i]);

    fprintf(output,")\n");

}

//------------------------------------------------- THE END -------------------------------------------------
fclose(output);
return 0;
}
