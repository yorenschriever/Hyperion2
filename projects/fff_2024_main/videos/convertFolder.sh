# Convert RGB
for f in ~/Downloads/02/Driehoek_Los/*; do python3 convert.py "$f" "/../mapping/singleTriangleMap.json" ; done
for f in ~/Downloads/02/Driehoeken_Groep/*; do python3 convert.py "$f" "/../mapping/allTriangleMap.json" ; done

# Convert RGBA
for f in ~/Downloads/02/Masks/*; do ./convert_with_alpha.sh "$f" "/../mapping/singleTriangleMap.json" ; done