#pragma once
#include "stdint.h"
#include <string>

class IHubController
{
public:
    virtual void onHubSlotActiveChange(int columnIndex, int slotIndex, bool active) {};
    virtual void onHubSlotNameChange(int columnIndex, int slotIndex, std::string name) {};
    virtual void onHubColumnDimChange(int columnIndex, uint8_t dim) {};
    virtual void onHubColumnNameChange(int columnIndex, std::string name) {};
    virtual void onHubMasterDimChange(uint8_t dim) {};

    virtual void onHubVelocityChange(int paramsSlotIndex, float velocity) {};
    virtual void onHubAmountChange(int paramsSlotIndex, float amount) {};
    virtual void onHubIntensityChange(int paramsSlotIndex, float intensity) {};
    virtual void onHubVariantChange(int paramsSlotIndex, float variant) {};
    virtual void onHubSizeChange(int paramsSlotIndex, float size) {};
    virtual void onHubOffsetChange(int paramsSlotIndex, float offset) {};
    virtual void onHubParamsNameChange(int paramsSlotIndex, std::string name) {};
};