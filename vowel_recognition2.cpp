#include<iostream>
#include<fstream>
#include<vector>
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<string.h>
#include<dirent.h> // for comparision (getcwd(buf, sizeof(buf)) != NULL)

using namespace std;

vector<int> s;
int size;
double C[12];

void input(double v[size]);
void dc_shift(double v[size]);
void normalisation(double v[size]);
int max_amplitude(double v[size]);
void calculation(double v[size],int a, int b);
double distance_measure(double ct[13], double cr[13]);

int main()
{
    char buf[BUFSIZ + 1];
  if (getcwd(buf, sizeof(buf)) != NULL)
	printf("%s\n", buf);
	int i, k;
//------------------------------------------------ input1 ---------------------------------------
input(s);

double x[size];
for(i=1; i<size; i++)
x[i] = s[i];

dc_shift(x);
normalisation(x);
k = max_amplitude(x);
calculation(x,k-160,k+160);

double c1[12];
for(i=1; i<=12; i++)
c1[i] = C[i];

//------------------------------------------------ input2 ---------------------------------------
input(s);

double y[size];
for(i=1; i<size; i++)
y[i] = s[i];

dc_shift(y);
normalisation(y);
k = max_amplitude(y);
calculation(x,k-160,k+160);

double c2[12];
for(i=1; i<=12; i++)
c2[i] = C[i];
//------------------------------------------------ result ---------------------------------------
double n = distance_measure(c1, c2);
printf("Distance : %f\n",n);

return 0;
}

void input(vector<double> v) // it takes only numbers from a textfile and store in an array v[]
{
    int i,j=0;

    char filename[256];
    printf("\nText File Name:");
    scanf("%s",filename);
//printf("%s",&filename);
    char *inname = filename;
    ifstream infile(inname);

    if (!infile)
    {
        cout << "\nThere is no " << inname << "here" << endl;
    }
    //cout << "Opened " << inname << " for reading." << endl;
    while (infile >> i)
    {
        v.push_back(i);
        //cout << "\nSample[" << j+1 << "] : " << v[j] << endl;
        j = j+1;
    }
    //printf("input function is executed");
    size = v.size();
}

void dc_shift(double v[size])  // it does DC Balancing of v[] and update v[]
{
    int i;
    double v_mean,temp = 0 ;
    printf("%d\n",size);
    for(i=0; i<size; i++)
    {
        temp += v[i];
        printf("%f:%f\n",v[i],temp);
    }
    v_mean = temp/size;
    printf("%d : %f\n",temp,v_mean);
    for(i=0; i<size; i++)
    {
        v[i] = v[i]-v_mean;
        printf("sample[%d] : %f\n",i,v[i]);
    }
}

void normalisation(double v[size]) // it does DC Balancing of v[] and update v[]
{
    int i, max_position;
    double max = 0;
    for(i=0; i<size; i++)
    {
        if(v[i]>0)
        {
            if(max<=v[i])
            {
                max = v[i];
                max_position= i;
            }
        }
        else
        {
            if(max<= -v[i])
            {
                max = -v[i];
                max_position= i;
            }
        }
    }
    for(i=0; i<size; i++)
    {
        v[i] = (1000*v[i])/max ;
    }
}

// here you have to write code to cut voiced region from given signal

int max_amplitude(double v[size]) // it return the position in v[] where sample value is maximum
{
    int i, max_position;
    double max = 0;
    for(i=0; i<size; i++)
    {
        if(max<=v[i])
        {
            max = v[i];
            max_position= i;
        }
    }
return max_position;
}


void calculation(double v[size],int a, int b)
{
    int i, j, N = 0 ,k ,m ;
/*----------------------------------------- Max_Amplitude_Frame Samples  s[N] ---------------------------------- */
    double s[320];
    for(i = a; i < b; i++)
    if((i >= 0)&&(i < size))
    {
        s[N] = v[i];
        N = N+1;
    }
/*----------------------------------------------- Hamming Window -----------------------------------------------*/
    double w[N], s_hammed[N];
    for(i=0; i<N; i++)
    {
        w[i] = 0.54 + 0.46*cos(6.2832*(i-N/2)/(N-1));
        s_hammed[i] = w[i]*s[i];
    }
/*----------------------------------------- Auto Co-relation Coeficient R[p+1] ---------------------------------- */

    printf("\nAuto Co-relation Coefficient: \n");
    int p = 12;
    double R[p+1];
    for(k=0; k<=p; k++)
    {
        R[k] = 0;
        for(m=0; m<N-k; m++)
        R[k] += s_hammed[m]*s_hammed[m+k];
        printf("R[%d] : %f\n",k,R[k]);
    }

/*----------------------------------------- LPC Coeficient A[p][p]---------------------------------- */

    double E[p+1], K[p], A[p][p];
    E[0] = R[0];
    for(i=1; i<=p; i++)
    {
        int temp = 0 ;
        for(j=1; j<i; j++)
        {
            temp += A[i-1][j]*R[i-j];
        }
        K[i] = (R[i]-temp)/E[i-1];
        A[i][i] = K[i];
        for(j=1; j<i; j++)
        {
            A[i][j] = A[i-1][j] - K[i]*A[i-1][i-j];
        }
        E[i] = (1-K[i]*K[i])*E[i-1];
    }
    printf("\nLPC Coeficient :\n");
    for(i=1; i<=p; i++)
    printf("a[%d] : %f\n",i,A[p][i]);

/*----------------------------------------- Cepstral Coeficient C[n]---------------------------------- */

    printf("\nCepstral Coeficient :\n");
    for(m=1; m<=p; m++)
    {
        C[m] = A[p][m];
        for(k=1; k<m; k++)
            C[m] += (k*C[k]*A[p][m-k])/m ;
        printf("C[%d] : %f\n",m,C[m]);
    }
}

double distance_measure(double ct[13], double cr[13]) // return tokhura distance of ct with cr
{
    int i,w[13] = {0,1,3,7,13,23,33,45,64,80,96,110,130};
    double D = 0 ;
    for(i=1; i<=12; i++)  // Q = 3p/2 = n = 12
        D = w[i]*(ct[i]-cr[i])*(ct[i]-cr[i]) ;
    return D;
}
