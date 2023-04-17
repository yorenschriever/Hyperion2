#pragma once

#include "IHubController.hpp"
#include "log.hpp"
#include "stdint.h"
#include <algorithm>
#include <vector>

class ControlHub
{

public:
    struct Slot
    {
        bool activated = false;
        bool flash = false;
        bool releaseColumn = true;
        std::string name = "";
    };

    struct Column
    {
        std::vector<Slot> slots;
        uint8_t dim = 255;
        std::string name = "";
    };

    uint8_t masterDim = 255;
    Params params;

private:
    std::vector<Column> columns;
    std::vector<IHubController *> controllers;
    const char *TAG = "CONTROL_HUB";

public:
    void buttonPressed(int columnIndex, int slotIndex)
    {
        // Log::info(TAG, "button pressed %d %d", columnIndex, slotIndex);

        auto slot = findSlot(columnIndex, slotIndex);
        if (slot == NULL)
            return;

        bool newValue;
        if (slot->flash)
            newValue = true;
        else
            newValue = !slot->activated;

        // Log::info(TAG, "newvalue, slotactivated %d, %d", newValue, slot->activated);

        if (newValue != slot->activated)
        {
            if (slot->releaseColumn)
            {
                int slot_off_index = 0;
                // for(auto slot_off : findColumn(columnIndex)->slots.data()){
                auto column = findColumn(columnIndex);
                for (auto slot_it = column->slots.begin(); slot_it < column->slots.end(); ++slot_it)
                {
                    if (slot_it->activated)
                    {
                        slot_it->activated = false;
                        int slotIndex = std::distance(column->slots.begin(), slot_it);
                        for (auto controller : controllers)
                            controller->onHubSlotActiveChange(columnIndex, slotIndex, false);
                    }
                }
            }

            slot->activated = newValue;

            // Log::info(TAG, "onHubSlotActiveChange %d", controllers.size());

            for (auto controller : controllers)
                controller->onHubSlotActiveChange(columnIndex, slotIndex, newValue);
        }
    }

    void buttonReleased(int columnIndex, int slotIndex)
    {
        auto slot = findSlot(columnIndex, slotIndex);
        if (slot == NULL)
            return;

        if (slot->flash && slot->activated)
        {
            slot->activated = false;

            for (auto controller : controllers)
                controller->onHubSlotActiveChange(columnIndex, slotIndex, false);
        }
    }

    void dim(int columnIndex, uint8_t value)
    {
        if (columnIndex < 0 || columnIndex > columns.size() - 1)
            return;
        auto column = columns[columnIndex];

        if (column.dim == value)
            return;

        columns[columnIndex].dim = value;

        for (auto controller : controllers)
            controller->onHubColumnDimChange(columnIndex, value);
    }

    void setMasterDim(uint8_t value)
    {
        if (masterDim == value)
            return;

        masterDim = value;

        for (auto controller : controllers)
            controller->onHubMasterDimChange(value);
    }

    void setVelocity(float velocity)
    {
        // Log::info(TAG, "velocity %f", velocity);
        if (params.velocity == velocity)
            return;

        params.velocity = velocity;

        for (auto controller : controllers)
            controller->onHubVelocityChange(velocity);
    }
    void setAmount(float amount)
    {
        if (params.amount == amount)
            return;

        params.amount = amount;

        for (auto controller : controllers)
            controller->onHubAmountChange(amount);
    }
    void setIntensity(float intensity)
    {
        if (params.intensity == intensity)
            return;

        params.intensity = intensity;

        for (auto controller : controllers)
            controller->onHubIntensityChange(intensity);
    }
    void setVariant(float variant)
    {
        if (params.variant == variant)
            return;

        params.variant = variant;

        for (auto controller : controllers)
            controller->onHubVariantChange(variant);
    }
    void setSize(float size)
    {
        if (params.size == size)
            return;

        params.size = size;

        for (auto controller : controllers)
            controller->onHubSizeChange(size);
    }
    void setOffset(float offset)
    {
        if (params.offset == offset)
            return;

        params.offset = offset;

        for (auto controller : controllers)
            controller->onHubOffsetChange(offset);
    }

