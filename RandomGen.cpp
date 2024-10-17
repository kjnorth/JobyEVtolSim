#include <random>

#include "RandomGen.h"

#include "AircraftCompanyCommon.h"

void CreateRandomAircraftIds(Aircraft* aircrafts) {
    // Use current time as seed for random generator
    srand(time(0));

    bool verifyAllIdsGenerated[TOTAL_AIRCRAFT_COMPANIES] = {false};
    for (uint8_t i = 0; i < TOTAL_AIRCRAFTS_IN_SIMULATION; i++) {
        aircraft_id_t randomId = (aircraft_id_t) (rand() % 5);
        aircrafts[i].m_id = randomId;
        verifyAllIdsGenerated[randomId] = true;
    }

    /** Verify at least one of each aircraft company exists in the array. If this
     * test fails, simply exit and rerun simulation. If this fails frequently, we
     * can come up with a method to populate the array with the missing ID(s). */
    for (uint8_t i = 0; i < TOTAL_AIRCRAFT_COMPANIES; i++) {
        if (verifyAllIdsGenerated[i] == false) {
            std::cout << "ERROR - all aircraft IDs were not generated." << std::endl;
            std::cout.flush();
            exit(EXIT_FAILURE);
        }
    }
}

void HourlyFaultGenerationPercentChance(Aircraft* plane) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::bernoulli_distribution d(GetFaultProbabilityPerHour(plane->m_id));
    if (d(gen)) {
        plane->m_numFaults++;
    }
}

void FaultTest(void) {
    /** The alpha aircraft company has a 25% chance of a fault occurring.
     * Run this aircraft through the HourlyFaultGenerationPercentChance function
     * 1000 times. The number of faults should be about 250.
     * 
     * This test is passing. */
    Aircraft plane(AIRCRAFT_ID_ALPHA);
    for (int i = 0; i < 1000; i++) {
        HourlyFaultGenerationPercentChance(&plane);
    }
    std::cout << "Fault test should produce around 250 faults. Result: " << plane.m_numFaults << std::endl;
}