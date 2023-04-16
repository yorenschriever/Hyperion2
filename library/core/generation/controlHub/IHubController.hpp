#pragma once
#include "stdint.h"

class IHubController
{
public:
    virtual void onHubSlotActiveChange(int columnIndex, int slotIndex, bool active) {};
    virtual void onHubSlotNameChange(int columnIndex, int slotIndex, std::string name) {};
    virtual void onHubColumnDimChange(int columnIndex, uint8_t dim) {};
    virtual void onHubColumnNameChange(int columnIndex, std::string name) {};
    virtual void onHubMasterDimChange(uint8_t dim) {};

    virtual void onHubVelocityChange(float velocity) {};
    virtual void onHubAmountChange(float amount) {};
    virtual void onHubIntensityChange(float intensity) {};
    virtual void onHubVariantChange(float variant) {};
    virtual void onHubSizeChange(float size) {};
    virtual void onHubOffsetChange(float offset) {};
};