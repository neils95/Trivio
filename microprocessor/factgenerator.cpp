#include "factgenerator.h"

#include <Arduino.h>

const int numFact = 8;
const int maxFactSize = 140;

// generate facts for testing purposes
char factArray[][maxFactSize] = {
  "The young kangaroo, or joey, is born alive at a very immature stage when it is only about 2 cm long and weighs less than a gram.",
  "Pteronophobia is the fear of being tickled by feathers!",
  "A flock of crows is known as a murder.",
  "Cherophobia is the fear of fun.",
  "Human saliva has a boiling point three times that of regular water.",
  "You cannot snore and dream at the same time.",
  "Recycling one glass jar saves enough energy to watch TV for 3 hours.",
  "The Titanic was the first ship to use the SOS signal.",
  /*"J.K. Rowling chose the unusual name ‘Hermione’ so young girls wouldn’t be teased for being nerdy!",
  "In the Caribbean there are oysters that can climb trees.",
  "A duel between three people is actually called a truel.",
  "The television was invented only two years after the invention of sliced bread.",
  "Alligators will give manatees the right of way if they are swimming near each other.",
  "Sunsets on Mars are blue."*/
};

void FACTGENERATOR::getFact(int index, char *fact) {
	if(index < numFact) {
		for( int i = 0; i < maxFactSize; i++) {
			*fact = factArray[index][i];
			fact++;
			if(factArray[index][i] == '\0') break;
		}
	}
}

int FACTGENERATOR::getNumFacts() {
	return numFact;
}