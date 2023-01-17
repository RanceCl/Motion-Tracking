
/*
	ECE 4310
	Lab 6
	Roderick "Rance" White
	
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>


#define SQR(x) ((x)*(x))


//size equals 10 * 2 * 62.5
void Acc_Gyro_Read(char *FileName, int size, double time[], double accX[], 
	double accY[], double accZ[], double pitch[], double roll[], double yaw[])
{
	FILE* fpt;
	int i, j;
	double t, aX, aY, aZ, pit, rol, ya;
	char a[100], b[100], c[100], d[100], e[100], f[100], g[100];

	/* read image */
	if ((fpt = fopen(FileName, "r")) == NULL)
	{
		printf("Unable to open %s for reading\n", FileName);
		exit(0);
	}

	/* Create arrays of the values from the text file */
	j = 0;
	//Skip the headers
	fscanf(fpt, "%s %s %s %s %s %s %s", a, b, c, d, e, f, g);
	while (1)
	{
		j++;
		//t[j], accX[j], accY[j], accZ[j], pit[j], rol[j], ya[j]
		//Read in each value of the text file and place them in the appropriate order
		i = fscanf(fpt, "%lf %lf %lf %lf %lf %lf %lf", &t, &aX, &aY, &aZ, &pit, &rol, &ya);
		//Places the read values in the appropriate position
		//This is the only way I could find to properly return all of the arrays
		time[j] = t;
		accX[j] = aX;
		accY[j] = aY;
		accZ[j] = aZ;
		pitch[j] = pit;
		roll[j] = rol;
		yaw[j] = ya;
		
		if (i != 7)
			break;																				//Break out of while loop if end is reached
		
	}
	fclose(fpt);
}

/* Writes in all of the information in the arrays to a table */
void Acc_Gyro_Write(char *FileName, int size, double time[], double accX[], 
	double accY[], double accZ[], double pitch[], double roll[], double yaw[])
{
	FILE* fpt;
	int j;

	fpt=fopen(FileName,"w");	
	//fprintf(fpt, "time,accel,velocity,average velocity,distance moved,cumulative distance moved\n");
	fprintf(fpt, "time,accX,accY,accZ,pitch,roll,yaw\n");
	for(j=0; j<=size; j++)
		fprintf(fpt, "%lf,%lf,%lf,%lf,%lf,%lf,%lf\n",time[j],accX[j],accY[j],accZ[j],
			pitch[j],roll[j],yaw[j]);	
	
	fclose(fpt);
}


/* This will smooth out the inputted list using basic Convolution */
void Basic_2D_Convolution(double *list, int ArraySize, int WindowSize)
{
	double *smoothed;
	int r,r2, W = WindowSize/2;
	double sum;
	
	/* allocate memory for smoothed version of list */
	smoothed=(double *)calloc(ArraySize, sizeof(double));
	
	/* smooth image, skipping the border points */
	for (r=W; r<ArraySize-W; r++){
		sum=0;
		for (r2=-WindowSize; r2<=WindowSize; r2++)
			sum+=list[r+r2];
		smoothed[r]=sum/WindowSize;
	}
	
	//smooth ends of the list
	for (r=0; r<W; r++)
	{
		smoothed[r]=list[r];
		smoothed[ArraySize-r]=list[ArraySize-r];
	}
	
	//Replace all values in the original array with the new values
	for (r=0; r<=ArraySize; r++)
		list[r] = smoothed[r];
	
	//printf("%lf\n", smoothed[ArraySize]);
}


/* Function to find the variance of the list */
void Information_Variance(double list[], double listVar[], int ArraySize, int WindowSize)
{
	double listDistance = 0;
	double listAvg = 0;
	int r,r2, W = WindowSize/2;
	
	/* The first part of this function is identical to the implementation of the convolution */
	for (r=W; r<ArraySize-W; r++){
		
		/* Find the average within the window */
		listAvg=0;
		for (r2=-WindowSize; r2<=WindowSize; r2++)
			listAvg += list[r+r2];
		listAvg = listAvg/WindowSize;
		
		/* Find the variance */
		for (r2=-WindowSize; r2<=WindowSize; r2++)
			//Subtract the mean of the window from the dataset to find the distance
			listDistance += SQR(list[r+r2]-listAvg);
		listVar[r] = listDistance/WindowSize;
		
	}
	
	//fix ends of the list
	for (r=0; r<W; r++)
	{
		listVar[r]=0;
		listVar[ArraySize-r]=0;
	}
}














