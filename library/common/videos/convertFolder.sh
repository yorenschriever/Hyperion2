# Convert RGB
# for f in ~/Downloads/Renders_kelder/*; do python3 convert.py "$f" "/../mapping/map.json" ; done

# Convert RGBA
for f in ~/Downloads/Masks/*; do ./convert_with_alpha.sh "$f" "/../mapping/map.json"; done