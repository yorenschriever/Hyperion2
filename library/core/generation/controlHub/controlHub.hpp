#pragma once

#include "IHubController.hpp"
#include "generation/patterns/helpers/params.h"
#include "log.hpp"
#include "stdint.h"
#include <algorithm>
#include <set>
#include <vector>

class ControlHub
{

public:
    ControlHub()
    {
        paramsSet.push_back(new Params());
    }

    struct Slot
    {
        bool activated = false;
        bool flash = false;
        bool releaseColumn = false;
        std::string name = "";
    };

    struct Column
    {
        std::vector<Slot> slots;
        uint8_t dim = 255;
        std::string name = "";
        bool forcedSelection = false;
    };

    uint8_t masterDim = 255;

private:
    std::vector<Column> columns;
    std::vector<IHubController *> controllers;
    std::vector<Params *> paramsSet;
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

        if (newValue == slot->activated)
            return;

        auto col = findColumn(columnIndex);
        if (newValue == false && col->forcedSelection)
        {
            int numberOn = 0;
            for (auto &slot : col->slots)
                numberOn += slot.activated ? 1 : 0;
            if (numberOn == 1)
                return;
        }

        if (slot->releaseColumn)
        {
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

        for (auto &controller : controllers)
            controller->onHubSlotActiveChange(columnIndex, slotIndex, newValue);
    }

    void buttonReleased(int columnIndex, int slotIndex)
    {
        auto slot = findSlot(columnIndex, slotIndex);
        if (slot == NULL)
            return;

        if (slot->flash && slot->activated)
        {
            slot->activated = false;

            for (auto &controller : controllers)
                controller->onHubSlotActiveChange(columnIndex, slotIndex, false);
        }
    }

    /*
     * Be aware that this method does not take into account column/button behaviour 
     * like flash mode, column release, forced selection etc. 
     * Use with caution.
     */
    void setSlotActive(int columnIndex, int slotIndex, bool active)
    {
        auto slot = findSlot(columnIndex, slotIndex);
        if (slot == NULL)
            return;

        slot->activated = active;

        for (auto &controller : controllers)
            controller->onHubSlotActiveChange(columnIndex, slotIndex, active);
    }

    void dim(int columnIndex, uint8_t value)
    {
        if (columnIndex < 0 || columnIndex > columns.size() - 1)
            return;
        auto &column = columns[columnIndex];

        if (column.dim == value)
            return;

        columns[columnIndex].dim = value;

        for (auto &controller : controllers)
            controller->onHubColumnDimChange(columnIndex, value);
    }

    void setMasterDim(uint8_t value)
    {
        if (masterDim == value)
            return;

        masterDim = value;

        for (auto &controller : controllers)
            controller->onHubMasterDimChange(value);
    }

    void setVelocity(int paramsSlotIndex, float velocity)
    {
        if (paramsSlotIndex < 0 || paramsSlotIndex >= paramsSet.size())
            return;
        if (paramsSet[paramsSlotIndex]->velocity == velocity)
            return;

        paramsSet[paramsSlotIndex]->velocity = velocity;

        for (auto &controller : controllers)
            controller->onHubVelocityChange(paramsSlotIndex, velocity);
    }
    void setAmount(int paramsSlotIndex, float amount)
    {
        if (paramsSlotIndex < 0 || paramsSlotIndex >= paramsSet.size())
            return;
        if (paramsSet[paramsSlotIndex]->amount == amount)
            return;

        paramsSet[paramsSlotIndex]->amount = amount;

        for (auto &controller : controllers)
            controller->onHubAmountChange(paramsSlotIndex, amount);
    }
    void setIntensity(int paramsSlotIndex, float intensity)
    {
        if (paramsSlotIndex < 0 || paramsSlotIndex >= paramsSet.size())
            return;
        if (paramsSet[paramsSlotIndex]->intensity == intensity)
            return;

        paramsSet[paramsSlotIndex]->intensity = intensity;

        for (auto &controller : controllers)
            controller->onHubIntensityChange(paramsSlotIndex, intensity);
    }

