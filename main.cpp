#include <iostream>

#include "AircraftCompanyCommon.h"
#include "Aircraft.h"

#define TOTAL_AIRCRAFT_COMPANIES (5u)

/** Define data common to aircraft of the same company.
 * Use the aircraft id to index through this array. */
aircraft_company_common_t G_AircraftCommon[TOTAL_AIRCRAFT_COMPANIES] = {
    {ALPHA_FLIGHT_DUR_TICKS, ALPHA_CHARGE_DUR_TICKS, ALPHA_PASSENGER_COUNT, ALPHA_FAULT_PROBABILITY},
};

#define SIMULATION_TICKS_1HR (60u * LOOP_TICKS_PER_MIN) // 60min

int main()
{
    Aircraft a1(AIRCRAFT_ID_ALPHA);

    uint32_t tickCount = 0;
    /** Loop 1 extra time to initialize all aircraft into the flying state.
     * Then all recorded ticks will either be from flying, charging, or waiting
     * to charge. */
    while (tickCount++ <= SIMULATION_TICKS_1HR) {
        switch (a1.m_state) {
            case AIRCRAFT_STATE_IDLE:
                a1.m_state = AIRCRAFT_STATE_FLYING;
                break;
            case AIRCRAFT_STATE_FLYING:
                a1.m_airTimeTicks++;
                if ((a1.m_airTimeTicks % G_AircraftCommon[a1.m_id].flightDurationTicks) == 0) {
                    // Battery is dead. Time to recharge. Is there a charger available?
                    a1.m_state = AIRCRAFT_STATE_WAITING_TO_CHARGE;
                }
                break;
            case AIRCRAFT_STATE_WAITING_TO_CHARGE:

                break;
            case AIRCRAFT_STATE_CHARGING:

                break;
            default:
                std::cout << "ERROR - default case should never execute." << std::endl;
                break;
        }
    }

    std::cout << "Flight time " << (float) (a1.m_airTimeTicks / (float) LOOP_TICKS_PER_MIN) <<
        " state " << a1.m_state << std::endl;
}
