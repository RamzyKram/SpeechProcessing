#include<iostream>
#include<fstream>
#include<vector>
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<string.h>
#include<dirent.h> // for comparision (getcwd(buf, sizeof(buf)) != NULL)

using namespace std;

double C[12];
int size, start_frame_no, end_frame_no;

double min( double x, double y, double z );
double max( double x, double y, double z );
void input(double v[80000]);
void dc_shift(double v[80000]);
void normalisation(double v[80000]);
double Energy(double v[80000],int a, int b);
void voiced_region(double v[80000]);
void coefficient(double v[80000],int a, int b);
double distance_measure(double ct[12], double cr[12]);

int main()
{
    char buf[BUFSIZ + 1];
  if (getcwd(buf, sizeof(buf)) != NULL)
	printf("%s\n", buf);
//------------------------------------------------ input1 ---------------------------------------


double s1[80000];
input(s1);
dc_shift(s1);
normalisation(s1);
voiced_region(s1);
int i,k;

double ceps1[end_frame_no - start_frame_no + 1][12];
int j1=0;
for(i=start_frame_no; i<=end_frame_no; i++)
{
    coefficient(s1,i*80,i*80+320);
    for(k=1; k<=12; k++)
    ceps1[j1][k] = C[k];
    j1++;
}

//------------------------------------------------ input2 ---------------------------------------
double s2[80000];
input(s2);
dc_shift(s2);
normalisation(s2);
voiced_region(s2);

double ceps2[end_frame_no - start_frame_no + 1][12];
int j2=0;
for(i=start_frame_no; i<=end_frame_no; i++)
{
    coefficient(s2,i*80,i*80+320);
    for(k=1; k<=12; k++)
    ceps2[j2][k] = C[k];
    j2++;
}

printf("---------------------------------------------- local distance matrix ----------------------------\n");
double local_distance[j1][j2];
for(i=0; i<j1; i++)
{
    for(k=0; k<j2; k++)
    {
        local_distance[i][k] = distance_measure(ceps1[i],ceps2[k]);
        printf("%s\t",local_distance[i][k]);
    }
    printf("\n");
}
printf("----------------------------------------------- END ----------------------------------------------\n");



printf("---------------------------------------------- global distance matrix ----------------------------\n");
double global_distance[j1][j2];
    global_distance[0][0] = local_distance[0][0];
    printf("%s\t",global_distance[0][0]);
for(i=1; i<j1; i++)
    global_distance[i][0] = local_distance[i][0] + local_distance[i-1][0];
for(k=1; k<j2; k++)
    {
        global_distance[0][k] = local_distance[0][k] + local_distance[0][k-1];
        printf("%s\t",global_distance[0][k]);
    }
    printf("\n");
for(i=1; i<j1; i++)
{
    printf("%s\t",global_distance[i][0]);
    for(k=1; k<j2; k++)
    {
        global_distance[i][k] = local_distance[i][k] + min(global_distance[i-1][k],global_distance[i-1][k-1],global_distance[i][k-1]);
        printf("%s\t",global_distance[i][k]);
    }
    printf("\n");
}
printf("-------------------------------------------------- END -------------------------------------------\n");

int path_i[j1+j2-1],path_j[j1+j2-1],a=0;
path_i[0] = 0; path_j[0] = 0;
i=0;
int j=0;
while((i==j1)&&(k==j2))
{   a=a+1;
    if(global_distance[i+1][k] = max(global_distance[i+1][k],global_distance[i+1][k+1],global_distance[i][k+1]))
    {
        i=i+1;
        path_i[a]=i;
        path_j[a]=k;
    }
    if(global_distance[i+1][k+1] = max(global_distance[i+1][k],global_distance[i+1][k+1],global_distance[i][k+1]))
    {
        i=i+1;
        k=k+1;
        path_i[a]=i;
        path_j[a]=k;
    }
    if(global_distance[i][k+1] = max(global_distance[i+1][k],global_distance[i+1][k+1],global_distance[i][k+1]))
    {
        k=k+1;
        path_i[a]=i;
        path_j[a]=k;
    }

}
for(i=0; i<=a; i++)
printf("Optimal Warping Path : ");
printf("(%d,%d) ",path_i[i],path_j[i]);
printf("\nOptimal Warping Path Cost : %f\n", global_distance[j1][j2]);

return 0;
}

double min( double x, double y, double z )
{
    if( ( x <= y ) && ( x <= z ) ) return x;
    if( ( y <= x ) && ( y <= z ) ) return y;
    if( ( z <= x ) && ( z <= y ) ) return z;
}

double max( double x, double y, double z )
{
    if( ( x >= y ) && ( x >= z ) ) return x;
    if( ( y >= x ) && ( y >= z ) ) return y;
    if( ( z >= x ) && ( z >= y ) ) return z;
}

