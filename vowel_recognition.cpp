#include<iostream>
#include<fstream>
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<string.h>
#include<dirent.h> // for comparision (getcwd(buf, sizeof(buf)) != NULL)

int total_samples,actual_samples, max_amplitude_position, start_frame_no, end_frame_no;
int x[80000];
double x_dc_balanced[80000], x_normalised[80000], y[40000] ;
char alphabet[] = "aeiou";
double cr[12],C[12] ;

using namespace std;

int to_do(); // it take what you want to do (i.e. it take your choice no. from screen)
void switch_function(int n); // it goes to that part of code which you want
void input_signal(); // it takes sample values from text file of an audio signal and store in an array x[]
void dc_shift();  // it does DC Balancing of x[] and store in another array x_dc_balanced[]
void normalisation(); // it does DC Balancing of x_dc_balanced[] and store in another array x_normalised[]
void actual_signal(); // it cut voiced region from x_normalised[] on the basis of energy of each frame[a,b] calculated by Energy(int a, int b) and store in another array y[]
double Energy(int a, int b);
int max_amplitude(); // it return the position in y[] where sample value is maximum. it is stored in max_amplitude_position
void reference_signal(int i);
void frame_samples(int a, int b); // it does  work(1):derivation of actual frame, work(2):apply hamming window on it work(3):calculate auto-corelation,LPC and then cestral coefficients
double distance_measure(double ct[12], double cr[12]); // it return tokhura distance of ct (cepstral coefficient of test signal) with cr (cepstral coefficient of reference signal)

int main()
{
    char buf[BUFSIZ + 1];
  if (getcwd(buf, sizeof(buf)) != NULL)
	printf("%s\n", buf);

    int num = to_do();
    switch_function(num);

return 0;
}

int to_do() // it take what you want to do (i.e. it take your choice no. from screen)
{
    printf("\n*************************** What do you want ? *****************************\n");

    printf("\n(1) Test the Signal");
    printf("\n(2) Reset the Reference Signal");
    printf("\n(3) Print the Reference Signal");
    printf("\n(4) Exit");
    printf("\n\n****************************************************************************\n");

    printf("\nEnter your choice : ");
    int choice;
    scanf("%d",&choice);
    return choice;
}


