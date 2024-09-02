# Convert RGB
# for f in ~/Downloads/Driehoek_Los/*; do python3 convert.py "$f" "/../mapping/singleTriangleMap.json" ; done

# Convert RGBA
for f in ~/Downloads/Masks_2/*; do ./convert_with_alpha.sh "$f" "/../mapping/singleTriangleMap.json" ; done