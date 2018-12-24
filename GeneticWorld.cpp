// Developed by Manuel Llorca as a C++ adaptation of the algorithm provided in
// https://github.com/jsvazic/GAHelloWorld
/*
* The MIT License
*
* Copyright (c) 2018 Manuel Llorca
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*/

#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>
#include <climits>
#include <cmath>
#include <ctime>

using namespace std;

// Crossover probability (between 0 and 1). Change as needed.
#define CROSS_PROB 0.2;
// Mutation probability (between 0 and 1). Change as needed.
#define MUT_PROB 0.02;
// Percentage of individuals that are directly preserved through generations.
#define SEL_RAT 0.1;
// Number of generations (Greater than 0). Change as needed.
#define NUM_GEN 100;
// Number of individuals (Greater than 1). Change as needed.
#define NUM_IND 1000;
// Torunament size. Change as needed.
#define TORUNAMENT_SIZE 3;

// String to be found by the algorithm. Change as needed.
#define OBJECTIVE "Hello world!";

// Type defining a single individual.
struct tIndiv {
	int adapt; // Individual adaptation
	string info; // Genetic information
};

// Unused comparator, can be used for sorting according to a better adaptation.
struct IncreasingAdapt {
	bool operator()(const tIndiv &x, const tIndiv &y){
		return x.adapt < y.adapt;
	}
};

// Type defining the population of the algorithm.
struct tPop {
	int size; // Number of individuals
	int lchrom; // Chromosome length
	int bestChrom; // Position of the best individual
	double totalAdapt; // Sum of adaptations
	vector<tIndiv> popul; // Vector of individuals
};

// First values are given to population.
void initialize(tPop &population) {
	population.size = NUM_IND;
	// Needed conversion to string.
	string obj = OBJECTIVE;
	population.lchrom = obj.size();
	// New individuals.
	for (int i = 0; i < population.size; i++) {
		tIndiv toAdd;
		toAdd.info = "";
		// New chars for individual info.
		for (int j = 0; j < population.lchrom; j++) {
			char randChar;
			// 95 printable chars, 32 is the ASCII number of the first one.
			randChar = rand() % 95 + 32;
			toAdd.info.push_back(randChar);
		}
		population.popul.push_back(toAdd);
	}
}

// Individual evaluation.
void evaluateIndiv(tIndiv &indiv) {
	// Needed conversion to string.
	string obj = OBJECTIVE;
	indiv.adapt = 0;
	// Compared to the objective.
	for (int i = 0; i < obj.size(); i++) {
		// Distance to correct char at position i.
		indiv.adapt += abs(indiv.info[i] - obj[i]);
	}
}

// Population evaluation.
// Evaluates each individual and keeps track of the best one.
void evaluate(tPop &population) {
	population.bestChrom = 0;
	int currentBest = INT_MAX;
	for (int i = 0; i < population.size; i++) {
		evaluateIndiv(population.popul[i]);
		if (population.popul[i].adapt < currentBest) {
			// New Best individual found
			currentBest = population.popul[i].adapt;
			population.bestChrom = i;
		}
	}
}

// Selects one individual by making a tournament.
tIndiv tournament(tPop const &population) {
	// First randomly chosen individual.
	tIndiv chosen = population.popul[rand() % population.size];
	int tsize = TORUNAMENT_SIZE;
	// tsize - 1 : first individual already chosen.
	for (int j = 0; j < tsize - 1; j++) {
		int newPos = rand() % population.size;
		if (population.popul[newPos].adapt < chosen.adapt) {
			// Deterministic choosing: always choose the best individual.
			chosen = population.popul[newPos];
		}
	}
	return chosen;
}

// Selects some individuals that survive.
void select(tPop &population, tPop & newPopulation) {
	newPopulation.size = 0;
	newPopulation.lchrom = population.lchrom;
	double selectRatio = SEL_RAT;
	// Number of selected individuals according to the proportion.
	int selected = floor(selectRatio * population.size);
	for (int i = 0; i < selected; i++) {
		tIndiv chosen = tournament(population);
		// Individual is chosen for the next generation
		newPopulation.popul.push_back(chosen);
		newPopulation.size++;
	}
}

// Perfoms crossover between two parents.
void crossover(tPop &population, tPop &newPopulation) {
	for (int i = newPopulation.size; i < population.size; i++) {
		tIndiv father, mother, child;
		// Choosing parents by random tournament selection.
		father = tournament(population);
		mother = tournament(population);
		// Crossover point.
		int index = rand() % population.lchrom;
		child.info = "";
		// Information transmitted from father.
		for (int i = 0; i < index; i++) {
			child.info.push_back(father.info[i]);
		}
		// Information transmitted from mother.
		for (int i = index; i < population.lchrom; i++) {
			child.info.push_back(mother.info[i]);
		}
		newPopulation.popul.push_back(child);
	}
	newPopulation.size = population.size;
}

// Iterates through population and mutates if it has to.
void mutate(tPop &population) {
	for (int i = 0; i < population.size; i++) {
		// Generates a real number between 0 and 1.
		double mutation = static_cast <double> (rand()) / static_cast <double> (RAND_MAX);
		double mutationProb = MUT_PROB;
		// Mutation check.
		if (mutation < mutationProb) {
			// Random mutation index.
			int index = rand() % population.lchrom;
			// New random char is created at index position.
			population.popul[i].info[index] = rand() % 95 + 32;
		}
	}
}

// Algorithm execution.
int main() {
	// Random seed.
	srand(static_cast <unsigned> (time(0)));
	tPop population;
	initialize(population);
	evaluate(population); // Note: here a lower number means a better adaptation.
	// Needed int conversion.
	int maxGen = NUM_GEN;
	// Generations loop.
	for (int currentGen = 0; currentGen < maxGen; currentGen++) {
		cout << "Best individual of generation " << currentGen + 1 << ":\n";
		cout << population.popul[population.bestChrom].info << '\n';
		tPop newPopulation;
		select(population, newPopulation);
		crossover(population, newPopulation);
		population = newPopulation;
		mutate(population);
		evaluate(population);
	}
	system("PAUSE");
}