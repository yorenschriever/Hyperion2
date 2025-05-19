# Convert RGB
for f in ./src/*; do python3 convert.py "$f" "/../mapping/videoMap.json" ; done

# Convert RGBA
# for f in ~/Downloads/Masks/*; do ./convert_with_alpha.sh "$f" "/../mapping/map.json"; done