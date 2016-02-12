#include "individual.h"
#include <cstdlib> // for "NULL", "exit()"
#include <iostream>

using namespace std;

individual::individual() { 
  myGenome = NULL; 
  numGenes = 0;
  myFitness = 0.0;
  maxValue = 0;
  minValue = 0;
}

individual::individual( const unsigned int nGenes, 
			const int max, 
			const int min ) {
  allocate_genome( nGenes );
  myFitness = 0.0;
  maxValue = max;
  minValue = min;
}

individual::~individual() {
  if( myGenome ) delete myGenome;
}

int&
individual::operator [] (const int index ){ 
  int o = 0;
  if( !myGenome ) exit(0);
  if( index > numGenes ) exit(0);
  return myGenome[index]; 
}

int individual::get(int index){
 if ( !myGenome ) return 0;
 if (index > numGenes ) return 0;
 return myGenome[index];
}

double 
individual::get_my_fitness() const {
  return myFitness;
}

void
individual::set_max_min_values( const int max, const int min ) {
  if( max < min ) return;
  maxValue = max;
  minValue = min;
}

void
individual::set_value( const int value, const unsigned int index ) {
  if( index >= numGenes ) return;
  //cout << "Set Gene " << index << " from " << myGenome[index] << " to " << value << ".\n";
  myGenome[index] = value;
}

void 
individual::copy_genome( int* input, const unsigned int nGenes ) {
  if( nGenes != numGenes ) return;
  if( nGenes < 0 ) return;
  for( int i=0; i<nGenes; i++ ) myGenome[i] = input[i];
}

void 
individual::allocate_genome( const unsigned int nGenes ) {
  /// if the genome has already been allocated, skip because we want to
  /// prevent memory leaks.
  if( myGenome ) return;

  /// Allocate space for the genome now
  myGenome = new int[nGenes];
  
  /// if failed, something is REALLY wrong, so bail out of the entire
  /// program.
  if( !myGenome ) exit(-1);

  /// Assign the numGenes used later to nGenes.
  numGenes = nGenes;
}

void 
individual::randomize_genome() {
  
  /// If there is no genome space allocated, do not do anything. 
  if( !myGenome ) return;

  /// If the max and min values are incorrect, then do nothing.
  if( minValue > maxValue ) return;

  /// find the random number, divide it by RAND_MAX (defined in
  /// cstdlib) to find the range, multiply it by the difference
  /// between max and min values and then add it to minValue to get
  /// the correct scaled range.
  //cout << "Inside";
  for( int i=0; i< numGenes; i++ ) {
    myGenome[i] = static_cast<int>(((rand()*1.0)/RAND_MAX)*(maxValue - minValue) + minValue);
   // cout << myGenome[i] << " :: ";
  }
 // cout << "\n";
}


#include <cmath>
/// YOU MUST COMPLETE THIS FUNCTION
/// precondition: targetPicture is a valid 2D array of integers and nRows and nCols are
///               correct values for the number of rows and columns in the target picture
///               that this genome is compare against.
/// postcondition: this function changes myFitness to reflect the fitness of this 
///                individual as the percent of the gene that MATCHES the target 
///                picture after a gene by gene comparison. 
void 
individual::calculate_fitness( int ** targetPicture, 
			       const unsigned int nRows, 
			       const unsigned int nCols ) {
  /// if the genome is not defined, then skip this step.
//	#include <stdio.h>
	if( !myGenome ) return;
	 double difference = 0.0;
	//For each of the genes, we calculate the absolute value of the diff
	//beteen two corresponding pixel values. We want absolute val because
	//it does us no good to add to one thing and then subtract from
	//another.
	for (int i = 0; i<numGenes; i++){
		difference += abs(myGenome[i]-targetPicture[i/nCols][i%nCols]);
//		cout << "Difference between genome[" << i << "] and targetPicture[" << i/nCols << "][" << i%nCols << "] is " << myGenome[i] << " - " << targetPicture[i/nCols][i%nCols] << "\n";
	}
 
  myFitness = difference/numGenes;
}

/// YOU MUST COMPLETE THIS FUNCTION
/// precondition: none, as whether the genome exists or not is checked
///               inside the function.
///
/// postcondition: given mRate probability, an element in the genome will 
///                be changed to a randomly allowed value.
void
individual::mutate( double const mRate ) {
//cout << maxValue << " " << minValue << "\n";
  // Check to make sure that the genome exists before trying to
  // manipulate it.
 	if( !myGenome ) return;
	for(int i = 0; i<numGenes; i++){
		//If the gene is below the mutation rate, mutate it.
		double randomNumber = (rand()*1.0)/RAND_MAX;
//		cout << mRate << " ";
//		cout << randomNumber; 
		if (randomNumber<mRate){
//			cout << "Mutation from " << myGenome[i];
			myGenome[i] = static_cast<int>(((rand()*1.0)/RAND_MAX)*(maxValue - minValue) + minValue);
//			cout << " to " << myGenome[i] << "\n";
		}
	}
  // go throughout the gene and, given a random probability, assign a gene to 
  // have the value between maxValue and minValue.
}
