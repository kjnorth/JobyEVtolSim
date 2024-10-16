/** E-Vtol simulation of 20 planes flying and recharging using only
 * 3 available chargers. */

#include <cstdlib> 
#include <ctime>
#include <iostream>
#include <queue>

#include "AircraftCompanyCommon.h"
#include "Aircraft.h"
#include "ChargeStation.h"

// #define ENABLE_TRACE // uncomment to enable trace output to the terminal

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
    {ALPHA_CRUISE_SPEED_MPH, ALPHA_FLIGHT_DUR_TICKS, ALPHA_CHARGE_DUR_TICKS, ALPHA_PASSENGER_COUNT, ALPHA_FAULT_PROBABILITY},
    {BRAVO_CRUISE_SPEED_MPH, BRAVO_FLIGHT_DUR_TICKS, BRAVO_CHARGE_DUR_TICKS, BRAVO_PASSENGER_COUNT, BRAVO_FAULT_PROBABILITY},
    {CHARLIE_CRUISE_SPEED_MPH, CHARLIE_FLIGHT_DUR_TICKS, CHARLIE_CHARGE_DUR_TICKS, CHARLIE_PASSENGER_COUNT, CHARLIE_FAULT_PROBABILITY},
    {DELTA_CRUISE_SPEED_MPH, DELTA_FLIGHT_DUR_TICKS, DELTA_CHARGE_DUR_TICKS, DELTA_PASSENGER_COUNT, DELTA_FAULT_PROBABILITY},
    {ECHO_CRUISE_SPEED_MPH, ECHO_FLIGHT_DUR_TICKS, ECHO_CHARGE_DUR_TICKS, ECHO_PASSENGER_COUNT, ECHO_FAULT_PROBABILITY},
};

bool IsBatteryDead(Aircraft* plane) {
    return ((plane->m_airTimeTicks % G_AircraftCompanyCommon[plane->m_id].flightDurationTicks) == 0);
}

bool IsChargingComplete(Aircraft* plane) {
    return ((plane->m_chargeTimeTicks % G_AircraftCompanyCommon[plane->m_id].chargeDurationTicks) == 0);
}

uint32_t GetCruiseSpeed(Aircraft* plane) {
    return G_AircraftCompanyCommon[plane->m_id].cruiseSpeedMph;
}

uint32_t GetPassengerCount(Aircraft* plane) {
    return G_AircraftCompanyCommon[plane->m_id].passengerCount;
}

float GetFaultProbability(Aircraft* plane) {
    return G_AircraftCompanyCommon[plane->m_id].faultProbability;
}

void TraceStartCharging(uint8_t arrayIndex, float timeMin, uint8_t numChargersInUse) {
    #ifdef ENABLE_TRACE
        std::cout <<
            "start charging - array index: " << +arrayIndex <<
            ", time: " << timeMin << " min" <<
            ", chargers in use: " << +numChargersInUse <<
            std::endl;
    #endif
}

void TraceFinishCharging(uint8_t arrayIndex, float timeMin, uint8_t numChargersInUse) {
    #ifdef ENABLE_TRACE
        std::cout <<
            "completed charging - array index: " << +arrayIndex <<
            ", time: " << timeMin << " min" <<
            ", chargers in use: " << +numChargersInUse <<
            std::endl;
    #endif
}

void TraceAddedToLine(uint8_t arrayIndex, float timeMin) {
    #ifdef ENABLE_TRACE
        std::cout <<
            "placed in line - array index: " << +arrayIndex <<
            ", time: " << timeMin << " min" <<
            std::endl;
    #endif
}

void CreateRandomAircraftIds(Aircraft* aircrafts) {
    bool verifyAllIdsGenerated[TOTAL_AIRCRAFT_COMPANIES] = {false};
    for (uint8_t i = 0; i < TOTAL_AIRCRAFTS_IN_SIMULATION; i++) {
        aircraft_id_t randomId = (aircraft_id_t) (rand() % 5);
        aircrafts[i].m_id = randomId;
        verifyAllIdsGenerated[randomId] = true;
    }

    /** Verify at least one of each aircraft company exists in the array. If this
     * test fails, simply exit and rerun simulation. If this fails frequently, we
     * can come up with a method to populate the array with the missind ID(s). */
    for (uint8_t i = 0; i < TOTAL_AIRCRAFT_COMPANIES; i++) {
        if (verifyAllIdsGenerated[i] == false) {
            std::cout << "ERROR - all aircraft IDs were not generated." << std::endl;
            std::cout.flush();
            exit(EXIT_FAILURE);
        }
    }
}

void RunSimulation(void) {
    // Use current time as seed for random generator
    srand(time(0));

    // create the array of 20 aircraft with random IDs
    Aircraft aircrafts[TOTAL_AIRCRAFTS_IN_SIMULATION];
    CreateRandomAircraftIds(aircrafts);

    // Test pased. Continue with the simulation
    ChargeStation battChargers;
    std::queue<uint8_t> aircraftInLineToCharge;

    uint32_t tickCount = 0;
    /** Loop 1 extra time to initialize all aircraft into the flying state.
     * Then all recorded ticks will either be from flying, charging, or waiting
     * to charge. */
    while (tickCount++ <= SIMULATION_TICKS_3HR) {
        // subtract 1 from tickCount to account for initialization when i = 0
        float timeMin = (tickCount - 1) / (float) LOOP_TICKS_PER_MIN;

        for (uint8_t i = 0; i < TOTAL_AIRCRAFTS_IN_SIMULATION; i++) {
            Aircraft* pCurCraft = &aircrafts[i];
            switch (pCurCraft->m_state) {
                case AIRCRAFT_STATE_IDLE:
                    pCurCraft->m_state = AIRCRAFT_STATE_FLYING;
                    break;
                case AIRCRAFT_STATE_FLYING:
                    pCurCraft->m_airTimeTicks++;
                    if (IsBatteryDead(pCurCraft)) {
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
                // pop the queue item now that we know an aircraft was successfully added to the charge station
                aircraftInLineToCharge.pop();
            } else {
                std::cout << "ERROR - adding aircraft to charger failed even though a charger should be available." << std::endl;
            }
        }
    }
    aircrafts[8].print(GetCruiseSpeed(&aircrafts[8]), GetPassengerCount(&aircrafts[8]));
}

int main()
{
    RunSimulation();
}