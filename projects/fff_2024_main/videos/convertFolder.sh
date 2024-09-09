# Convert RGB
for f in ~/Downloads/02/Driehoek_Los/*; do python3 convert.py "$f" "/../mapping/singleTriangleMap.json" ; done
for f in ~/Downloads/02/Driehoeken_Groep/*; do python3 convert.py "$f" "/../mapping/allTriangleMap.json" ; done

# videos doen we niet zoalngik de puixel aantallen niet heb
#for f in ~/Downloads/02/Cirkels_CentrePiece/*; do python3 convert.py "$f" "/../mapping/allTriangleMap.json" ; done

# Convert RGBA
for f in ~/Downloads/02/Masks/*; do ./convert_with_alpha.sh "$f" "/../mapping/singleTriangleMap.json" ; done