void switch_function(int n) // it goes to that part of code which you want
{
char ch, cha[2];
int i, j, k;
switch(n)
{
    case 1 :
            input_signal();    // x[total_samples]
            dc_shift();        // x_dc_sifted[total_samples]
            normalisation();   // x_normalised[total_samples]
            actual_signal();   // y[actual_samples]
            max_amplitude_position = max_amplitude();

            FILE *fp;
            fp = fopen("output/result.txt", "w+");
            for(i=0; i<total_samples; i++)
            fprintf(fp,"Sample[%d] : %d\n",i+1,x_normalised[i]);
            fprintf(fp,"\nActual Signal starts from frame No : %d\n",start_frame_no);
            fprintf(fp,"\nActual Signal ends at frame No : %d\n",end_frame_no);

            frame_samples(max_amplitude_position-160,max_amplitude_position+160);
            printf("\n");

            fprintf(fp,"\nCepstral Coeficient :\n");
            for(i=1; i<13; i++)
            fprintf(fp,"C[%d] : %f\n",i,C[i]);

            double min_d, d[sizeof(alphabet)-1];
            int min_d_position;
            for(i=1; i<sizeof(alphabet); i++)
            {
                reference_signal(i-1);   // Array cr stores cepstral coefficients of (i+1)th alphabet
                d[i] = distance_measure(C,cr);
                cha[0] = alphabet[i-1];
                cha[1] = '\0';
                printf("Distance Measure with Signal %s => D_TCEP[%s] : %f\n",&cha, &cha,d[i]);
                fprintf(fp,"\nDistance Measure with Signal %s => D_TCEP[%s] : %f",&cha, &cha,d[i]);
            }
            min_d = d[1];
            min_d_position=1;
            for(i=2; i<sizeof(alphabet); i++)
            {
                if(min_d > d[i])
                    {
                        min_d = d[i];
                        min_d_position = i;
                    }
            }
            cha[0] = alphabet[min_d_position-1];
            cha[1] = '\0';
            printf("\nYou have given signal : %s \nwith distance : %f\n\n",&cha,min_d);
            fprintf(fp,"\nYou have given signal : %s \nwith distance : %f",&cha,min_d);
            fclose(fp);

            k = to_do();
            switch_function(k);
            break;
    case 2 :
            printf("\nTo Reset Reference Signal => Small Letter Alphabet : ");
            scanf("%s",&ch);

            cha[0] = ch;
            cha[1] = '\0';
            strupr(cha);
            //printf("\nyou have given : %s\n",cha);
            j=0;
            for(i=0; i<sizeof(alphabet)-1; i++)
            {
               if(alphabet[i] == ch)
                {
                    input_signal();    // x[total_samples]
                    dc_shift();        // x_dc_sifted[total_samples]
                    normalisation();   // x_normalised[total_samples]
                    actual_signal();    // y[actual_samples]
                    max_amplitude_position = max_amplitude();
                    frame_samples(max_amplitude_position-160,max_amplitude_position+160);

                    char filename[256];
                    filename[0] = alphabet[i];
                    filename[1] = '\0';
                    strcat(filename,".txt");
                    char path[256] = "output/";
                    strcat(path,filename);
                    //printf("\nfilename : %s\n",&alphabet[i]);
                    //printf("\npath : %s\n",&path);
                    FILE *fp;
                    fp = fopen(path, "w+");
                    for(k=1; k<13; k++)
                    fprintf(fp,"%f\n",C[k]);
                    fclose(fp);
                    j = j+1;
                }
            }
            if(j == 0) printf("\nYour have given out of range\n\n");
            else printf("\nCepstral Coefficient of Reference Signal %s has been reset successfully\n\n",&cha);

            k = to_do();
            switch_function(k);
            break;

    case 3 :
            printf("\nTo Print Reference Signal => Small Letter Alphabet : ");
            scanf("%s",&ch);

            cha[0] = ch;
            cha[1] = '\0';
            strupr(cha);
            //printf("\nyou have given : %s\n",cha);
            j=0;
            for(i=0; i<sizeof(alphabet)-1; i++)
            {
                if(alphabet[i] == ch)
                {
                    //printf("\nfilename : %s\n",&alphabet[i]);
                    //printf("i : %d\n",i);
                    reference_signal(i);   // Array cr stores cepstral coefficients of (i+1)th alphabet
                    j = j+1;
                }
            }
            if(j == 0)
                printf("\nThere is no such reference signal stored\n\n");
            else
            {
                for(k=1; k<=12; k++)
                printf(" %s Signal Cepstral Coefficien[%d] : %f\n",cha,k,cr[k]);
            }
            printf("\n");
            k = to_do();
            switch_function(k);
            break;
    case 4 :
            printf("\n--------------------------- Thanks for Evaluation ----------------------------\n");
            break;
    default :
            printf("\nyou have entered wrong choice\n\n");
            k = to_do();
            switch_function(k);
            break;
}
}

void input_signal() // it takes sample values from text file of an audio signal and store in an array x[]
{
    int i,j=0,k;
    char filename[256];
    char path[256] = "input/";
    printf("\nText File Name of Audio Signal: ");
    scanf("%s",filename);
    strcat(path,filename);
    //printf("%s",&path);
    char *inname = path;
    ifstream infile(inname);

    if (!infile)
    {
        cout << "\nThere is a problem to open file " << inname << " for reading.\n\n" << endl;
        k = to_do();
        switch_function(k);
    }
    //cout << "Opened " << inname << " for reading." << endl;
    while (infile >> i)
    {
        x[j] = i;
        cout << "\nSample[" << j+1 << "] : " << x[j] << endl;
        j = j+1;
    }
    total_samples = j;
}

void dc_shift()  // it does DC Balancing of x[] and store in another array x_dc_balanced[]
{
    int i;
    double x_mean, temp = 0;
    for(i=0; i<total_samples; i++)
    {
        temp += x[i];
    }
    x_mean = temp/total_samples;
    //printf("%f\n",x_mean);
    for(i=0; i<total_samples; i++)
    {
        x_dc_balanced[i] = x[i]-x_mean;
    }
    //printf("%f\n",x_mean);
}

