Keyed-Tileset design:
Keyfile and Image file have same prefix,
so you get tiles1.key, tiles1.png.
The keyfile indicates x,y position of named/tagged tiles.
So external interface offers:
 - you can ask for the graphic-tile to represent a given key string.
The system requires 2-3 things:
 - a parser that reads in and writes key files.
 - a runtime 'hash' to look up this info.
 - a class to read in and hold the image.
 - an editor to click-up and map keys to tiles.
(My tilemapper app.)