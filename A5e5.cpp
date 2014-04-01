#define __STDC_FORMAT_MACROS
#include <assert.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <math.h>
#include <deque>
#include <iostream>

using namespace std;

struct robstruct{
	 int sourceareg[3];
	 int sourcepreg[3];
	 int rready;
	 int destareg[2];
	 int destpreg[2];
	 int destfree[2];
	 int isLoad;
	 int isStore;
	 uint64_t addressls;
	 int seqno;
	 int issued;
	 int fetchc;
	 int issuec;
	 int donec;
	 int commitc;
	 int ismispredict;
	 int isnotdone;
	 char macroOp[12];
	 char microOp[23];
 };

deque<robstruct> rob;
//robstruct robread;



void simulate(FILE* inputFile, FILE* outputFile,unsigned int N)
{

  // See the documentation to understand what these variables mean.
  int32_t microOpCount;
  uint64_t instructionAddress;
  int32_t sourceRegister1;
  int32_t sourceRegister2;
  int32_t destinationRegister;
  char conditionRegister;
  char TNnotBranch;
  char loadStore;
  int64_t immediate;
  uint64_t addressForMemoryOp;
  uint64_t fallthroughPC;
  uint64_t targetAddressTakenBranch;
  char macroOperation[12];
  char microOperation[23];
  int width = 8;
  int exitflag = 0;
  int fetchready = 0;

  int64_t totalMicroops = 0;
  int64_t totalMacroops = 0;
 int arch_reg[50] = {0};
 int maptable[50] = {0};
 int pregsize = 2048;
 int currentcycle = 0;
 int scoreboard[pregsize];

 int index_size = 16;
   int size = pow(2,index_size);
   int Tbit = 16;
   uint64_t THistory = 16;
   uint64_t TVal = pow(2,Tbit)-1;

   int THUpdate = 0;

   char BM[size];

   for(int i=0; i<=size; i++)
   {
   BM[i]='N';
   }

   char prediction;
   int index = 0;





 int latency;
 int temp, temp1, temp2, tempf1, tempf2 = 0;
 int i,j;

  for(i=0; i<50; i++)
    {
  	  arch_reg[i]=i;
  	  maptable[i]=i;
    }


  deque<int> phy_reg(pregsize,0);


  for (i=0; i<pregsize; i++)
  {
	  scoreboard[i]=0;
      phy_reg.push_back(i);
  }

  for (i=0; i<(50+pregsize); i++)
    {
  	  phy_reg.pop_front();
    }


  //cache declarations
  uint64_t Address_access = 0;
    int ind_bits = 0;
    int misscount = 0;
    unsigned int p1, p2 = 0;
    int hit = 0;
    int col = 0;

  int S = 8*1024;
  int A = 2;
  int B = 64;
  int way = A;
  int set = S/(A*B);
  int wway = 0;
    int offset = log2(B);
    int indexc = log2(set);
    int setno = 0;
    uint64_t cache[set][way];
    int leastRec[set];
    char lstate[set][way];


    for(i=0;i<set;i++)
    {
    leastRec[i] = 0;
    for(j=0; j<way;j++)
    {
    cache[i][j]=0;
    lstate[i][j]='C';
    }
    }






  //fprintf(outputFile, "Processing trace...\n");

  while (true) {

	//commit

	  //for width more than 1, should traverse inside rob

	  if(rob.size()!=0)
	  {
		  for(i=0; i<width ; i++)
	{

			  if(rob.size()==0)
				  break;

			  robstruct robfront;
			  robfront = rob.front();


		if( robfront.donec <= currentcycle && (robfront.issued == 1) )
		{
			robfront.commitc = currentcycle;

		/*	fprintf(outputFile, " %d:",robfront.seqno);
			fprintf(outputFile, "  %d",robfront.fetchc);
			fprintf(outputFile, "  %d",robfront.issuec);
			fprintf(outputFile, "  %d",robfront.donec);
			fprintf(outputFile, "  %d",robfront.commitc);

			if(robfront.sourceareg[0]!=-1)
			{
			fprintf(outputFile, ",  r%d ->",robfront.sourceareg[0]);
			fprintf(outputFile, "  p%d",robfront.sourcepreg[0]);
			}

			if(robfront.sourceareg[1]!=-1)
			{
			fprintf(outputFile, ",  r%d ->",robfront.sourceareg[1]);
			fprintf(outputFile, "  p%d",robfront.sourcepreg[1]);
			}

			if(robfront.sourceareg[2]!=-1)
			{
		    fprintf(outputFile, ",  r%d ->",robfront.sourceareg[2]);
			fprintf(outputFile, "  p%d,",robfront.sourcepreg[2]);
			}

			if(robfront.destareg[0]!=-1)
			{
				fprintf(outputFile, ",  r%d ->",robfront.destareg[0]);
				fprintf(outputFile, "  p%d",robfront.destpreg[0]);
				fprintf(outputFile, "  [p%d]",robfront.destfree[0]);
			}

			if(robfront.destareg[1]!=-1)
			{
				fprintf(outputFile, ",  r%d ->",robfront.destareg[1]);
				fprintf(outputFile, "  p%d",robfront.destpreg[1]);
				fprintf(outputFile, "  [p%d]",robfront.destfree[1]);
			}

			fprintf(outputFile, " |");
			fprintf(outputFile, " %s",robfront.macroOp);
			fprintf(outputFile, " %s\n",robfront.microOp); */

			rob.pop_front();

		}


	}
	  }

	int count = 0;

	//issue
  if(rob.size()!=0)
  {
		deque<robstruct>::iterator it;



		for(it=rob.begin() ; it!= rob.end(); it++)
		{

			if( (*it).sourceareg[0]==-1 && (*it).sourceareg[1]==-1 && (*it).sourceareg[2]!=49 )
				(*it).rready = 1;

			if( (*it).sourceareg[0]==-1 && (*it).sourceareg[1]==-1 && (*it).sourceareg[2]==49 )
			{
				if(scoreboard[(*it).sourcepreg[2]]==0)
				(*it).rready = 1;
			}

			if( (*it).sourceareg[0]==-1 && (*it).sourceareg[1]!=-1 && (*it).sourceareg[2]!=49 )
			{
				if(scoreboard[(*it).sourcepreg[1]]==0)
				(*it).rready = 1;
			}

			if( (*it).sourceareg[0]==-1 && (*it).sourceareg[1]!=-1 && (*it).sourceareg[2]!=49 )
			{
				if((scoreboard[(*it).sourcepreg[1]]==0)&&(scoreboard[(*it).sourcepreg[2]]==0))
				(*it).rready = 1;
			}

			if( (*it).sourceareg[0]!=-1 && (*it).sourceareg[1]==-1 && (*it).sourceareg[2]!=49 )
			{
				if(scoreboard[(*it).sourcepreg[0]]==0)
				(*it).rready = 1;
			}

			if( (*it).sourceareg[0]!=-1 && (*it).sourceareg[1]==-1 && (*it).sourceareg[2]==49 )
			{
				if((scoreboard[(*it).sourcepreg[0]]==0)&&(scoreboard[(*it).sourcepreg[2]]==0))
				(*it).rready = 1;
			}

			if( (*it).sourceareg[0]!=-1 && (*it).sourceareg[1]!=-1 && (*it).sourceareg[2]!=49 )
			{
				if((scoreboard[(*it).sourcepreg[0]]==0)&&(scoreboard[(*it).sourcepreg[1]]==0))
				(*it).rready = 1;
			}

			if( (*it).sourceareg[0]!=-1 && (*it).sourceareg[1]!=-1 && (*it).sourceareg[2]==49 )
			{
				 if((scoreboard[(*it).sourcepreg[0]]==0)&&(scoreboard[(*it).sourcepreg[1]]==0)&&(scoreboard[(*it).sourcepreg[2]]==0))
				 (*it).rready = 1;
			}

			if((*it).isLoad==1)
						{
							deque<robstruct>::iterator it2;

							for(it2=rob.begin() ; it2!= rob.end(); it2++)
						{
								if((*it2).donec <= currentcycle && ((*it2).issued == 1) )
								(*it2).isnotdone = 0;

								if((*it2).isStore==1 && ((*it2).seqno < (*it).seqno) && ((*it2).isnotdone==1))
								{
								(*it).rready = 0;
								}




						}
						}

             if (((*it).issued == 0) && ((*it).rready == 1) )
             {

            	 //cache simulation


            	 if((*it).isLoad==1)
            	 {
            		// latency = 4;
            		 uint64_t rand1 = pow(2,offset);
            		 Address_access = (*it).addressls & ~(rand1-1);
            		 	//Address_access = addressForMemoryOp & 0xFFFFFFFFFFFFE0;

            		 	p1 = Address_access;



            		 	 uint64_t rand2 = pow(2,indexc);
            		 	 ind_bits = ((*it).addressls >> offset) & (rand2-1);

            		 	 setno = ind_bits;




            		 	 for(wway=0;wway<way;wway++)
            		 	 {
            		 	 if(cache[setno][wway] == ((*it).addressls >> (offset+indexc)))
            		 	 col = wway;
            		 	 }

            		 	 if(cache[setno][col] == ((*it).addressls >> (offset+indexc)))
            		 	 {
            		 	 hit = 1;

            		 	 if((col == 0) && leastRec[setno] == 1)
            		 	 leastRec[setno] = 1;
            		 	 else if((col == 1) && leastRec[setno] == 0)
            		 	 leastRec[setno] = 0;
            		 	 else if((col == 0) && leastRec[setno] == 0)
            		 	 leastRec[setno] = 1;
            		 	 else if((col == 1) && leastRec[setno] == 1)
            		 	 leastRec[setno] = 0;
            		 	 }

            		 	 else
            		 	 {
            		 	 hit = 0;


            		 	 if(leastRec[setno]==0)
            		 	 cache[setno][0]=((*it).addressls >> (offset+indexc));
            		 	 else if(leastRec[setno]==1)
            		 	 cache[setno][1]=((*it).addressls >> (offset+indexc));

            		 	 if(leastRec[setno]==0)
            		 	 leastRec[setno]=1;
            		 	 else if(leastRec[setno]==1)
            		 	 leastRec[setno]=0;


            		 	 }





            		 	 if(hit == 1)
            		 	 {
            		 	 latency = 4;
            		 	 }

            		 	 else
            		 	 {
            		 	  latency = 4+7;
            		 	 }




            	 }
            	 else
            		 latency = 1;

            	 (*it).issued = 1;
            	 (*it).issuec = currentcycle;
            	 (*it).donec = currentcycle+latency;
            	 count = count+1;


            	 if((*it).ismispredict==1)
            	 {
            		 if(fetchready==-1)
            		fetchready = latency + 3;
            	 }



            	 if( (*it).destareg[0]!=-1)
            	 {
            	     scoreboard[(*it).destpreg[0]]=latency;
            	 }

            	 if((*it).destareg[1]==49)
            	 {
            	     scoreboard[(*it).destpreg[1]]=latency;
            	 }


             }




            	 if(count==width)
            		 break;


		  }


  }

		 // }

		  //fetch cycle

    int fcount = 0;

	for(fcount=0;fcount<width;fcount++)
	{

		if(fetchready>0)
		{
			fetchready = fetchready-1;
			break;
		}

		if(fetchready == -1)
			break;


       if(fetchready == 0)
{

		if(rob.size()==N)
			break;

		if(exitflag==1)
			break;



    int result = fscanf(inputFile,
                        "%" SCNi32
                        "%" SCNx64
                        "%" SCNi32
                        "%" SCNi32
                        "%" SCNi32
                        " %c"
                        " %c"
                        " %c"
                        "%" SCNi64
                        "%" SCNx64
                        "%" SCNx64
                        "%" SCNx64
                        "%11s"
                        "%22s",
                        &microOpCount,
                        &instructionAddress,
                        &sourceRegister1,
                        &sourceRegister2,
                        &destinationRegister,
                        &conditionRegister,
                        &TNnotBranch,
                        &loadStore,
                        &immediate,
                        &addressForMemoryOp,
                        &fallthroughPC,
                        &targetAddressTakenBranch,
                        macroOperation,
                        microOperation);

    if (result == EOF) {
    	exitflag=1;
      break;
    }

    robstruct robread;

	 robread.sourceareg[0]=-1;
	 robread.sourceareg[1]=-1;
	 robread.sourceareg[2]=-1;
	 robread.sourcepreg[0]= 0;
	 robread.sourcepreg[1]= 0;
	 robread.sourcepreg[2]= 0;
	 robread.rready=0;
	 robread.destareg[0]=-1;
	 robread.destareg[1]=-1;
	 robread.destpreg[0]=0;
	 robread.destpreg[1]=0;
	 robread.destfree[0]=0;
	 robread.destfree[1]=0;
	 robread.isLoad = 0 ;
	 robread.isStore = 0;
	 robread.addressls = 0;
	 robread.isnotdone = 1;
	 robread.issued = 0;
	 robread.fetchc=currentcycle;
	 robread.issuec=0;
	 robread.donec=0;
	 robread.commitc=0;
	 robread.ismispredict = 0;
	 int p=0;

	 for(p=0; p<12;p++)
	 robread.macroOp[p] = macroOperation[p];

	 for(p=0; p<23; p++)
	 robread.microOp[p] = microOperation[p];

    if (result != 14) {
      fprintf(stderr, "Error parsing trace at line %" PRIi64 "\n", totalMicroops);
      abort();
    }

    // For each micro-op

    totalMicroops++;
    robread.seqno = totalMicroops;


    //fprintf(outputFile, "%" PRIi64 ",", totalMicroops);
    // For each macro-op:
    if (microOpCount == 1) {
      totalMacroops++;
    }


    if(sourceRegister1 != -1)
    {
    	temp1=maptable[arch_reg[sourceRegister1]];
    	robread.sourceareg[0]=sourceRegister1;
    	robread.sourcepreg[0]=temp1;
    	//fprintf(outputFile, "r%d ->",arch_reg[sourceRegister1]);
    	//fprintf(outputFile, " p%d",temp1);

    }

    if(sourceRegister2 != -1)
    {
    	//fprintf(outputFile, ",");
    	temp2=maptable[arch_reg[sourceRegister2]];
    	robread.sourceareg[1]=sourceRegister2;
    	robread.sourcepreg[1]=temp2;
    	//fprintf(outputFile, "r%d ->",arch_reg[sourceRegister2]);
    	//fprintf(outputFile, " p%d",temp2);
    }

    if(destinationRegister != -1)
    {   //fprintf(outputFile, ",");

    	temp = maptable[arch_reg[destinationRegister]];
    	maptable[destinationRegister] = phy_reg.front();
    	robread.destareg[0]=destinationRegister;
    	robread.destpreg[0]=maptable[destinationRegister];
    	robread.destfree[0]=temp;
    	scoreboard[robread.destpreg[0]] = -1;
    	phy_reg.pop_front();
    	phy_reg.push_back(temp);

    	//fprintf(outputFile, "r%d ->",arch_reg[destinationRegister]);
    	//fprintf(outputFile, " p%d",maptable[destinationRegister]);
    	//fprintf(outputFile, " [p%d]",temp);
    }

    if(conditionRegister != '-')
        {
        	if(conditionRegister == 'R')
        	{

        		tempf1=maptable[arch_reg[49]];
        		robread.sourceareg[2]=49;
        		robread.sourcepreg[2]=tempf1;
        		//fprintf(outputFile, "r%d ->",arch_reg[49]);
        	   // fprintf(outputFile, " p%d",tempf1);
        	}
        	else
        	{
        	  	    tempf2 = maptable[arch_reg[49]];
        	    	maptable[49] = phy_reg.front();
        	    	robread.destareg[1]=49;
        	    	robread.destpreg[1]=maptable[49];
        	    	scoreboard[robread.destpreg[1]] = -1;
        	    	robread.destfree[1]=tempf2;
        	    	phy_reg.pop_front();
        	    	phy_reg.push_back(tempf2);

        	    	//fprintf(outputFile, "r%d ->",arch_reg[49]);
        	    	//fprintf(outputFile, " p%d",maptable[49]);
        	    	//fprintf(outputFile, " [p%d]",tempf2);

        	}

        }

    if(loadStore == 'L')
    	robread.isLoad = 1;
    else if(loadStore == 'S')
    	robread.isStore = 1;

    if((robread.isLoad == 1) || (robread.isStore==1))
    {
    	robread.addressls = addressForMemoryOp;
    }

    //branch-prediction

    if(conditionRegister == 'R' && TNnotBranch != '-')
    	{
    	//totalBranches++;

    	index = (instructionAddress ^ THistory) & (size-1);


    	if(TNnotBranch == 'T')
    	THUpdate = 1;
    	else if(TNnotBranch == 'N')
    	THUpdate = 0;

        if( (BM[index] == 't') || (BM[index] == 'T'))
    	prediction = 'T';
    	else if( (BM[index] == 'n') || (BM[index] == 'N'))
    	prediction = 'N';

    	if(prediction == TNnotBranch)
    	{
    	if((BM[index]=='N')||(BM[index]=='n'))
    	BM[index]='N';
    	else if((BM[index]=='T')||(BM[index]=='t'))
    	BM[index]='T';
    	}
    	else
    	{
    	//incorrectCount++;

    		fetchready = -1;
    		robread.ismispredict = 1;

    	if((BM[index]=='N')||(BM[index]=='t'))
    	BM[index]='n';
    	else if((BM[index]=='T')||(BM[index]=='n'))
    	BM[index]='t';
    	}
    	THistory = THistory << 1;
    	THistory = THistory & TVal;
    	THistory = THistory | THUpdate;
    	}

   // fprintf(outputFile, " | ");
  //  fprintf(outputFile, "%s ",macroOperation);
   // fprintf(outputFile, "%s \n",microOperation);


  rob.push_back(robread);

  }

       if(TNnotBranch=='T')
    	   break;
	}


//advance to next cycle
	currentcycle++;
	int k = 0;

	for(k=0; k<pregsize; k++)
	{
		if((scoreboard[k])>0)
		{
			scoreboard[k]=scoreboard[k]-1;
		}

	}


	//if(rob.size()==0)
		//break;
	if(exitflag==1 && rob.size()==0)
		break;



}
 // fprintf(outputFile, "%d ",log2(N));
  fprintf(outputFile, "%d ",N);

  fprintf(outputFile, "%d ",currentcycle);
  fprintf(outputFile, "%  " PRIi64 "\n", totalMicroops);


}

int main(int argc, char *argv[])
{

	FILE *inputFile = stdin;
  FILE *outputFile = stdout;
  unsigned int N;
  int something = 0;



  for(something=5;something<=10;something++)
  {
  if (argc >= 2) {
    inputFile = fopen(argv[1], "r");
    assert(inputFile != NULL);
  }
  if (argc >= 3) {
    outputFile = fopen(argv[2], "a+");
    assert(outputFile != NULL);
  }

  N = pow(2,something);
  simulate(inputFile, outputFile, N);
  fclose(inputFile);
  fclose(outputFile);
  }
  return 0;
}
