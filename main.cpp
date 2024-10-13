/** E-Vtol simulation of 20 planes flying and recharging using only
 * 3 available chargers. */

#include <iostream>

#include "AircraftCompanyCommon.h"
#include "Aircraft.h"
#include "ChargeStation.h"

#define TOTAL_AIRCRAFT_COMPANIES (5u)

/** Define data common to aircraft of the same company.
 * Use the aircraft id to index through this array. */
aircraft_company_common_t G_AircraftCompanyCommon[TOTAL_AIRCRAFT_COMPANIES] = {
    {ALPHA_CRUISE_SPEED_MPH, ALPHA_FLIGHT_DUR_TICKS, ALPHA_CHARGE_DUR_TICKS, ALPHA_PASSENGER_COUNT, ALPHA_FAULT_PROBABILITY},
};

bool IsBatteryDead(Aircraft* plane) {
    return ((plane->m_airTimeTicks % G_AircraftCompanyCommon[plane->m_id].flightDurationTicks) == 0);
}

bool IsChargingComplete(Aircraft* plane) {
    return ((plane->m_chargeTimeTicks % G_AircraftCompanyCommon[plane->m_id].chargeDurationTicks) == 0);
}

#define SIMULATION_TICKS_1HR (60u * LOOP_TICKS_PER_MIN) // 60min
#define SIMULATION_TICKS_2HR (120u * LOOP_TICKS_PER_MIN)
#define SIMULATION_TICKS_3HR (180u * LOOP_TICKS_PER_MIN)

int main()
{
    ChargeStation cs;
    Aircraft a1(AIRCRAFT_ID_ALPHA);

    uint32_t tickCount = 0;
    /** Loop 1 extra time to initialize all aircraft into the flying state.
     * Then all recorded ticks will either be from flying, charging, or waiting
     * to charge. */
    while (tickCount++ <= SIMULATION_TICKS_3HR) {
        switch (a1.m_state) {
            case AIRCRAFT_STATE_IDLE:
                a1.m_state = AIRCRAFT_STATE_FLYING;
                break;
            case AIRCRAFT_STATE_FLYING:
                a1.m_airTimeTicks++;
                if (IsBatteryDead(&a1)) {
                    if (cs.addAircraft()) {
                        a1.m_state = AIRCRAFT_STATE_CHARGING;
                    } else {
                        // TODO: add this plane to the line
                        a1.m_state = AIRCRAFT_STATE_WAITING_TO_CHARGE;
                    }
                }
                break;
            case AIRCRAFT_STATE_CHARGING:
                a1.m_chargeTimeTicks++;
                if (IsChargingComplete(&a1)) {
                    a1.m_state = AIRCRAFT_STATE_FLYING;
                }
                break;
            case AIRCRAFT_STATE_WAITING_TO_CHARGE:

                break;
            default:
                std::cout << "ERROR - default case should never execute." << std::endl;
                break;
        }
    }

    a1.print(G_AircraftCompanyCommon[a1.m_id].cruiseSpeedMph, G_AircraftCompanyCommon[a1.m_id].passengerCount);
}
