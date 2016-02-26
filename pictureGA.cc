#include "pictureGA.h"
#include <fstream>
#include <string>
#include <iostream>
#include <cstdlib>
#include <algorithm>
#include <math.h>
using namespace std;

pictureGA::pictureGA( int popSize ) {
  maxGenerations = 10000000;
  targetFitness = 0.9;
  numIndividuals = 0;
  generation = NULL;
  srand(time(NULL));
  input_target_picture();
  //Let's do some work for multiple crossover.
  cout << "How many crossover points would you like?\n";
  numCrossover = 0;
  //This while loop will ensure that we get a valid input for # of crossovers.
  while ((numCrossover < 1)||(numCrossover > (nRows*nCols))){
    cout << "Based on your input file, the value must be between 1 and " << nRows * nCols << ". ";
    cin >> numCrossover; cout << "\n";
  }
  //Dynamically allocate number of crossovers based on validated input.
  crossoverPts = new int[numCrossover];
  //Ask the user what kind of mutation they're looking at.
  cout << "What is the desired mutation rate? ";
  cin >>  mutationRate; cout << "\n";
  /*
  If the user enters a value that is greater than one, we're going to assume
  that they're probably in percentage form. (1 = 1%, 100 = 100%)
  If the user enters a value less than one, we will keep it in decimal form.
  (.99 = .99, 0.00000001 = 0.00000001)
  */
  if (mutationRate >= 1){
	mutationRate = mutationRate /100;
  }
  mutationLowerBound = mutationRate;
  mutationUpperBound = mutationRate-1; //So that we always enter while loop.
  //While loop to ensure we get a value bigger than the mutationRate.
  while (mutationUpperBound < mutationRate){
    cout << "What is the upper bound for mutation? (Must be higher than or equal to mutation rate) ";
    cin >> mutationUpperBound; cout << "\n";
  }
  if (mutationUpperBound >= 1){
    mutationUpperBound = mutationUpperBound /100;
  }
  // time to create the population. Allocate space and randomize
  generation = new individual[popSize];
  int minv = targetPicture[0][0]; //Temp Minimum variable
  int maxv = targetPicture[0][0]; //Temp Max variable;
  for (int i=0; i<nRows; i++){
	for (int j=0; j<nCols; j++){
		if (targetPicture[i][j] > maxv){
			maxv = targetPicture[i][j];
		}
		else if (targetPicture[i][j] < minv){
			minv = targetPicture[i][j];
		}
	}
  }
  for( int i=0; i<popSize; i++ ) {
    generation[i].allocate_genome( nRows * nCols );
    generation[i].set_max_min_values(maxv, minv);
    generation[i].randomize_genome();
  }
  numIndividuals = popSize;


}

pictureGA::~pictureGA() {
  if( targetPicture ) {
    for( int i=0; i<nRows; i++ )
      if( targetPicture[i] ) delete [] targetPicture[i];
    delete [] targetPicture;
  }
  if( generation ) delete [] generation;
  //This line added just to make sure we're playing safe with memory.
  delete crossoverPts;
}

void
pictureGA::go() {
  int numGeneration = 0;
  double fitnessDifference = 0;;
  cout << "\033[2J"; //Clear the screen.
  do {
    //\003[;\033[1;1H Places terminal cursor at 0,0.
    cout << "\033[1;1HGENERATION: " << numGeneration << "\n";
    fitnessDifference = calculate_fitnesses();
    //cout << "Starting Roulete...\n";
    roulettify();
    //cout << "Roulette finished...\nStarting Spawn...\n";
    spawn();
    //cout << "Spawn finished...\nStarting Irradiation...\n";
    /*
    This if statement's purpose is to activate variable mutation when needed.
    If the fitness gets under two, we pick a random mutation rate that is between
    the lower bound and the upper bound of mutation.
    */
    if ((targetFitness - fitnessDifference) < 2){
      mutationRate = (double)rand() / RAND_MAX;
      mutationRate = (mutationRate *(mutationUpperBound - mutationLowerBound)) + mutationLowerBound;
      cout << "Activated variable mutation.\n"; //let user know that we've done something.
    }
    irradiate();
    //cout << "Irradtiation Complete...\n";
    numGeneration ++;

  } while ( numGeneration < maxGenerations && fitnessDifference > targetFitness);
  cout << "Best fitness: " << bestfitness << "\n";
}

/***********************************************************
 *  PRIVATE MEMBER FUNCTIONS
 ***********************************************************/
void
pictureGA::input_target_picture() {

  ifstream inFile;

  string filename;

  cout << "What is the name of the input file?";
  cin >> filename;
  inFile.open( filename.c_str());

  if( inFile.fail() ) {
    cout << "Error opening " << filename << endl;
    exit(1);
  }

  /*!
    1. Open the file after asking the user for the filename. There are
    several error-checking going on.
    2. Read the size of the image (number of rows and columns ) as
    the first two numbers in this file.
    3. Allocate the rows
    4. For each row, create the columns and then read the values int them
  */
  inFile >> nRows;
  inFile >> nCols;

  targetPicture = new int*[nRows];
  if( !targetPicture ) {
    cout << "Error allocating targetPicture!\n";
    exit(2);
  }

  for( int i=0; i<nRows; i++ ) {
    targetPicture[i] = new int[nCols];
    for( int j=0; j<nCols; j++ )  {
      inFile >> targetPicture[i][j];
    }
  }

  inFile.close();
}

