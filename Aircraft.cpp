#include "Aircraft.h"
#include "AircraftCompanyCommon.h"

Aircraft::Aircraft() {
  m_id = AIRCRAFT_ID_ALPHA;
  m_state = AIRCRAFT_STATE_IDLE;
  m_airTimeTicks = 0;
  m_numFlights = 0;
  m_chargeTimeTicks = 0;
  m_numChargeSessions = 0;
  m_numFaults = 0;
}

Aircraft::Aircraft(aircraft_id_t id)
{
  m_id = id;
  m_state = AIRCRAFT_STATE_IDLE;
  m_airTimeTicks = 0;
  m_numFlights = 0;
  m_chargeTimeTicks = 0;
  m_numChargeSessions = 0;
  m_numFaults = 0;
}

void Aircraft::print(uint32_t cruiseSpeedMph, uint32_t numPassengers) const {
  float airTimeMin = m_airTimeTicks / (float) LOOP_TICKS_PER_MIN;
  float chargeTimeMin = m_chargeTimeTicks / (float) LOOP_TICKS_PER_MIN;

  // speed in miles/hour * (1hour / 60min) * airTimeMin
  float distanceTraveled = (cruiseSpeedMph * airTimeMin) / 60.0f;
  float passengerMiles = numPassengers * distanceTraveled;

  std::cout <<
    "Aircraft ID: " << m_id <<
    "\nState: " << m_state <<
    "\nFlight time: " << airTimeMin << " min" <<
    "\nCharge time: " << chargeTimeMin << " min" <<
    "\nCharge sessions: " << m_numChargeSessions <<
    "\nNum faults: " << m_numFaults <<
    "\nDistance traveled: " << distanceTraveled << " miles" <<
    "\nPassenger miles: " << passengerMiles <<
    "\n-------------------\n" <<
    std::endl;
};
