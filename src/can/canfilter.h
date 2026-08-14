#ifndef CANFILTER_H
#define CANFILTER_H

// C++ standard-library headers
#include <cstdint>

class CANFilter
{
public:
    CANFilter();
    void setFilter(uint32_t id, uint32_t mask, int bus);
    bool checkFilter(uint32_t id, int bus);

public:
    uint32_t ID;
    uint32_t mask;
    int bus;
};

#endif // CANFILTER_H
