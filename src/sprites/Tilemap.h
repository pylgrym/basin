#pragma once

#include <vector>
#include <map>

#include <atlimage.h> // For CImage.

#include <assert.h>

#include <gdiplus.h>

class Assoc {
public:
  CString key;
  CPoint tilepos;
  Assoc(CString key_, int x, int y):key(key_), tilepos(x,y){}
  Assoc(){}
};

class Tilemap
{
public:
  Tilemap();
  ~Tilemap();

  // EDIT MODE:
  std::vector<Assoc> tileAssocs;

  bool load(CString filename);
  bool save(CString filename);

  // ACCESS/RUNTIME MODE:
  // std::vector<Assoc> hash;
  std::map<CString,Assoc> hash;
  void buildHashes();

  // void buildCreatureHash();
  // void buildThingHash();
  // bool charFromHash(int myChar, int& x, int& y, int creatureThingIndex, TileEnum tileType);

  Assoc* getAssoc(CString key);
  std::map<int, CPoint> creatureTile; // Maps from monster-index to tilepos.
  std::map<int, CPoint> thingTile; // Maps from thing-index to tilepos.

};


const COLORREF colorNone = RGB(1, 2, 3);

class Tiles {
public:
  enum SizesEnum { TileWidth=32,TileHeight=32};

  CString tileFile;
  Tilemap keys;
  CImage img;

  Gdiplus::Image* imgPlus;

  CString keyFile() const { return tileFile + L".key";  }
  CString imgFile() const { return tileFile + L".png";  }

  Tiles();

  CPoint tile(const CString& key) { 
    assert(keys.tileAssocs.size() > 0);
    Assoc* a = keys.getAssoc(key);
    if (a != NULL) {
      return a->tilepos;
    }
    // No tile for string - return some 'none-tile':
    CPoint nonePos(0, 0);
    return nonePos;
  }

  void drawTile( int x, int y, const TCHAR* key, CDC& dc, Gdiplus::Graphics& gr, bool bTransp, int factor, COLORREF color, int& cost);
  void drawTileB(int x, int y, CPoint tilePos,   CDC& dc, Gdiplus::Graphics& gr, bool bTransp, int factor, COLORREF color, int& cost);
  void drawTileA(int x, int y, const char*  key, CDC& dc, Gdiplus::Graphics& gr, bool bTransp, int factor, int& cost);

  // requires gdi+:
  void tintTile(CRect& src, CRect& tgt, Gdiplus::Graphics& graphics, COLORREF matColor);
};

