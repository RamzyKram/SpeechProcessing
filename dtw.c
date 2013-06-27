/* Design Document : Comparision of two Speech Signals by DTW algorithm :

Step 1 : Generate a text file ( Like signal1.txt, signal2.txt ) of each speech signal by some softare or program.

Step 2 : Generate frames (Each frame contain a set of cepstral coefficients) by splitting the voiced region from that text file
  	 and save these frames in order in a text file( Like input1.txt, input2.txt ).
		 where each row represent a frame and contains frame's set of coefficients as column elements.

Step 3 : Apply this program "dtw.c" only after doing step1 and step2 to find similerities and differences between signal1 and signal2.
		 It take two text files as input ( Like "input1.txt" and "input2.txt" ) and give Optimal Warping Path and Cost as output.
*/

/* Input format :-  input.txt : 9 frames of speech signal and each frame has 12 cepstral coefficient
1	1	1	1	1	1	1	1	1	1	1	1
2	2	2	2	2	2	2	2	2	2	2	2
3	3	3	3	3	3	3	3	3	3	3	3
4	4	4	4	4	4	4	4	4	4	4	4
5	5	5	5	5	5	5	5	5	5	5	5
6	6	6	6	6	6	6	6	6	6	6	6
7	7	7	7	7	7	7	7	7	7	7	7
8	8	8	8	8	8	8	8	8	8	8	8
9	9	9	9	9	9	9	9	9	9	9	9

*/

// Assumption : All cepstral coefficients are integer type.
#include<stdio.h>

int distance_measure(int ct[12], int cr[12]) //return Tohkura Distance(A Weighted Cepstral Distance) between the set of cepstral coefficients "ct" and "cr"
{
    int i,w[12] = {1,3,7,13,23,33,45,64,80,96,110,130};
    int D = 0 ;
    for(i=0; i<12; i++)
        D = w[i]*(ct[i]-cr[i])*(ct[i]-cr[i]) ;
    return D;
}

int min( int x, int y, int z ) // return minimum among integer x, y and z
{
    if( ( x <= y ) && ( x <= z ) ) return x;
    if( ( y <= x ) && ( y <= z ) ) return y;
    if( ( z <= x ) && ( z <= y ) ) return z;
}


int main()
{
    int i,k;

printf("\n------------------------------------------ input1 -----------------------------\n");

int n;
printf("First Input Text File => No. of Rows : ");
scanf("%d",&n);

int ceps1[n][12];
    char filename1[256];
    printf("First Input Text File Name : ");
    scanf("%s",filename1);
    printf("\n");

FILE *file1;
file1 = fopen(filename1,"r");

if(file1==NULL)
    printf("file not found!");

for(i=0; i<n; i++)
{
    for(k=0; k<12; k++)
    {
        fscanf(file1,"%d",&ceps1[i][k]);
        printf("%d ",ceps1[i][k]);
    }printf("\n");
}
fclose(file1);

printf("\n------------------------------------------ input2 -----------------------------\n");

int m;
printf("First Input Text File => No. of Rows : ");
scanf("%d",&m);

int ceps2[m][12];
    char filename2[256];
    printf("Second Input Text File Name : ");
    scanf("%s",filename2);
    printf("\n");

FILE *file2;
file2 = fopen(filename2,"r");

if(file2==NULL)
    printf("file not found!");

for(i=0; i<m; i++)
{
    for(k=0; k<12; k++)
    {
        fscanf(file2,"%d",&ceps2[i][k]);
        printf("%d ",ceps2[i][k]);
    }printf("\n");
}
fclose(file2);

printf("\n----------------------------- Local Distance Matrix ---------------------------\n\n");

int local_distance[n][m];

for(i=0; i<n; i++)
{
    for(k=0; k<m; k++)
    {
        local_distance[i][k] = distance_measure(ceps1[i],ceps2[k]);
        printf("%d\t ",local_distance[i][k]);
    }
    printf("\n");
}

printf("-------------------------------------------------------------------------------\n\n");



printf("----------------------------- Global Distance Matrix --------------------------\n\n");

int global_distance[n][m];

    global_distance[0][0] = local_distance[0][0];
    printf("%d\t ",global_distance[0][0]);

for(i=1; i<n; i++) // generating first row element of global_distance matrix
    global_distance[i][0] = local_distance[i][0] + global_distance[i-1][0];

for(k=1; k<m; k++) // generating first column element of global_distance matrix
    {
        global_distance[0][k] = local_distance[0][k] + global_distance[0][k-1];
        printf("%d\t ",global_distance[0][k]);
    }
    printf("\n");

for(i=1; i<n; i++)
{
    printf("%d\t ",global_distance[i][0]);
    for(k=1; k<m; k++)
    {
        global_distance[i][k] = local_distance[i][k] + min(global_distance[i-1][k],global_distance[i-1][k-1],global_distance[i][k-1]);
        printf("%d\t ",global_distance[i][k]);
    }
    printf("\n");
}

printf("-------------------------------------------------------------------------------\n\n");

printf("Optimal Warping Path : (0,0) ");

i=0;
k=0;

while((i!=n-1)||(k!=m-1))
{
    if(i==n-1) // if you reached to the last row (n-1) then go only one step forward in last row
        k=k+1;
    else if(k==m-1) //if you have reached to the last column (m-1) then go only one step upward in last column
        i=i+1;
    else
    {   int global_minm = min(global_distance[i+1][k],global_distance[i+1][k+1],global_distance[i][k+1]);

        if(global_distance[i+1][k] == global_minm)
        {
            i=i+1;
        }
        else if(global_distance[i+1][k+1] == global_minm)
        {
            i=i+1;
            k=k+1;
        }
        else//(global_distance[i][k+1] == global_minm)
        {
            k=k+1;
        }
    }

printf("(%d,%d) ",i,k);

}

printf("\nOptimal Warping Path Cost : %d\n", global_distance[n-1][m-1]);

printf("\n-------------------------------------------------------------------------------\n\n");

return 0;

}
