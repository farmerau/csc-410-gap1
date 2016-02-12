//! An individual of a generation of a genetic algorithm
/*! ****************************************************************
  file: individual.h
  
  This class defines the individual "solution" genome to the problem
  of guessing the target picture. 

  ADD WHAT YOU HAVE CHANGED and list your group members.
  **************************************************************** */

class 
individual {
 public:
  /*! \brief Default constructor

    Function constructor, not knowing in advance the number of
    genes. We need to deallocate (not necessary) and null everything
    to make sure that we do not run the GA with incorrect genomes.
  */
  individual();

  /*! \brief Default constructor with number of genes known

    Function constructor, knowing in advance the number of genes and
    the possible max and min values, respectively.
    There is no error checking to make sure that the max > min, for example.
  */
  individual( const unsigned int, const int, const int );

  /*! \brief Destructor that removes the myGenome array if it exists */
  ~individual();

  /*! \brief Given an index, it returns the address of the element at
    that index.

    Precondition: the myGenome array has been allocated. No error
    checking occurs here.

    Postcondition: If the genome is not allocated or index >= numGenes,
    returns zero as an indicator
  */
  int& operator [] ( const int ) ;
  int get(int index);

  /*! \brief Returns the fitness of the individual */
  double get_my_fitness() const;

  /*! \brief Mutator function to set the possible max and min values
 
    precondition: none, error checking to make sure max >= min is done
    within
  */
  void set_max_min_values( const int, const int );

  /*! \brief Mutator function to set value at myGenome[index]

    precondition: none, error checking inserted to make sure second
    parameter < numGenes. First parameter = value, second = index
  */
  void set_value( const int, const unsigned int );

  /*! \brief Mutator function to copy values to myGenome[0] from input
     array of integer values. The second parameter is the number of
     genes to copy.
    
    precondition: none, error checking inserted to make sure nGenes <
    numGenes. First parameter = values, second = index
  */
  void copy_genome( int*, const unsigned int );

  /*! \brief Function to create a blank genome.
  
    This function allocates space only and is called by the
    paramterized constructor and by itself to create the initial
    population.
  */
  void allocate_genome( const unsigned int );

  /*! \brief Functions to create a randomly assigned genome 
  
    Precondition: the genome has been allocated, but error checking
    is done.  The max and min possible values have been set.
  */
  void randomize_genome();

  /*! \brief Function to calculate the fitness
  
    precondition: none, as whether the genome exists or not is checked
    inside the function.
    
    postcondition: the variable myFitness stores the fitness of this
    genome based on the total percent difference from 
    the target picture.
  */
  void calculate_fitness( int**, const unsigned int, const unsigned int );

  /*! \brief Function to mutate the genome, given a mutation rate */
  void mutate( const double );

 private:
  int* myGenome;   ///< the actual array of information representing a "chromosome". For this problem, these are integers.
  unsigned int numGenes;    ///< the number of genes in my genome. We need to keep track for crossover and mutation
  double myFitness; ///< The result of applying the fitness function.
  double maxValue;  ///< the maximum value allowed in the genome - used by randomize and mutate
  double minValue;  ///< the minimum value allowed in the genome - used by randomize and mutate
};
