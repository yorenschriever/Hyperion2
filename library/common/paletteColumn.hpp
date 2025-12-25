#pragma once
#include "hyperion.hpp"

void setupPaletteColumn(Hyperion *hyp, int columnIndex=0)
{
    auto paletteColumn = new PaletteColumn(
        &hyp->hub,
        columnIndex,
        0,
        {
            &pinkSunset,
            &sunset8,
            &campfire,
            &heatmap2,
            &sunset2,
            &sunset7,
            &tunnel,
            &redSalvation,
            &plumBath,
            

            &sunset6,
            
            &sunset1,
            &coralTeal,
            &deepBlueOcean,
            
            &heatmap,
            &sunset4,
            &candy,
            &sunset3,
            &greatBarrierReef,
            &blueOrange,
            &peach,
            &denseWater,
            &purpleGreen,
            &sunset5,
            &salmonOnIce,
            &sunset2,
            &retro,
        });
    hyp->hub.subscribe(paletteColumn);
    hyp->hub.setColumnName(columnIndex, "Palette");
    hyp->hub.buttonPressed(columnIndex, 0);
    hyp->hub.setFlashColumn(columnIndex, false, true);
    hyp->hub.setForcedSelection(columnIndex);
}
