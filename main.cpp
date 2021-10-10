#include <iostream>
#include <fstream>
#include <random>
#include <cstring>
#include <iomanip>
#include <chrono>

#define ROUND 100
#define GENERATION 100
#define POPULATION 100
#define MAX_POSITION 100.0
#define MIN_POSITION -100.0
#define CROSSOVER_RATE 0.5
#define MUTATION_RATE 0.5
#define MUTATE_POINT 1  // The number of the mutate point

using namespace std;

typedef struct Individuals {
    int gene[13];
    long double value;
    long double fitness;
} Individuals;

Individuals individual[POPULATION];
Individuals pool[POPULATION];
Individuals best;

int random(int start, int end) {
    random_device rd;
    default_random_engine gen = default_random_engine(rd());
    uniform_int_distribution<int> dis(start, end);
    return dis(gen);
}

void initialize() {
    for (int i = 0; i < POPULATION; i++) {
        individual[i].value = 0;
        individual[i].fitness = INT_MAX;
        pool[i].value = 0;
        pool[i].fitness = INT_MAX;
        for (int j = 0; j < 13; j++) {
            individual[i].gene[j] = random(0, 1);
            pool[i].gene[j] = 0;
        }
    }
    for (int i = 0; i < 13; i++) {
        best.gene[i] = 0;
    }
    best.value = 0;
    best.fitness = INT_MAX;
}

void calcFitness() {
    for (int i = 0; i < POPULATION; i++) {
        individual[i].value = 0.0;
        individual[i].fitness = INT_MAX;
        for (int j = 0; j < 13; j++) {
            if (individual[i].gene[j] == 1) {
                individual[i].value += (long double) (pow(2, j - 6));
            }
        }

        if (individual[i].value > MAX_POSITION) {
            individual[i].value = MAX_POSITION;
        } else if (individual[i].value < MIN_POSITION) {
            individual[i].value = MIN_POSITION;
        }

        individual[i].fitness = (long double) abs(individual[i].value);

        if (individual[i].fitness < best.fitness) {
            memcpy(&best, &individual[i], sizeof(best));
        }
    }
}

void select() {
    int i1, i2;
    for (int i = 0; i < POPULATION; i++) {
        i1 = random(0, POPULATION - 1);
        do {
            i2 = random(0, POPULATION - 1);
        } while (i1 == i2);
        if (individual[i1].fitness < individual[i2].fitness) {
            memcpy(&pool[i], &individual[i1], sizeof(pool[i]));
        } else {
            memcpy(&pool[i], &individual[i2], sizeof(pool[i]));
        }
    }
}

void crossover() {
    int i1, i2;
    for (int i = 0; i < POPULATION; i += 2) {
        if ((random(0, 100) / 100.0) > CROSSOVER_RATE) {
            continue;
        }

        i1 = random(0, POPULATION - 1);
        do {
            i2 = random(0, POPULATION - 1);
        } while (i1 == i2);

        int crossoverPosition = random(0, 12);
        for (int j = 0; j < 13; j++) {
            if (j < crossoverPosition) {
                individual[i].gene[j] = pool[i1].gene[j];
                individual[i + 1].gene[j] = pool[i2].gene[j];
            } else {
                individual[i].gene[j] = pool[i2].gene[j];
                individual[i + 1].gene[j] = pool[i1].gene[j];
            }
        }
    }
}

void mutate() {
    int position;
    for (int i = 0; i < POPULATION; i++) {
        if (((random(0, 100) / 100.0)) > MUTATION_RATE) {
            continue;
        }
        for (int j = 0; j < MUTATE_POINT; j++) {
            position = random(0, 12);
            individual[i].gene[position] = 1 - individual[i].gene[position];
        }
    }
}

int main() {
    long double best_value = INT_MAX, worst_value = 0.0, total = 0.0;

    // count execution time
    auto start = chrono::steady_clock::now();

    ofstream ofs;
    ofs.open("output.txt");
    ofs << "Round: " << ROUND << endl;
    ofs << "Generation: " << GENERATION << endl;
    ofs << "Population: " << POPULATION << endl;
    ofs << "Crossover rate: " << CROSSOVER_RATE << endl;
    ofs << "Mutation rate: " << MUTATION_RATE << endl;
    ofs << "Mutation type: " << MUTATE_POINT << " point(s)" << endl;
    ofs << "============================" << endl;

    for (int j = 0; j < ROUND; j++) {
        initialize();
        for (int i = 0; i < GENERATION; i++) {
            calcFitness();
            select();
            crossover();
            mutate();
        }

        total += best.fitness;
        if (best.fitness < best_value) {
            best_value = best.fitness;
        } else if (best.fitness > worst_value) {
            worst_value = best.fitness;
        }
        ofs << "Round: " << j + 1 << " / " << ROUND << " --- " << best.fitness << endl;
    }

    ofs << "============================" << endl;
    ofs << "Average value: " << (long double) total / ROUND << endl;
    ofs << "Worst value: " << worst_value << endl;
    ofs << "Best value: " << best_value << endl;
    ofs << "============================" << endl;

    // count execution time
    auto end = chrono::steady_clock::now();
    cout << "Time taken: " << chrono::duration<double>(end - start).count() << " s" << endl;

    ofs << "Time taken: " << chrono::duration<double>(end - start).count() << " s" << endl;
    ofs.close();
}