void normalisation() // it does DC Balancing of x_dc_balanced[] and store in another array x_normalised[]
{
    int i, max_position;
    double max = 0;
    for(i=0; i<total_samples; i++)
    {
        if(x_dc_balanced[i]>0)
        {
            if(max<=x_dc_balanced[i])
            {
                max = x_dc_balanced[i];
                max_position= i;
            }
        }
        else
        {
            if(max<= -x_dc_balanced[i])
            {
                max = -x_dc_balanced[i];
                max_position= i;
            }
        }
    }
    //printf("Max Position : %d",max_position);
    for(i=0; i<total_samples; i++)
    {
        x_normalised[i] = (1000*x_dc_balanced[i])/max ;
        printf("x_normalised[%d] : %f\n",i,x_normalised[i]);
    }
}
void actual_signal()  // x_normalised[] to y[] conversion  and calculate start & end frame no. of actual signal
{
    int i, j=0, k, m, n=0, min_energy_frame_no, frames = 1000;//((total_samples-240)/80) ;  //(total_samples/80) < frames+4 <= (total_samples/80)+1
    double e[frames], min_energy = 0;
    for(i=0; i<(total_samples-320); i = i+80)
    {
        e[j] = Energy(i,i+320);
        //printf("%d :E[%d]: %f\n",i,j,e[j]);
        //printf("total samples : %d\n", total_samples);
        j = j+1;
        if(min_energy <= e[i])
        {
            min_energy = e[i];
            min_energy_frame_no = i;
        }
    }
    double silence_energy = (e[0]+e[1]+e[2]+e[3]+e[4])/5 ;  // assumption : first 1600 Samples is silence
    printf("\n%d\n",silence_energy);                   //              and atleast 3200 samples is present.
    for(i=5; i<j; i++)
        if((e[i-1] < silence_energy)&&(e[i] > 2*silence_energy))
            for(k=1; k<10; k++)
            {
                if(e[i+k] <= silence_energy)
                m = m+1;
            }

    if(m==0)
        {
            start_frame_no = i*80;
            for(i=(start_frame_no/80)+10; i<j; i++)
                if((e[i-1] > silence_energy)&&(e[i] <= silence_energy))
                    for(k=1; k<10; k++)
                        {
                            if(e[i+k] > silence_energy)
                            n = n+1;
                        }
            if(n==0) end_frame_no = i*80;
            else end_frame_no = total_samples-1;
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
            int k = to_do();
            switch_function(k);
        }

    actual_samples = end_frame_no - start_frame_no;
    for(i=0; i<actual_samples; i++)
    {
        y[i] = x_normalised[start_frame_no+i];
    }

}

double Energy(int a, int b) // return energy of frame[a,b] of x_normalised
{
    int i;
    double energy = 0 ;
    for(i=a; i<b; i++)
    {
        energy += x_normalised[i]*x_normalised[i] ;
    }
    //printf("%f\n",energy);
return energy;
}

int max_amplitude() // it return the position in y[] where sample value is maximum. it is stored in max_amplitude_position
{
    int i, max_position;
    double max = 0;
    for(i=0; i<actual_samples; i++)
    {
        if(max<=y[i])
        {
            max = y[i];
            max_position= i;
        }
    }
//printf("\nSample No %d : %d ( Here Amplitude is Maximum )\n",(start_frame_no+max_position),y[max_position]);
return max_position;
}

void reference_signal(int i) // cr stores ith vowel's cepstral coefficient from its file (ith vowel).txt of output folder
{           //printf("\nthis is the reference_signal executing\n");

            char filename[256];
            filename[0] = alphabet[i];
            filename[1] = '\0';
            strcat(filename,".txt");
            char path[256] = "output/";
            strcat(path,filename);
            //printf("\nfilename : %s\n",filename);
            char *inname = path;
            ifstream infile(inname);
            if (!infile)
            {
                cout << "\nThere is no such reference signal stored " << inname << endl;
            }
            double k;
            int j = 0;
            while (infile >> k)
            {
                j = j+1;
                cr[j] = k;
                //cout << "Cepstral Coefficient [" << j << "] : " << cr[j] << endl;
            }
}

void frame_samples(int a, int b)
{
    int i, j, N = 0 ,k ,m ;
/*----------------------------------------- Max_Amplitude_Frame Samples  s[N] ---------------------------------- */
    double s[320];
    for(i = a; i < b; i++)
    if((i >= 0)&&(i < actual_samples))
    {
        s[N] = y[i];
        N = N+1;
    }
    //printf("No of frame samples : %d\n",N);
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
