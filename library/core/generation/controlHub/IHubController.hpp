#pragma once
#include "stdint.h"

class IHubController
{
public:
    virtual void onHubSlotActiveChange(int columnIndex, int slotIndex, bool active) = 0;
    virtual void onHubColumnDimChange(int columnIndex, uint8_t dim) = 0;
    virtual void onHubMasterDimChange(uint8_t dim) = 0;

    virtual void onHubVelocityChange(float velocity) = 0;
    virtual void onHubAmountChange(float amount) = 0;
    virtual void onHubIntensityChange(float intensity) = 0;
    virtual void onHubVariantChange(float variant) = 0;
    virtual void onHubSizeChange(float size) = 0;
    virtual void onHubOffsetChange(float offset) = 0;
};