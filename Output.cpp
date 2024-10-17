#include "Output.h"

#include "AircraftCompanyCommon.h"

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