PixelMap gridMap(int amount_hor, int amount_ver, float distance=0, float center_x=0, float center_y=0)
{
    PixelMap map;

    if (distance==0){
        //calculate distance by filling the entire canvas
        distance = 2. / std::max(amount_hor, amount_ver);
    }

    float start_x  = center_x - ((amount_hor-1) * distance )/2;
    float start_y  = center_y - ((amount_ver-1) * distance )/2;

    for (int y=0; y<amount_ver; y++)
    {
        for (int x=0; x<amount_hor; x++)
        {
            map.push_back({
                .x = start_x + x * distance,
                .y = start_y + y * distance
            });
        }
    }
    return map;
}