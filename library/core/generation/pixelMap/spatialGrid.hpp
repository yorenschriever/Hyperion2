#pragma once

#include "pixelMap.hpp"
#include <vector>
#include <cmath>
#include <algorithm>

class SpatialGrid
{
    static const int GRID_SIZE = 10;
    std::vector<std::vector<int>> cells;
    float cellWidth;
    float cellHeight;
    float minX = -1.0f;
    float minY = -1.0f;
    float maxX = 1.0f;
    float maxY = 1.0f;

    int cellIndex(int cx, int cy) const
    {
        return cy * GRID_SIZE + cx;
    }

    int toCellX(float x) const
    {
        int cx = (int)((x - minX) / cellWidth);
        if (cx < 0) cx = 0;
        if (cx >= GRID_SIZE) cx = GRID_SIZE - 1;
        return cx;
    }

    int toCellY(float y) const
    {
        int cy = (int)((y - minY) / cellHeight);
        if (cy < 0) cy = 0;
        if (cy >= GRID_SIZE) cy = GRID_SIZE - 1;
        return cy;
    }

public:
    void build(PixelMap *map)
    {
        cells.clear();
        cells.resize(GRID_SIZE * GRID_SIZE);
        cellWidth = (maxX - minX) / GRID_SIZE;
        cellHeight = (maxY - minY) / GRID_SIZE;
        for (int i = 0; i < map->size(); i++)
        {
            int cx = toCellX(map->x(i));
            int cy = toCellY(map->y(i));
            cells[cellIndex(cx, cy)].push_back(i);
        }
    }

    void getPixelsInRange(float x, float y, float r, std::vector<int> &result) const
    {
        result.clear();
        int cxCenter = toCellX(x);
        int cyCenter = toCellY(y);

        int cellsNeededX = (int)std::ceil(r / cellWidth) + 1;
        int cellsNeededY = (int)std::ceil(r / cellHeight) + 1;

        int cxMin = std::max(0, cxCenter - cellsNeededX);
        int cxMax = std::min(GRID_SIZE - 1, cxCenter + cellsNeededX);
        int cyMin = std::max(0, cyCenter - cellsNeededY);
        int cyMax = std::min(GRID_SIZE - 1, cyCenter + cellsNeededY);

        for (int cy = cyMin; cy <= cyMax; cy++)
        {
            float cellBottomY = minY + cy * cellHeight;
            float cellTopY = cellBottomY + cellHeight;
            float closestY = (y < cellBottomY) ? cellBottomY : (y > cellTopY ? cellTopY : y);
            float dy = y - closestY;

            for (int cx = cxMin; cx <= cxMax; cx++)
            {
                float cellLeftX = minX + cx * cellWidth;
                float cellRightX = cellLeftX + cellWidth;
                float closestX = (x < cellLeftX) ? cellLeftX : (x > cellRightX ? cellRightX : x);
                float dx = x - closestX;

                if (dx * dx + dy * dy > r * r)
                    continue;

                const auto &cell = cells[cellIndex(cx, cy)];
                for (int idx : cell)
                    result.push_back(idx);
            }
        }
    }
};

class SpatialGrid3D
{
    static const int GRID_SIZE = 10;
    std::vector<std::vector<int>> cells;
    float cellWidth;
    float cellHeight;
    float cellDepth;
    float minX = -1.0f;
    float minY = -1.0f;
    float minZ = -1.0f;
    float maxX = 1.0f;
    float maxY = 1.0f;
    float maxZ = 1.0f;

    int cellIndex(int cx, int cy, int cz) const
    {
        return (cz * GRID_SIZE + cy) * GRID_SIZE + cx;
    }

    int toCellX(float x) const
    {
        int cx = (int)((x - minX) / cellWidth);
        if (cx < 0) cx = 0;
        if (cx >= GRID_SIZE) cx = GRID_SIZE - 1;
        return cx;
    }

    int toCellY(float y) const
    {
        int cy = (int)((y - minY) / cellHeight);
        if (cy < 0) cy = 0;
        if (cy >= GRID_SIZE) cy = GRID_SIZE - 1;
        return cy;
    }

    int toCellZ(float z) const
    {
        int cz = (int)((z - minZ) / cellDepth);
        if (cz < 0) cz = 0;
        if (cz >= GRID_SIZE) cz = GRID_SIZE - 1;
        return cz;
    }

public:
    void build(PixelMap3d *map)
    {
        cells.clear();
        cells.resize(GRID_SIZE * GRID_SIZE * GRID_SIZE);
        cellWidth = (maxX - minX) / GRID_SIZE;
        cellHeight = (maxY - minY) / GRID_SIZE;
        cellDepth = (maxZ - minZ) / GRID_SIZE;
        for (int i = 0; i < map->size(); i++)
        {
            int cx = toCellX(map->x(i));
            int cy = toCellY(map->y(i));
            int cz = toCellZ(map->z(i));
            cells[cellIndex(cx, cy, cz)].push_back(i);
        }
    }

    void getPixelsInRange(float x, float y, float z, float r, std::vector<int> &result) const
    {
        result.clear();
        int cxCenter = toCellX(x);
        int cyCenter = toCellY(y);
        int czCenter = toCellZ(z);

        int cellsNeededX = (int)std::ceil(r / cellWidth) + 1;
        int cellsNeededY = (int)std::ceil(r / cellHeight) + 1;
        int cellsNeededZ = (int)std::ceil(r / cellDepth) + 1;

        int cxMin = std::max(0, cxCenter - cellsNeededX);
        int cxMax = std::min(GRID_SIZE - 1, cxCenter + cellsNeededX);
        int cyMin = std::max(0, cyCenter - cellsNeededY);
        int cyMax = std::min(GRID_SIZE - 1, cyCenter + cellsNeededY);
        int czMin = std::max(0, czCenter - cellsNeededZ);
        int czMax = std::min(GRID_SIZE - 1, czCenter + cellsNeededZ);

        for (int cz = czMin; cz <= czMax; cz++)
        {
            float cellFrontZ = minZ + cz * cellDepth;
            float cellBackZ = cellFrontZ + cellDepth;
            float closestZ = (z < cellFrontZ) ? cellFrontZ : (z > cellBackZ ? cellBackZ : z);
            float dz = z - closestZ;

            for (int cy = cyMin; cy <= cyMax; cy++)
            {
                float cellBottomY = minY + cy * cellHeight;
                float cellTopY = cellBottomY + cellHeight;
                float closestY = (y < cellBottomY) ? cellBottomY : (y > cellTopY ? cellTopY : y);
                float dy = y - closestY;

                for (int cx = cxMin; cx <= cxMax; cx++)
                {
                    float cellLeftX = minX + cx * cellWidth;
                    float cellRightX = cellLeftX + cellWidth;
                    float closestX = (x < cellLeftX) ? cellLeftX : (x > cellRightX ? cellRightX : x);
                    float dx = x - closestX;

                    if (dx * dx + dy * dy + dz * dz > r * r)
                        continue;

                    const auto &cell = cells[cellIndex(cx, cy, cz)];
                    for (int idx : cell)
                        result.push_back(idx);
                }
            }
        }
    }
};

