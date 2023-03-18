#pragma once
#include "stdint.h"

class IHubController
{
public:
    virtual void onHubSlotActiveChange(int columnIndex, int slotIndex, bool active)=0;
    virtual void onHubColumnDimChange(int columnIndex, uint8_t dim)=0;
    virtual void onHubMasterDimChange(uint8_t dim)=0;
};