int main()
{
	FILE* fpt;
	int size = 10 * 2 * 62.5;
	double time[size], accX[size], accY[size], accZ[size], pitch[size], roll[size], yaw[size];
	double accX_var[size], accY_var[size], accZ_var[size], pitch_var[size], roll_var[size],
		yaw_var[size];
	int i, j, WindowSize=15;
	
	int MoveStart=0, MoveStop=0, Moving=0;
	double TimeStart = 0, TimeStop = 0, velPrev = 0;
	double distX[size], distY[size], distZ[size], velX[size], velY[size], velZ[size];
	double distPit[size], distRol[size], distYaw[size];
	double distTotX=0, distTotY=0, distTotZ=0, velTotX=0, velTotY=0, velTotZ=0;
	int accthresh = 50;
	
	//char a[100], b[100], c[100], d[100], e[100], f[100], g[100];
	
	/* READS IN THE VALUES OF THE LIST */
	// Read in the list
	if ((fpt = fopen("acc_gyro.txt", "r")) == NULL)
	{
		printf("Unable to open %s for reading\n", "acc_gyro.txt");
		exit(0);
	}
	Acc_Gyro_Read("acc_gyro.txt", size, time, accX, accY, accZ, pitch, roll, yaw);
	Acc_Gyro_Write("acc_gyro.csv", size, time, accX, accY, accZ, pitch, roll, yaw);
	
	/* Smooth all data */
	//Basic_2D_Convolution(time, size, WindowSize);
	Basic_2D_Convolution(accX, size, WindowSize);
	Basic_2D_Convolution(accY, size, WindowSize);
	Basic_2D_Convolution(accZ, size, WindowSize);
	Basic_2D_Convolution(pitch, size, WindowSize);
	Basic_2D_Convolution(roll, size, WindowSize);
	Basic_2D_Convolution(yaw, size, WindowSize);
	
	Acc_Gyro_Write("acc_gyro_smoothed.csv", size, time, accX, accY, accZ, pitch, roll, yaw);
	
	/* Find the variances of the smoothed data */
	//Information_Variance(time, time_var, size, WindowSize);
	Information_Variance(accX, accX_var, size, WindowSize);
	Information_Variance(accY, accY_var, size, WindowSize);
	Information_Variance(accZ, accZ_var, size, WindowSize);
	Information_Variance(pitch, pitch_var, size, WindowSize);
	Information_Variance(roll, roll_var, size, WindowSize);
	Information_Variance(yaw, yaw_var, size, WindowSize);
	
	Acc_Gyro_Write("acc_gyro_variance.csv", size, time, accX_var, accY_var, 
		accZ_var, pitch_var, roll_var, yaw_var);
	
	
	
	
	fpt = fopen("Lab6Final.csv", "w");
	fprintf(fpt, "Start Time,Stop Time,Vel X,Vel Y,Vel Z,Tot Vel X,Total Vel Y,Tot Vel Z,Dist X,Dist Y,Dist Z,Tot Dist X,Total Dist Y,Tot Dist Z,Dist Pitch,Dist Roll,Dist Yaw\n");
			
	/* Detewrmine if the phone is moving */
	for(i=0; i<=size; i++)
	{
		/* Reset for calculations */
		distPit[i]=0;distRol[i]=0;distYaw[i]=0;
		velX[i]=0;distX[i]=0;velY[i]=0;distY[i]=0;velZ[i]=0;distZ[i]=0;

		/* Check if the phone is currently moving */
		if(accX_var[i] >= accthresh || accY_var[i] >= accthresh || accZ_var[i] >= accthresh)
			Moving = 1;
		else if(pitch_var[i] >= accthresh || roll_var[i] >= accthresh || yaw_var[i] >= accthresh)
			Moving = 1;
		else
			Moving = 0;
		
		/* Check if the phone has started moving */
		//Is moving, yet there is no indicator that it has already started
		if(Moving==1 && MoveStart==0)
		{
			MoveStart = i;
			TimeStart = MoveStop * 0.05;
		}
		
		/* Check if the phone has stopped moving */
		// Isn't moving, but had started moving, and currently hasn't stopped moving
		if(Moving==0 && MoveStart!=0 && MoveStop==0)
		{
			MoveStop = i;
			TimeStop = MoveStop * 0.05;
		}
		
		/* While currently moving, calculate the integration calculations */
		//Isn't currently starting and isn't currently starting, so mid motion
		if(MoveStart!=0 && MoveStop!=0)
		{
			//Loop through the value for how long the phone is moving
			for(j=MoveStart; j<MoveStop; j++)
			{
				distPit[i] += (pitch[j] * 0.05);
				distRol[i] += (roll[j] * 0.05);
				distYaw[i] += (yaw[j] * 0.05);
				
				velPrev = velX[i];
				velX[i] += (accX[j] * 9.81 * 0.05);
				distX[i] += 0.05 * (velX[i] + velPrev)/2;
				
				velPrev = velY[i];
				velY[i] += (accY[j] * 9.81 * 0.05);
				distY[i] += 0.05 * (velY[i] + velPrev)/2;
				
				velPrev = velZ[i];
				velZ[i] += (accZ[j] * 9.81 * 0.05);
				distZ[i] += 0.05 * (velZ[i] + velPrev)/2;
				
				/* Values in total */
				//distTotPit+=distPit[i]; distTotRol+=distRol[i]; distTotYaw+=distYaw[i];
				distTotX+=distX[i]; distTotY+=distY[i]; distTotZ+=distZ[i];
				velTotX+=velX[i]; velTotY+=velY[i]; velTotZ+=velZ[i];
							fprintf(fpt, "%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf\n",
								TimeStart,TimeStop,velX[i],velY[i],velZ[i],velTotX,velTotY,velTotZ,distX[i],distY[i],
								distZ[i],distTotX,distTotY,distTotZ,distPit[i],distRol[i],distYaw[i]);

				fprintf(fpt, "\n");
			}
			MoveStart = 0;
			MoveStop = 0;
			TimeStart = 0;
			TimeStop = 0;
		}
		Moving = 0;
	}
	fclose(fpt);
	


}










