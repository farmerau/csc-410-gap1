// The driver file.
/********************************************************************
  file: PictureGA_dr.cpp
  
  This file contains the main driver of the program. Right now, it
  only creates an instance of the PictureGA object, calling its
  constructor.

  Names of the pair programming members:
    1) Driver - Austin Farmer

  Comments team members want to add about collaboration:
    Rissa and I ended up working very separately. What we did instead
    was talk back and forth about our progress. After class I will be
    meeting her to discuss what we'd like to change about how we wor-
    ked this time. As it turns out, she had concerns about the work
    and (not to my knowing) was afraid to let me know she didn't un-
    derstand parts of the project. I hope to resolve this.

  ******************************************************************** */

#include "pictureGA.h"
#include <iostream>
using namespace std;

int 
main( int argc, char** argv ) {
  int numIndividuals;

  cout << "How many individuals are there in the population?";
  cin >> numIndividuals;

  pictureGA myPicture(numIndividuals);
myPicture.go();
  return 0;
}
