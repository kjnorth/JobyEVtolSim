// Defines any function that generates random numbers to be used in the simulation

#pragma once

#include "Aircraft.h"

void CreateRandomAircraftIds(Aircraft* aircrafts); // takes array of aircrafts as parameter
void HourlyFaultGenerationPercentChance(Aircraft* plane); // takes one single plane as parameter
void FaultTest(void);