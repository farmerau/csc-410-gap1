#include "pictureGA.h"
#include <fstream>
#include <string>
#include <iostream>
#include <cstdlib>
using namespace std;

pictureGA::pictureGA( int popSize ) {
  numIndividuals = 0;
  generation = NULL;
  srand(time(NULL));
  /*! 
    1. get the target picture with the default filename "picture.in"
    2. allocate the initial population and then randomize them.
  */

  /*! Notes:
     1. Need to find the max and min allowed values for the genome
     from somewhere! Should it be from reading the target picture, or
     raw possible greyscale values, assuming a single byte per pixel?
  */
  input_target_picture();
  // We never actually ask them for the mutation rate, so I capture it.
  cout << "What is the desired mutation rate? ";
  cin >>  mutationRate; cout << "\n";
  if (mutationRate >= 1){
	mutationRate = mutationRate /100;
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
}

void 
pictureGA::go() {
  int numGeneration = 0;
  double fitnessDifference = 0;
  do {
    fitnessDifference = calculate_fitnesses();
    roulettify();
    spawn();
    irradiate();
    numGeneration ++; 

  } while ( numGeneration < 10000000 && fitnessDifference > 0.9 );
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
//		cout << "Fitness of individual " << i << " " << generation[i].get_my_fitness() << "\n";
		avg += generation[i].get_my_fitness();
	}
//	cout << "Value of avg: " << avg << " numIndividuals: " << numIndividuals << "\n";
	//cout << "Average fitness: " << avg/numIndividuals << ".\n";
        if (avg/numIndividuals < bestfitness){
		bestfitness = avg/numIndividuals;
	}
        //cout << "Best fitness: " << bestfitness << "\n";
	return avg/numIndividuals; //Calculate average fitness before return
}

void
pictureGA::roulettify() {

  double* rouletteValues = new double[numIndividuals];
  double rouletteSoFar(0);
  int bestParent = 0;
  int nextBest = 0;
  /// YOU MUST COMPLETE THIS FUNCTION

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

  for( int i =0; i<numIndividuals; i++ ) 
    rouletteValues[i] = 0.0;

  for( int i =0; i<numIndividuals; i++ ) {
    rouletteSoFar += (100-generation[i].get_my_fitness())/totalFitness;
    rouletteValues[i] = rouletteSoFar;
	//cout << rouletteSoFar << " ";
  }
  
  // Once the roulette values are in place, find the parent indices
parent1 = 0;
parent2 = 0;

double select = ((double) rand() / (RAND_MAX)) + 1;
select --;
cout << select << " selected .\n";
for (int j = 0; j < numIndividuals; j++){
	if (rouletteValues[j] <= select){
		if (j > 0){
			if (rouletteValues[j] > rouletteValues[j-1]){
				parent1 = j;
			}
		}
		else{
			parent1 = j;
		}
	}
}
do{
	select = ((double) rand() / (RAND_MAX)) + 1;
	select--;
	for (int j = 0; j < numIndividuals; j++){
	if (rouletteValues[j] <= select){
		if (j > 0){
			if (rouletteValues[j] > rouletteValues[j-1]){
				parent2 = j;
			}
		}
		else{
			parent2 = j;
		}
	}
}
}
while (parent1 == parent2);

  cout << "Results of roulette: Best Parent: " << bestParent << " Next Best: " << nextBest << " p1 " << parent1 << " p2 " << parent2 << "\n";
 parent1 = bestParent;
  parent2 = nextBest;
  // To find P1 and P2, we need to generate two random numbers from 0
  // to 100. Because the total of all the candidates' fitness is 100%,
  // picking somewhere in the range would give the parents. A nuance
  // is that the parents have to be different, so the indices must be
  // checked to be different.
  /// YOU MUST COMPLETE THE REST OF THIS FUNCTION
}

void 
pictureGA::spawn() {
  
  int p1(parent1), p2(parent2);   // parent indices
  unsigned int crossover(0);
  unsigned int i(0);
 // srand(time(NULL));
  //srand needed to seed the random function.
  int * par1 = new int[nRows*nCols];//essentially a genome
  int * par2 = new int[nRows*nCols];//essentially a genome.

  //This for loop will copy the genomes from the selected parents. This
  //is necessary to prevent us from overloading the values inside the
  //population before we finish our breeding.
  for (unsigned int z = 0; z < (nRows*nCols); z++){
	par1[z] = generation[p1][z];
        //cout << p1 << "p1 :: " << z << "z :: " << generation[p1][z] << "p1z\n";
	par2[z] = generation[p2][z];
	//cout << p2 << "p2 :: " << z << "z :: " << generation[p2][z] << "p2z\n";
  }
  for (int z = 0; z < (nRows*nCols); z++){
	//cout << par1[z] << ";";
  }
  //cout << "\n------------------------------\n";
  for (int z = 0; z < (nRows*nCols); z++){
	//cout << par2[z] << ";";
  }

 for (i; i < numIndividuals; i++){
	//cout << "******************INDIVIDUAL " << i << "****************************\n";
	do
	{
	crossover =  ((int) rand() % (nRows*nCols));
	}
		//WE NEED TO PREVENT CROSSOVER IN NONSENSE LOCATIONS
	while ((crossover == (nRows*nCols)) || (crossover == 0));
	//cout << crossover << "\n";
	//THIS IS WHERE STUFF IS ABOUT TO GET MUDDY.
	//WE'RE GOING TO DEAL WITH THE GENERATION OF TWO INDIVIDUALS
	//PER LOOP, FOR THE SAKE OF CONSISTENT CROSSOV
	for (unsigned int j = 0; j < (nRows*nCols); j++){
		if (j <= crossover){
			generation[i].set_value(par1[j], j);
		}
		else{
			generation[i].set_value(par2[j], j);
		}
	}
	if (i != (numIndividuals -1)){
		i++;
		for (unsigned int j = 0; j < (nRows*nCols); j++){
			if (j <= crossover){
				generation[i].set_value(par2[j], j);
			}
			else{
				generation[i].set_value(par1[j], j);
			}
		}
	}
	//cout << "***************END INDIVIDUAL " << i << "*****************************\n";

} 
  /// YOU MUST COMPLETE THIS FUNCTION; the given variables are suggestions
delete par1;
delete par2;
}

void 
pictureGA::irradiate() {
  /// YOU MUST COMPLETE THIS FUNCTION.
  ///
  /// Whatever mutation policy is up to you, but you must explain what
  /// you did in the comments.
  for (int i =0; i < numIndividuals; i++){
	generation[i].mutate(mutationRate);
  }

}