    void subscribe(IHubController *controller, bool sendCurrentStatus = false)
    {
        controllers.push_back(controller);
        if (sendCurrentStatus)
            this->sendCurrentStatus(controller);
    }

    void unsubscribe(IHubController *controller)
    {
        controllers.erase(std::remove(controllers.begin(), controllers.end(), controller), controllers.end());
    }

    void sendCurrentStatus(IHubController *controller)
    {
        // Log::info(TAG, "sending current status");
        for (int columnIndex = 0; columnIndex < columns.size(); columnIndex++)
        {
            auto column = columns[columnIndex];
            for (int slotIndex = 0; slotIndex < column.slots.size(); slotIndex++)
            {
                auto slot = column.slots[slotIndex];
                controller->onHubSlotActiveChange(columnIndex, slotIndex, slot.activated);
                controller->onHubSlotNameChange(columnIndex, slotIndex, slot.name);
                // Log::info(TAG, "sending current status2");
            }
            controller->onHubColumnDimChange(columnIndex, column.dim);
            controller->onHubColumnNameChange(columnIndex, column.name);
        }
        controller->onHubMasterDimChange(masterDim);

        controller->onHubAmountChange(params.amount);
        controller->onHubIntensityChange(params.intensity);
        controller->onHubOffsetChange(params.offset);
        controller->onHubSizeChange(params.size);
        controller->onHubVariantChange(params.variant);
        controller->onHubVelocityChange(params.velocity);
    }

    void expandTo(unsigned int minColumns, unsigned int minRows, bool expandAllColumns = false)
    {
        for (int i = columns.size() ; i <= minColumns; i++)
        {
            columns.push_back(Column());
            // Log::info(TAG,"added column. column size = %d", columns.size());
        }

        if (!expandAllColumns)
        {
            for (int j = columns[minColumns].slots.size(); j <= minRows; j++)
            {
                columns[minColumns].slots.push_back(Slot());
                // Log::info(TAG,"added slot. columns.lots size = %d", columns[i].slots.size());
            }
        }
        else
        {
            for (int i = 0; i < columns.size(); i++)
            {
                for (int j = columns[i].slots.size(); j <= minRows; j++)
                {
                    columns[i].slots.push_back(Slot());
                    // Log::info(TAG,"added slot. columns.lots size = %d", columns[i].slots.size());
                }
            }
        }

        // for (int i = 0; i < columns.size(); i++)
        //    Log::info(TAG,"column %d has %d slots", i, columns[i].slots.size());
    }

    void setFlash(int columnIndex, int slotIndex, bool flash=true)
    {
        auto slot = findSlot(columnIndex, slotIndex);
        if (!slot){
            Log::error(TAG,"Cannot set slot to flash mode");
            return;
        }
        slot->flash = flash;
    }

    void setFlashColumn(int columnIndex, bool flash=true)
    {
        auto column = findColumn(columnIndex);
        for(auto& slot: column->slots)
            slot.flash = flash;
    }

    void setFlashRow(int rowIndex, bool flash=true)
    {
        for(auto& column: columns)
        {
            if (column.slots.size() > rowIndex)
                column.slots[rowIndex].flash = flash;
        }
    }

    Slot *findSlot(int columnIndex, int slotIndex) &
    {
        if (columnIndex < 0 || columnIndex > columns.size() - 1)
            return nullptr;
        auto column = &columns.data()[columnIndex];

        if (slotIndex < 0 || slotIndex > column->slots.size() - 1)
            return nullptr;

        // Log::info(TAG, "find slot %d %d", columns.size(), column->slots.size());

        return &column->slots.data()[slotIndex];
    }

    Column *findColumn(int columnIndex) &
    {
        if (columnIndex < 0 || columnIndex > columns.size() - 1)
            return nullptr;
        return &columns.data()[columnIndex];
    }

    Slot *findSlot(Column *column, int slotIndex) &
    {
        if (slotIndex < 0 || slotIndex > column->slots.size() - 1)
            return nullptr;

        return &column->slots.data()[slotIndex];
    }
};
