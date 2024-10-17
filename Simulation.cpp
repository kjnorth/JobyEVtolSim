/** E-Vtol simulation of 20 planes flying and recharging using only
 * 3 available chargers. */

#include <cstdlib>
#include <iostream>
#include <queue>

#include "AircraftCompanyCommon.h"
#include "Aircraft.h"
#include "ChargeStation.h"
#include "Output.h"
#include "RandomGen.h"
#include "Trace.h"

/** Use these definitions to test the simulation for different
 * amounts of time. Verify recorded data matches expectations
 * for 1, 2, and 3 hours of simulation time. */
#define SIMULATION_TICKS_1HR (60u * LOOP_TICKS_PER_MIN) // 60min
#define SIMULATION_TICKS_2HR (120u * LOOP_TICKS_PER_MIN)
#define SIMULATION_TICKS_3HR (180u * LOOP_TICKS_PER_MIN)

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
                        if (battChargers.removeAircraft() == false) {
                            std::cout << "ERROR - removing aircraft from charger failed because no chargers are in use." << std::endl;
                        }
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
    aircrafts[8].print(GetCruiseSpeed(aircrafts[8].m_id), GetPassengerCount(aircrafts[8].m_id));
}