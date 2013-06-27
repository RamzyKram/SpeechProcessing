#include<iostream>
#include<fstream>
#include<vector>
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<string.h>
#include<dirent.h> // for comparision (getcwd(buf, sizeof(buf)) != NULL)

using namespace std;

double ceps[1000][12]; // frames = 1000
int start_frame_no, end_frame_no;

void input();

int main()
{
    char buf[BUFSIZ + 1];
  if (getcwd(buf, sizeof(buf)) != NULL)
	printf("%s\n", buf);
//------------------------------------------------ input1 ---------------------------------------

input();

int i,k;
double ceps1[end_frame_no - start_frame_no + 1][12];
int j1=0;
for(i=start_frame_no; i<=end_frame_no; i++)
{
    for(k=1; k<=12; k++)
    {
        ceps1[j1][k] = ceps[j1][k];
        printf("%f ",ceps1[j1][k]);
    }
    printf("\n");
    j1++;
}
return 0;
}


void input() // it takes only numbers from a textfile and store in an array v[]
{
    int i,j;
    vector<double> v;
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
    double v_mean,temp =0;
    j=0;
    while (infile >> i)
    {
        temp += i;
        v.push_back(i);
        //cout << "\nSample[" << j+1 << "] : " << v[j] << endl;
        j = j+1;
    }
    //printf("input function is executed");
    v_mean = temp/v.size();
    printf("\nSamples_Mean : %f\n",v_mean);


    int max_position;
    double max = 0;
    for(i=0; i<v.size(); i++)
    {   v[i] = v[i]-v_mean;
        //printf("DC_Balanced_Sample[%d] : %f\n",v[i]);
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
    printf("\nMax DC_Balanced_Sample : %f\n",max);
    for(i=0; i<v.size(); i++)
    {
        v[i] = (1000*v[i])/max ;
        //printf("Normalised_Sample[%d] : %f\n",i,v[i]);
    }

 // cut the voiced region and calculate start & end frame no. of voiced region

    int k, m, n, frames = ( v.size()/80 ) - 3 ;
    //printf("frames : %d ", frames);
    double e[frames]; //(v.size()/80) < frames+4 <= (v.size()/80)+1
    j=0;
    for(i=0; i<(v.size()-320); i = i+80)
    {
        e[j]=0;
        for(k=i; k<i+320; k++)
        e[j]  += v[i]*v[i];
        //printf("Energy of Frame[%d] : %f\n",j,e[j]);
        j = j+1;
    }
    double silence_energy = (e[0]+e[1]+e[2]+e[3]+e[4])/5 ;  // assumption : first 1600 Samples is silence
    printf("\nSilence Energy : %f\n",silence_energy);                   //              and atleast 3200 samples is present.
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
            printf("\nActual Signal ends at frame No : %d\n\n",end_frame_no);
        }
    else
        {
            printf("\nStarting Points of voiced region in Given Signal is not found\n\n");
            /* That means either (1) or (2)
            (1) Given Signal is silence.
            (2) Assumption is not followed.
            Assumption : Atleast first 1600 Samples should be silence to set a reference energy
                         which helps to decide Starting Points of voiced region.
            */
        }

double w[320],s[320];
for(k=0; k<320; k++)
    w[k] = 0.54 + 0.46*cos(6.2832*(k-160)/319);
double R[13];
double E[13], K[12], A[12][12];
double C[12];

int i1, j1=0;
for(i1=start_frame_no; i1<=end_frame_no; i1++)
{
printf("%d\n",i1);
/*-------------------------------------- Hamming Window -----------------------------------------------*/
    for(k=0; k<320; k++)
        s[k] = w[k]*v[i1*80+k];
    //printf("%f\n", s[0]);
    printf("start sample of frame[%d] : %f\n",i1,v[i1*80]);
/*----------------------------------------- Auto Co-relation Coeficient R[p+1] ---------------------------------- */

    //printf("\nAuto Co-relation Coefficient: \n");
    for(k=0; k<13; k++)
    {
        R[k] = 0;
        for(m=0; m<320-k; m++)
        R[k] += s[m]*s[m+k];
        //printf("R[%d] : %f\n",k,R[k]);
    }

/*----------------------------------------- LPC Coeficient A[p][p]---------------------------------- */
    E[0] = R[0];
    for(i=1; i<13; i++)
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
    //for(i=1; i<13; i++)
    //printf("a[%d] : %f\n",i,A[12][i]);

/*----------------------------------------- Cepstral Coeficient C[n]---------------------------------- */

    //printf("\nCepstral Coeficient :\n");
    for(m=1; m<13; m++)
    {
        C[m] = A[12][m];
        for(k=1; k<m; k++)
            C[m] += (k*C[k]*A[12][m-k])/m ;
        //printf("C[%d] : %f\n",m,C[m]);
        ceps[j1][m] = C[m];
        //printf("Ceps[%d][%d] : %f\n",j1,m,ceps[j1][m]);
    }
//-------------------------------------------------------------------------------------------------------
    j1 = j1 + 1;

}


}
