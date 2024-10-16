/** Defines the charge station class. Only 3 chargers are availbe for use.
 * Note that only one instance of this class will be used in the application. */

#pragma once

#include <cstdint>

#define TOTAL_CHARGERS (3u)

class ChargeStation
{
private:
  uint8_t m_numChargersInUse;
public:
  ChargeStation() {m_numChargersInUse = 0;}
  uint8_t getNumChargersInUse(void) const {return m_numChargersInUse;}
  bool isChargerAvailable(void) const {return (m_numChargersInUse < TOTAL_CHARGERS);}
  bool addAircraft(void) {
    if (isChargerAvailable()) {
      m_numChargersInUse++;
      return true;
    }
    return false;
  }
  bool removeAircraft(void) {
    if (m_numChargersInUse > 0) {
      m_numChargersInUse--;
      return true;
    }
    return false;
  }
};

