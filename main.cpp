/** E-Vtol simulation of 20 planes flying and recharging using only
 * 3 available chargers. */

#include <cstdlib> 
#include <ctime>
#include <iostream>
#include <queue>
#include <random>

#include "AircraftCompanyCommon.h"
#include "Aircraft.h"
#include "ChargeStation.h"
#include "Trace.h"

/** Use these definitions to test the simulation for different
 * amounts of time. Verify recorded data matches expectations
 * for 1, 2, and 3 hours of simulation time. */
#define SIMULATION_TICKS_1HR (60u * LOOP_TICKS_PER_MIN) // 60min
#define SIMULATION_TICKS_2HR (120u * LOOP_TICKS_PER_MIN)
#define SIMULATION_TICKS_3HR (180u * LOOP_TICKS_PER_MIN)

#define TOTAL_AIRCRAFTS_IN_SIMULATION       (20u)

/** Define data common to aircraft of the same company.
 * Use the aircraft id to index through this array. */
aircraft_company_common_t G_AircraftCompanyCommon[TOTAL_AIRCRAFT_COMPANIES] = {
    {ALPHA_CRUISE_SPEED_MPH, ALPHA_FLIGHT_DUR_TICKS, ALPHA_CHARGE_DUR_TICKS, ALPHA_PASSENGER_COUNT, ALPHA_FAULT_PROB_PER_HR},
    {BRAVO_CRUISE_SPEED_MPH, BRAVO_FLIGHT_DUR_TICKS, BRAVO_CHARGE_DUR_TICKS, BRAVO_PASSENGER_COUNT, BRAVO_FAULT_PROB_PER_HR},
    {CHARLIE_CRUISE_SPEED_MPH, CHARLIE_FLIGHT_DUR_TICKS, CHARLIE_CHARGE_DUR_TICKS, CHARLIE_PASSENGER_COUNT, CHARLIE_FAULT_PROB_PER_HR},
    {DELTA_CRUISE_SPEED_MPH, DELTA_FLIGHT_DUR_TICKS, DELTA_CHARGE_DUR_TICKS, DELTA_PASSENGER_COUNT, DELTA_FAULT_PROB_PER_HR},
    {ECHO_CRUISE_SPEED_MPH, ECHO_FLIGHT_DUR_TICKS, ECHO_CHARGE_DUR_TICKS, ECHO_PASSENGER_COUNT, ECHO_FAULT_PROB_PER_HR},
};

bool IsBatteryDead(Aircraft* plane) {
    return ((plane->m_airTimeTicks % G_AircraftCompanyCommon[plane->m_id].flightDurationTicks) == 0);
}

bool IsChargingComplete(Aircraft* plane) {
    return ((plane->m_chargeTimeTicks % G_AircraftCompanyCommon[plane->m_id].chargeDurationTicks) == 0);
}

uint32_t GetCruiseSpeed(aircraft_id_t id) {
    return G_AircraftCompanyCommon[id].cruiseSpeedMph;
}

uint32_t GetPassengerCount(aircraft_id_t id) {
    return G_AircraftCompanyCommon[id].passengerCount;
}

float GetFaultProbabilityPerHour(aircraft_id_t id) {
    return G_AircraftCompanyCommon[id].faultProbabilityPerHour;
}

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

void WriteDataToOutputFile(Aircraft* aircrafts) {
    /*
     Keep track of the following statistics per vehicle type: 
     - average flight time per flight
     - average distance traveled per flight
     - average time charging per charge session
     - total number of faults
     - total number of passenger miles.
    */
    // first, sum up the data
    float dataSums[TOTAL_AIRCRAFT_COMPANIES][5];
    for (uint8_t i = 0; i < TOTAL_AIRCRAFTS_IN_SIMULATION; i++) {
        // in this loop, i increments through the 20 aircraft in the simulation

        // add up flight times, num flights, time charging, num charge sessions, and faults
        aircraft_id_t curId = aircrafts[i].m_id;
        // [id][0] holds total flight time in minutes per vehicle type
        dataSums[curId][0] += (aircrafts[i].m_airTimeTicks / (float) LOOP_TICKS_PER_MIN);
        // [id][1] holds total number of flights per vehicle type
        dataSums[curId][1] += aircrafts[i].m_numFlights;
        // [id][2] holds total charge time in minutes per vehicle type
        dataSums[curId][2] += (aircrafts[i].m_chargeTimeTicks / (float) LOOP_TICKS_PER_MIN);
        // [id][3] holds total number of charge sessions per vehicle type
        dataSums[curId][3] += aircrafts[i].m_numChargeSessions;
        // [id][4] holds total number of faults per vehicle type
        dataSums[curId][4] += aircrafts[i].m_numFaults;
    }
    // now compute the averages/totals
    float averagesAndTotals[TOTAL_AIRCRAFT_COMPANIES][5];
    for (uint8_t i = 0; i < TOTAL_AIRCRAFT_COMPANIES; i++) {
        // in this loop, i increments through the 5 aircraft companies

        uint32_t curVehicleTypeCruiseSpeed = GetCruiseSpeed((aircraft_id_t) i);

        averagesAndTotals[i][0] = dataSums[i][0] / dataSums[i][1]; // average flight time in minutes per flight per vehicle type
        // speed in miles/hour * (1hour / 60min) * avgFlightTimeMin = miles
        averagesAndTotals[i][1] = (curVehicleTypeCruiseSpeed * averagesAndTotals[i][0]) / 60.0f; // average distance traveled per flight
        averagesAndTotals[i][2] = dataSums[i][2] / dataSums[i][3]; // average charge time in minutes per charge session per vehicle type
        averagesAndTotals[i][3] = dataSums[i][4]; // total number of faults per vehicle type. redundant, but here for consistency
        // speed in (miles/hour * (1hour / 60min) * totalFlightTimeMin * num passengers) = passenger miles
        averagesAndTotals[i][4] = ((curVehicleTypeCruiseSpeed * dataSums[i][0]) / 60.0f) * GetPassengerCount((aircraft_id_t) i); // total number of passenger miles per vehicle type
    }
}

