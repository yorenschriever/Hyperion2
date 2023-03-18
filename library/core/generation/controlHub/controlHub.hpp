
#include "IHubController.hpp"
#include "log.hpp"
#include "stdint.h"
#include <vector>
#include <algorithm> 

class ControlHub
{

public:
    struct Slot
    {
        bool activated = false;
        bool flash = false;
        char name[20] = "";
    };

    struct Column
    {
        std::vector<Slot> slots;
        uint8_t dim = 255;
        char name[20] = "";
    };

private:
    std::vector<Column> columns;
    uint8_t masterDim_;

    std::vector<IHubController *> controllers;

public:
    void buttonPressed(int columnIndex, int slotIndex)
    {
        //Log::info("CONTROLHUB", "button pressed %d %d", columnIndex, slotIndex);

        auto slot = findSlot(columnIndex, slotIndex);
        if (slot == NULL)
            return;

        bool newValue;
        if (slot->flash)
            newValue = true;
        else
            newValue = !slot->activated;

        //Log::info("CONTROLHUB", "newvalue, slotactivated %d, %d", newValue, slot->activated);

        if (newValue != slot->activated)
        {
            slot->activated = newValue;

            //Log::info("CONTROLHUB", "onHubSlotActiveChange %d", controllers.size());

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

        column.dim = value;

        for (auto controller : controllers)
            controller->onHubColumnDimChange(columnIndex, value);
    }

    void masterDim(uint8_t value)
    {
        if (masterDim_ == value)
            return;

        masterDim_ = value;

        for (auto controller : controllers)
            controller->onHubMasterDimChange(value);
    }

    void subscribe(IHubController *controller)
    {
        controllers.push_back(controller);
    }

    void unsubscribe(IHubController *controller)
    {
        controllers.erase(std::remove(controllers.begin(), controllers.end(), controller), controllers.end());
    }

    static ControlHub fromGrid(unsigned int columns, unsigned int rows)
    {
        auto result = ControlHub();
        result.columns = std::vector<Column>(columns);
        for (int i = 0; i < columns; i++)
        {
            result.columns.push_back(Column());
            for (int j = 0; j < rows; j++)
            {
                result.columns[i].slots.push_back(Slot());
            }
        }
        return result;
    }

    Slot *findSlot(int columnIndex, int slotIndex) &
    {
        if (columnIndex < 0 || columnIndex > columns.size() - 1)
            return nullptr;
        auto column = &columns.data()[columnIndex];

        if (slotIndex < 0 || slotIndex > column->slots.size() - 1)
            return nullptr;

        //Log::info("CONTROLHUB", "find slot %d %d", columns.size(), column->slots.size());

        return &column->slots.data()[slotIndex];
    }
};