double
pictureGA::calculate_fitnesses() {
	double avg = 0.0;
	//Because I never found where the individuals were actually checked for
	//fitness I took it into my own hands.
	for (int i=0; i<numIndividuals; i++){
		generation[i].calculate_fitness(targetPicture, nRows, nCols);
		//This is just a summation of fitnesses.
		avg += generation[i].get_my_fitness();
	}
        if (avg/numIndividuals < bestfitness){
		bestfitness = avg/numIndividuals;
	}
        cout << "Best fitness: " << bestfitness << ", AVG: " << avg/numIndividuals << "\n";
	return avg/numIndividuals; //Calculate average fitness before return
}

void
pictureGA::roulettify() {

  double* rouletteValues = new double[numIndividuals];
  double rouletteSoFar(0);
  int bestParent = 0;
  int nextBest = 0;
  // find the total fitness before dividing.
  double totalFitness = 0;
  for( int i =0; i<numIndividuals; i++ ){
    totalFitness+=(100-generation[i].get_my_fitness());
    if (generation[i].get_my_fitness() < generation[bestParent].get_my_fitness()){
		bestParent = i;
    }
    else if (generation[i].get_my_fitness() < generation[nextBest].get_my_fitness()){
		nextBest = i;
	}
  }
  //I've elected to remove the meat from Roulettify, because I like convergence.
  parent1 = bestParent;
  parent2 = nextBest;
}

void
pictureGA::spawn() {

  int p1(parent1), p2(parent2);   // parent indices
  unsigned int crossover(0);
  unsigned int i(0);
  /*
  We're declaring two parents so that we can copy them from the old generation.
  This will prevent us from writing over the parents during the creation of the
  new generation.
  */
  int * par1 = new int[nRows*nCols];//essentially a genome
  int * par2 = new int[nRows*nCols];//essentially a genome.
  for (int z = 0; z < (nRows*nCols); z++){
    /*
    We/re going to take advantage of the overloaded array subscript operator
    and copy the entire genome from the parents in the genome to our temporary
    parents home. (Sounds like a nursing home now, huh?)
    */
    par1[z] = generation[p1][z];
    par2[z] = generation[p2][z];
  }
  //We're going to iterate through each of the individuals so that we make new ones.
  for (i; i < numIndividuals; i++){
    //We need to assing the crossover points to the crossover arrray.
    for (int j = 0; j < numCrossover; j++){
      crossoverPts[j] = 0;
    }
    //This loop should ensure uniqueness of individuals.
    for (int j = 0; j < numCrossover; j++){
      bool flag = false;
      int temp;
      while (!flag){
        temp = ((int) rand() % (nRows*nCols));
        for (int k = 0; k < numCrossover; k++){
          if (crossoverPts[k] == temp){
            break;
          }
          else if ((k + 1) == numCrossover){
            flag = true;
          }
        }
      }
      crossoverPts[j] = temp;
    }
    //Now we're going to sort the crossover points using std::sort.
    crossoverSort();
    int crossIndex = 0;
    bool parentChooser = false;
    for (int j = 0; j < (nRows * nCols); j++){
      //Let's check to see if we've hit a crossover point.
      if (crossoverPts[crossIndex] == j){
        //We did! Let's instruct the copier to start copying from other parent.
        parentChooser =!parentChooser;
        if((crossIndex+1)<numCrossover){
          //This increments our crossIndex IF we have another one in the array.
          crossIndex++;
        }
      }
      //We're using parentChooser as a pivot.
      if(parentChooser){
        generation[i].set_value(par2[j], j);
        //We're checking to see if there's an individual after this one.
        if ((i+1) < (nRows*nCols)){
          //There's an individual after this one.
          i++;
          //We're at C2 instead of C1 now.
          generation[i].set_value(par1[j], j);
          //Store value into C2.
          if (j < ((nRows * nCols)-1)){
            //If we aren't already at the end of the genome, return to C1.
            i--;
          }
        }
      }
      else{
        //I'm electing to not comment this section because of it's similarity
        //to the segment inside the if(){}.
        generation[i].set_value(par1[j], j);
        if ((i+1) < (nRows*nCols)){
          i++;
          generation[i].set_value(par2[j], j);
          if (j < ((nRows * nCols)-1)){
            i--;
          }
        }
      }
    }
  }
//Let's practice safe memory usage.
delete [] par1;
delete [] par2;
}
void pictureGA::crossoverSort(){
  //This is a simple sort function. Should do the trick.
  for (int i = 0; i < numCrossover; i++){
    if (i+1 < numCrossover){
      if (crossoverPts[i] > crossoverPts[i+1]){
        int j = i;
        while((j+1 < numCrossover)&&(crossoverPts[j] > crossoverPts[j+1])){
          int temp = crossoverPts[j];
          crossoverPts[j] = crossoverPts[j+1];
          crossoverPts[j+1] = temp;
          int k = j;
          while (((k-1) > -1)&&(crossoverPts[k] < crossoverPts[k-1])){
            temp = crossoverPts[k];
            crossoverPts[k] = crossoverPts[k-1];
            crossoverPts[k-1] = temp;
            k--;
          }
          j++;
        }
      }
    }
  }
}
void
pictureGA::irradiate() {
  for (int i =0; i < numIndividuals; i++){
	generation[i].mutate(mutationRate);
  }
}