void RunSimulation(void) {
    /** Create the array of 20 aircraft with random IDs. This runs a test to ensure
     * at least 1 of each aircraft company was generated. If that test fails, the
     * simulation exits immediately, and must be rerun.
     * 
     * So far, after many trials, we have always generated an ID for each company. */
    Aircraft aircrafts[TOTAL_AIRCRAFTS_IN_SIMULATION];
    CreateRandomAircraftIds(aircrafts);

    ChargeStation battChargers;
    std::queue<uint8_t> aircraftInLineToCharge;

    uint32_t tickCount = 0;
    /** Loop 1 extra time to initialize all aircraft into the flying state.
     * Then all recorded ticks will either be from flying, charging, or waiting
     * to charge. */
    while (tickCount <= SIMULATION_TICKS_3HR) {
        float timeMin = tickCount / (float) LOOP_TICKS_PER_MIN; // for tracing state transitions

        for (uint8_t i = 0; i < TOTAL_AIRCRAFTS_IN_SIMULATION; i++) {
            Aircraft* pCurCraft = &aircrafts[i];
            switch (pCurCraft->m_state) {
                case AIRCRAFT_STATE_IDLE:
                    pCurCraft->m_state = AIRCRAFT_STATE_FLYING;
                    pCurCraft->m_numFlights++;
                    break;
                case AIRCRAFT_STATE_FLYING:
                    pCurCraft->m_airTimeTicks++;
                    if (IsBatteryDead(pCurCraft)) {
                        // place every aircraft in line while waiting for the loop cycle to complete
                        aircraftInLineToCharge.push(i);
                        pCurCraft->m_state = AIRCRAFT_STATE_WAITING_TO_CHARGE;
                    }
                    break;
                case AIRCRAFT_STATE_CHARGING:
                    pCurCraft->m_chargeTimeTicks++;
                    if (IsChargingComplete(pCurCraft)) {
                        battChargers.removeAircraft();
                        TraceFinishCharging(i, timeMin, battChargers.getNumChargersInUse());
                        pCurCraft->m_state = AIRCRAFT_STATE_FLYING;
                        pCurCraft->m_numFlights++;
                    }
                    break;
                case AIRCRAFT_STATE_WAITING_TO_CHARGE:
                    /** Nothing happens when waiting to charge. Once the loop cycle completes,
                     * planes in line are added to chargers that may have become available. */
                    break;
                default:
                    std::cout << "ERROR - default case should never execute." << std::endl;
                    break;
            }

            if ((tickCount > 0) && ((tickCount % SIMULATION_TICKS_1HR) == 0)) {
                // Use hourly percent chance to potentially generate a fault
                HourlyFaultGenerationPercentChance(pCurCraft);
            }
        }

        /** Now that the loop cycle has completed, add aircraft to chargers if any are available.
         * This implementation prevents timing issues from occurring.
         * The original method was:
         * In the charging state, if an aircraft finishes charging, add the next one in line to
         * the free charger. This caused timing problems because if the next plane in line had
         * a larger index than the plane that just finished charging, the loop would reach the
         * next plane during the same cycle, and increment it's charge ticks. We'd see in the
         * trace that an aircraft finished charging at 63.98 minutes for example, when the
         * expected time to finish charging was 64 minutes.
         */
        while ((battChargers.getNumChargersInUse() < TOTAL_CHARGERS) && (aircraftInLineToCharge.size() > 0)) {
            uint8_t aircraftIndex = aircraftInLineToCharge.front();
            if (battChargers.addAircraft()) {
                TraceStartCharging(aircraftIndex, timeMin, battChargers.getNumChargersInUse());
                aircrafts[aircraftIndex].m_state = AIRCRAFT_STATE_CHARGING;
                aircrafts[aircraftIndex].m_numChargeSessions++;
                // pop the queue item now that we know an aircraft was successfully added to the charge station
                aircraftInLineToCharge.pop();
            } else {
                std::cout << "ERROR - adding aircraft to charger failed even though a charger should be available." << std::endl;
            }
        }

        tickCount++;
    }
    
    // Simulation has finished. Write data to output file.
    WriteDataToOutputFile(aircrafts);
}

int main()
{
    // FaultTest(); // call this function to test the fault generation code
    RunSimulation();
}