/** E-Vtol simulation of 20 planes flying and recharging using only
 * 3 available chargers. */

#include <iostream>
#include <queue>

#include "AircraftCompanyCommon.h"
#include "Aircraft.h"
#include "ChargeStation.h"

/** Use these definitions to test the simulation for different
 * amounts of time. Verify recorded data matches expectations
 * for 1, 2, and 3 hours of simulation time. */
#define SIMULATION_TICKS_1HR (60u * LOOP_TICKS_PER_MIN) // 60min
#define SIMULATION_TICKS_2HR (120u * LOOP_TICKS_PER_MIN)
#define SIMULATION_TICKS_3HR (180u * LOOP_TICKS_PER_MIN)

#define TOTAL_AIRCRAFT_COMPANIES            (5u)
#define TOTAL_AIRCRAFTS_IN_SIMULATION       (6u)

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

void RunSimulation(void) {
    ChargeStation battChargers;
    std::queue<uint8_t> aircraftInLineToCharge;

    Aircraft aircrafts[TOTAL_AIRCRAFTS_IN_SIMULATION] = {
        Aircraft(AIRCRAFT_ID_ALPHA),
        Aircraft(AIRCRAFT_ID_BRAVO),
        Aircraft(AIRCRAFT_ID_CHARLIE),
        Aircraft(AIRCRAFT_ID_DELTA),
        Aircraft(AIRCRAFT_ID_ECHO),
        Aircraft(AIRCRAFT_ID_CHARLIE),
    };

    uint32_t tickCount = 0;
    /** Loop 1 extra time to initialize all aircraft into the flying state.
     * Then all recorded ticks will either be from flying, charging, or waiting
     * to charge. */
    while (tickCount++ <= SIMULATION_TICKS_3HR) {
        for (uint8_t i = 0; i < TOTAL_AIRCRAFTS_IN_SIMULATION; i++) {
            Aircraft* pCurCraft = &aircrafts[i];
            float timeMin = (tickCount - 1) / (float) LOOP_TICKS_PER_MIN;
            switch (pCurCraft->m_state) {
                case AIRCRAFT_STATE_IDLE:
                    pCurCraft->m_state = AIRCRAFT_STATE_FLYING;
                    break;
                case AIRCRAFT_STATE_FLYING:
                    pCurCraft->m_airTimeTicks++;
                    if (IsBatteryDead(pCurCraft)) {
                        if (battChargers.addAircraft()) {
                            // std::cout << "aircraft charging id: " << +i << " time: " << timeMin << std::endl;
                            pCurCraft->m_state = AIRCRAFT_STATE_CHARGING;
                        } else {
                            aircraftInLineToCharge.push(i);
                            // std::cout << "aircraft placed in line id: " << +i << " time: " << timeMin << std::endl;
                            pCurCraft->m_state = AIRCRAFT_STATE_WAITING_TO_CHARGE;
                        }
                    }
                    break;
                case AIRCRAFT_STATE_CHARGING:
                    pCurCraft->m_chargeTimeTicks++;
                    if (IsChargingComplete(pCurCraft)) {
                        // std::cout << "aircraft completed charging id: " << +i << " time: " << timeMin << std::endl;
                        battChargers.removeAircraft();
                        pCurCraft->m_state = AIRCRAFT_STATE_FLYING;

                        if (aircraftInLineToCharge.size() > 0) {
                            uint8_t aircraftIndex = aircraftInLineToCharge.front();
                            if (battChargers.addAircraft()) {
                                // std::cout << "aircraft charging id: " << +aircraftIndex << " time: " << timeMin << std::endl;
                                aircrafts[aircraftIndex].m_state = AIRCRAFT_STATE_CHARGING;
                                // pop the queue item now that we know an aircraft was successfully added to the charge station
                                aircraftInLineToCharge.pop();
                            } else {
                                std::cout << "ERROR - adding aircraft to charger failed even though a charger should be available." << std::endl;
                            }
                        }
                    }
                    break;
                case AIRCRAFT_STATE_WAITING_TO_CHARGE:
                    /** Nothing happens when waiting to charge. In the CHARGING state, once an
                     * aircraft finishes charging, the next one in line takes its place. */
                    break;
                default:
                    std::cout << "ERROR - default case should never execute." << std::endl;
                    break;
            }
        }
    }
    std::cout << "Sim end. chargers in use: " << +battChargers.getNumChargersInUse() << std::endl;
}

int main()
{
    RunSimulation();
}