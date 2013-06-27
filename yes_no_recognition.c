#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<string.h>

int frame_size = 160;
int frame_no;
int total_samples;
int x[1000];
int start_frame_no;
int end_frame_no;
int result[100][3];
int zcr[100];
int energy[100];
void input_signal();
void calculate();
void actual_signal();
void output();


void input_signal()
{
    int i = 0 , j = 0 ;
    char buffer[256][256][10];
    char filename[256];
    printf("\n text file name of audio signal: ");
    scanf("%s",filename);
    printf("%s\n", filename);
    FILE * myfile;
    myfile = fopen(filename,"r");

    while (!feof(myfile))
    {
        if(j == 256)
            {
                i = i+1;
                j=0;
            }

         fgets(buffer[i][j],256,myfile);

         if(buffer[i][j][strlen(buffer[i][j])-1] == '\n' )
            {
                x[i*256+j] = atoi(buffer[i][j]);
                //printf("sample %d : %d\n",i*256+j+1,x[i*256+j]);
                j = j+1 ;
            }
    }
    x[i*256+j] = atoi(buffer[i][j]);
    //printf("sample %d : %d\n",i*256+j+1,x[i*256+j]);
    j = j+1 ;
    fclose(myfile);

printf("\nNo. of total samples : %d\n",i*256+j);
total_samples = i*256+j;
}

void calculate()
{
int k=0,j;
frame_no = 1;

do{
zcr[frame_no] = 0;
energy[frame_no] = 0;

for(j=(frame_no-1)*frame_size; j<frame_no*frame_size; j++)
{    energy[frame_no] += x[j]*x[j];
    if(x[j]*x[j+1]<0)
    zcr[frame_no] += 1;
}
if(energy[frame_no]>500*frame_size)
{
result[k][0] = frame_no;
result[k][1] = energy[frame_no];
result[k][2] = zcr[frame_no];
k++;
}
frame_no++;
}while(j<total_samples);
}

void actual_signal()
{
int i;
for(i=1; i<(total_samples-1); i++)
if((energy[i-1]<energy[i])&&(energy[i]<energy[i+1])&&(5*energy[i-1]<energy[i+1]))
{
    start_frame_no = i;
    break;
}
for(i=start_frame_no; i<(total_samples-1); i++)
if((energy[i-1]>energy[i])&&(energy[i]>energy[i+1])&&(energy[i-1]>3*energy[i+1]))
{
    end_frame_no = i;
    break;
}
}

void output()
{
int i,j, sum_zcr=0,sum_energy=0;
float avg_energy,avg_zcr;
char filename[256];
printf("\n output textfile name of given audio signal: ");
scanf("%s",filename);
FILE *fp;
fp = fopen(filename, "w+");
for(i=0; i<frame_no; i++)
{
fprintf(fp,"Frame No %d\t Energy = %d\t ZCR = %d\n",i,energy[i],zcr[i]);
printf("Frame No %d\t Energy = %d\t ZCR = %d\n",i,energy[i],zcr[i]);
sum_energy += energy[i];
sum_zcr += zcr[i];
}
avg_energy = sum_energy/frame_no;
avg_zcr = sum_zcr/frame_no;

fprintf(fp,"voiced signal: start frame no = %d\n",start_frame_no);
printf("voiced signal: start frame no = %d\n",start_frame_no);

fprintf(fp,"voiced signal: start frame no = %d\n",end_frame_no);
printf("voiced signal: start frame no = %d\n",end_frame_no);

fprintf(fp,"voiced signal: total frames = %d\n",(end_frame_no - start_frame_no));
printf("voiced signal: total frames = %d\n",(end_frame_no - start_frame_no));

fprintf(fp,"voiced signal: total zcr = %d\n",sum_zcr);
printf("voiced signal: total zcr = %d\n",sum_zcr);

fprintf(fp,"voiced signal: total energy = %d\n",sum_energy);
printf("voiced signal: total energy = %d\n",sum_energy);

fprintf(fp,"voiced signal: average zcr = %d\n",avg_zcr);
printf("voiced signal: average zcr = %d\n",avg_zcr);

fprintf(fp,"voiced signal: average energy = %d\n",avg_energy);
printf("voiced signal: average energy = %d\n",avg_energy);

fprintf(fp,"unvoiced signal: total frames = %d\n",(frame_no - end_frame_no + start_frame_no));
printf("unvoiced signal: total frames = %d\n",(frame_no - end_frame_no + start_frame_no));

for(j=end_frame_no; j>(start_frame_no+2*(end_frame_no - start_frame_no)/3); j--)
{
if(zcr[j]<10)
{
    fprintf(fp,"\n given voice signal is 'YES'\n");
    printf("\n given voice signal is 'YES'\n");
}
else
{
    fprintf(fp,"\n given voice signal is 'NO'\n");
    printf("\n given voice signal is 'NO'\n");
}
}fclose(filename);
}

int main(void)
{
    input_signal();
    calculate();
    actual_signal();
    output();
    return 0;
}


