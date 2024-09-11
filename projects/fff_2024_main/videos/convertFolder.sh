# Convert RGB
for f in ~/Downloads/02/Driehoek_Los/*; do python3 convert.py "$f" "/../mapping/singleTriangleMap.json" "."; done
for f in ~/Downloads/02/Driehoeken_Groep/*; do python3 convert.py "$f" "/../mapping/allTriangleMap.json" "."; done

for f in ~/Downloads/02/Cirkels_CentrePiece/*; do python3 convert.py "$f" "/../mapping/ophanimRing1Map.json" "ring1" ; done
for f in ~/Downloads/02/Cirkels_CentrePiece/*; do python3 convert.py "$f" "/../mapping/ophanimRing3Map.json" "ring3" ; done

# Convert RGBA
for f in ~/Downloads/02/Masks/*; do ./convert_with_alpha.sh "$f" "/../mapping/singleTriangleMap.json" ; done