void input(double v[80000]) // it takes only numbers from a textfile and store in an array v[]
{
    int i,j=0;

    char filename[256];
    printf("\nSample Text File Name:");
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
        v[j] = i;
        //cout << "\nSample[" << j+1 << "] : " << v[j] << endl;
        j = j+1;
    }
    //printf("input function is executed");
    size = j;
}

void dc_shift(double v[80000])  // it does DC Balancing of v[] and update v[]
{
    int i;
    double v_mean = 0 ;
    //printf("%d\n",size);
    for(i=0; i<size; i++)
    {
        v_mean += v[i]/size;
        //printf("%f : %f\n",v[i],temp);
    }
    //v_mean = temp;
    //printf("%lf : %lf\n",temp,v_mean);
    for(i=0; i<size; i++)
    {
        v[i] = v[i]-v_mean;
       // printf("sample[%d] : %f\n",i,v[i]);
    }
}

void normalisation(double v[80000]) // it does DC Balancing of v[] and update v[]
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
        //printf("sample[%d] : %f\n",i,v[i]);
    }
}

double Energy(double v[80000],int a, int b) // return energy of frame[a,b] of v[]
{
    int i;
    double energy = 0 ;
    for(i=a; i<b; i++)
    {
        energy += v[i]*v[i] ;
    }
    //printf("%f\n",energy);
return energy;
}

void voiced_region(double v[80000])  // cut the voiced region and calculate start & end frame no. of voiced region
{
    int i, j=0, k, m, n, frames = size/80 - 3;//((size-240)/80) ;  //(size/80) < frames+4 <= (total_samples/80)+1
    double e[frames];
    for(i=0; i<(size-320); i = i+80)
    {
        e[j] = Energy(v,i,i+320);
        //printf("%d :E[%d]: %f\n",i,j,e[j]);
        //printf("total samples : %d\n", size);
        j = j+1;
    }
    double silence_energy = (e[0]+e[1]+e[2]+e[3]+e[4])/5 ;  // assumption : first 1600 Samples is silence
    printf("\nSilence Energy : %f\n",silence_energy);              //              and atleast 3200 samples is present.
    for(i=5; i<j; i++)
        if((e[i-1] < silence_energy)&&(e[i] > 2*silence_energy))
        {
            m=0;
            for(k=1; k<=10; k++)
            {
                if(e[i+k] <= silence_energy)
                m = m+1;
            }
            if(m==0)
            {
                start_frame_no = i;
                break;
            }
        }

    if(m==0)
    {
        for(i=start_frame_no+10; i<j; i++)
            if((e[i-1] > silence_energy)&&(e[i] <= silence_energy))
            {
                n=0;
                for(k=1; k<=10; k++)
                {
                    if(e[i+k] > silence_energy)
                    n = n+1;
                }
                if(n==0)
                {
                    end_frame_no = i;
                    break;
                }

            }
            if(n!=0) end_frame_no = j-1;
            printf("\nActual Signal starts from frame No : %d\n",start_frame_no);
            printf("\nActual Signal ends at frame No : %d\n",end_frame_no);
        }
    else
        {
            printf("\nStarting Points of voiced region in Given Signal is not found\n");
            /* That means either (1) or (2)
            (1) Given Signal is silence.
            (2) Assumption is not followed.
            Assumption : Atleast first 1600 Samples should be silence to set a reference energy
                         which helps to decide Starting Points of voiced region.
            */
        }

}

void coefficient(double v[80000],int a, int b)
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

    //printf("\nAuto Co-relation Coefficient: \n");
    int p = 12;
    double R[p+1];
    for(k=0; k<=p; k++)
    {
        R[k] = 0;
        for(m=0; m<N-k; m++)
        R[k] += s_hammed[m]*s_hammed[m+k];
        //printf("R[%d] : %f\n",k,R[k]);
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
    //printf("\nLPC Coeficient :\n");
    //for(i=1; i<=p; i++)
    //printf("a[%d] : %f\n",i,A[p][i]);

/*----------------------------------------- Cepstral Coeficient C[n]---------------------------------- */

    //printf("\nCepstral Coeficient :\n");
    for(m=1; m<=p; m++)
    {
        C[m] = A[p][m];
        for(k=1; k<m; k++)
            C[m] += (k*C[k]*A[p][m-k])/m ;
        //printf("C[%d] : %f\n",m,C[m]);
    }
}

double distance_measure(double ct[12], double cr[12]) // return tokhura distance of ct with cr
{
    int i,w[13] = {0,1,3,7,13,23,33,45,64,80,96,110,130};
    double D = 0 ;
    for(i=1; i<=12; i++)  // Q = 3p/2 = n = 12
        D = w[i]*(ct[i]-cr[i])*(ct[i]-cr[i]) ;
    return D;
}
