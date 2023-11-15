# for f in ~/Downloads/Renders_kelder/*; do python3 convert.py "$f" ; done

for f in ~/Downloads/Masks/*; do ./convert_with_alpha.sh "$f" ; done