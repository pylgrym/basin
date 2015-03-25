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



class Tiles {
public:
  enum SizesEnum { TileWidth=32,TileHeight=32 };
  enum DrawType { Blend=0,Mask=1,Raw=2};

  CString tileFile;
  Tilemap keys;

  bool hasCompatDC;
  CDC compatDC;
  void initCompatDC(CDC& src);

  CImage img;
  // CImage img2;

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

  void drawTile( int x, int y, const TCHAR* key, CDC& dc, Gdiplus::Graphics& gr, DrawType bTransp, int factor, COLORREF color, int& cost, int& numTints);
  void drawTileB(int x, int y, CPoint tilePos,   CDC& dc, Gdiplus::Graphics& gr, DrawType bTransp, int factor, COLORREF color, int& cost, int& numTints);
  void drawTileA(int x, int y, const char*  key, CDC& dc, Gdiplus::Graphics& gr, DrawType bTransp, int factor, COLORREF color, int& cost, int& numTints);

  // requires gdi+:
  void tintTile(CRect& src, CRect& tgt, Gdiplus::Graphics& graphics, COLORREF matColor);
};