    void setVariant(int paramsSlotIndex, float variant)
    {
        if (paramsSlotIndex < 0 || paramsSlotIndex >= paramsSet.size())
            return;
        if (paramsSet[paramsSlotIndex]->variant == variant)
            return;

        paramsSet[paramsSlotIndex]->variant = variant;

        for (auto &controller : controllers)
            controller->onHubVariantChange(paramsSlotIndex, variant);
    }
    void setSize(int paramsSlotIndex, float size)
    {
        if (paramsSlotIndex < 0 || paramsSlotIndex >= paramsSet.size())
            return;
        if (paramsSet[paramsSlotIndex]->size == size)
            return;

        paramsSet[paramsSlotIndex]->size = size;

        for (auto &controller : controllers)
            controller->onHubSizeChange(paramsSlotIndex, size);
    }
    void setOffset(int paramsSlotIndex, float offset)
    {
        if (paramsSlotIndex < 0 || paramsSlotIndex >= paramsSet.size())
            return;
        if (paramsSet[paramsSlotIndex]->offset == offset)
            return;

        paramsSet[paramsSlotIndex]->offset = offset;

        for (auto &controller : controllers)
            controller->onHubOffsetChange(paramsSlotIndex, offset);
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

        for (int paramsSlotIndex = 0; paramsSlotIndex < paramsSet.size(); paramsSlotIndex++)
        {
            auto params = paramsSet[paramsSlotIndex];
            controller->onHubVelocityChange(paramsSlotIndex, params->velocity);
            controller->onHubAmountChange(paramsSlotIndex, params->amount);
            controller->onHubSizeChange(paramsSlotIndex, params->size);
            controller->onHubOffsetChange(paramsSlotIndex, params->offset);
            controller->onHubVariantChange(paramsSlotIndex, params->variant);
            controller->onHubIntensityChange(paramsSlotIndex, params->intensity);
            controller->onHubParamsNameChange(paramsSlotIndex, params->name);
        }
    }

    void expandTo(unsigned int minColumns, unsigned int minRows, bool expandAllColumns = false)
    {
        for (int i = columns.size(); i <= minColumns; i++)
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

    void setFlash(int columnIndex, int slotIndex, bool flash = true, bool releaseColumn = false)
    {
        auto slot = findSlot(columnIndex, slotIndex);
        if (!slot)
        {
            Log::error(TAG, "Cannot set slot to flash mode");
            return;
        }
        slot->flash = flash;
        slot->releaseColumn = releaseColumn;
    }

    void setFlashColumn(int columnIndex, bool flash = true, bool releaseColumn = false)
    {
        auto column = findColumn(columnIndex);
        if (!column)
        {
            Log::error(TAG, "Cannot set column to flash mode");
            return;
        }
        for (auto &slot : column->slots)
        {
            slot.flash = flash;
            slot.releaseColumn = releaseColumn;
        }
    }

    void setForcedSelection(int columnIndex, bool force = true)
    {
        auto column = findColumn(columnIndex);
        if (!column)
        {
            Log::error(TAG, "Cannot set column forced selection");
            return;
        }
        column->forcedSelection = force;
    }

    void setFlashRow(int rowIndex, bool flash = true, bool releaseColumn = false)
    {
        for (auto &column : columns)
        {
            if (column.slots.size() > rowIndex)
            {
                column.slots[rowIndex].flash = flash;
                column.slots[rowIndex].releaseColumn = releaseColumn;
            }
        }
    }

    Slot *findSlot(int columnIndex, int slotIndex) &
    {
        if (columnIndex < 0 || columnIndex > ((int)columns.size()) - 1)
            return nullptr;
        auto column = &columns.data()[columnIndex];

        if (slotIndex < 0 || slotIndex > ((int)column->slots.size()) - 1)
            return nullptr;

        // Log::info(TAG, "find slot %d %d", columns.size(), column->slots.size());

        return &column->slots.data()[slotIndex];
    }

    int getColumnCount()
    {
        return columns.size();
    }

    int getSlotCount(int columnIndex)
    {
        auto col = findColumn(columnIndex);
        if (!col)
            return 0;
        return col->slots.size();
    }

    Column *findColumn(int columnIndex) &
    {
        if (columnIndex < 0 || columnIndex > ((int)columns.size()) - 1)
            return nullptr;
        return &columns.data()[columnIndex];
    }

    Slot *findSlot(Column *column, int slotIndex) &
    {
        if (slotIndex < 0 || slotIndex > ((int)column->slots.size()) - 1)
            return nullptr;

        return &column->slots.data()[slotIndex];
    }

    void setColumnName(int columnIndex, const char *name)
    {
        auto col = findColumn(columnIndex);
        if (!col)
        {
            Log::error(TAG, "Cannot set name of column %d", columnIndex);
            return;
        }
        col->name = name;
    }

    // params
    // currently you can only add params. first of all, because you probably never have to remove them,
    // and second: it will become confusing that the slots will shift if one item is removed.
    // the mappings to the slots will stay the same,

    int addParams(Params *params)
    {
        paramsSet.push_back(params);
        return paramsSet.size() - 1;
    }

    Params *getParams(int paramsSlotIndex)
    {
        if (paramsSlotIndex < 0 || paramsSlotIndex > paramsSet.size() - 1)
        {
            Log::error(TAG, "Cannot access params in slot %d, num slots = %d", paramsSlotIndex, paramsSet.size());
            return nullptr;
        }

        return paramsSet[paramsSlotIndex];
    }

};
