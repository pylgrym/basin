#include "stdafx.h"
#include "Tilemap.h"

#include <fstream>
#include "../util/debstr.h"

#include "../numutil/myrnd.h"
#include "myconsts.h"

#include <gdiplus.h>
using namespace Gdiplus;
#pragma comment (lib,"Gdiplus.lib")

#include "../Envir.h"

Tilemap::Tilemap()
{
}



Tilemap::~Tilemap()
{
}


bool Tilemap::save(CString filename) {
  std::ofstream f(filename);
  std::vector<Assoc>::iterator i;
  for (i = tileAssocs.begin(); i != tileAssocs.end(); ++i) {
    Assoc& assoc = *i;

    CT2A ac(assoc.key, CP_ACP); // CP_UTF8);

    f << '"' << (const char*)ac << '"' << " "; 
    f << assoc.tilepos.x << " ";
    f << assoc.tilepos.y << std::endl;
  }
  return true;
}



bool Tilemap::load(CString filename) {
  std::ifstream f(filename);

  debstr() << "\nitems..\n";
  while (f.good() && !f.bad()) { 

    std::string line;
    std::getline(f, line);

    // Approach: We find first quote and last quote,EVERYTHING in between is the key. This way, a string may even contain a quote..
    size_t lastQuote = line.find_last_of('"');
    if (lastQuote == std::string::npos) { continue; } // error.
    size_t firstQuote = line.find_first_of('"');
    assert(firstQuote != lastQuote);
    std::string key = line.substr(firstQuote+1, (lastQuote - firstQuote)-1);

    std::string numPart = line.substr(lastQuote + 2);

    // JG: this is brittle against bad formatting
    // - would be better if it parsed for found integers.
    size_t numDivider = numPart.find_first_of(' '); 
    std::string leftNum = numPart.substr(0, numDivider);
    std::string rightNum = numPart.substr(numDivider+1);
    int x = stoi(leftNum);
    int y = stoi(rightNum);

    CA2T uc(key.c_str(), CP_ACP);  
    CString s = uc;

    Assoc assoc;
    assoc.key = s;
    assoc.tilepos.x = x;
    assoc.tilepos.y = y;

    debstr() << "tile:[" << assoc.tilepos.x << " / " << assoc.tilepos.y << "]'" << key.c_str() << "'\n";
    tileAssocs.push_back(assoc);
  }
  return true;
}





Assoc* Tilemap::getAssoc(CString key) { // Helper for hash-builders.
  std::map<CString, Assoc>::iterator i;
  i = hash.find(key);
  if (i == hash.end()) { return NULL;  }
  return &i->second;
}


void Tilemap::buildHashes() { 
  // First, a basic hash-map between text-keys and the assocs:
  std::vector<Assoc>::iterator i;
  for (i = tileAssocs.begin(); i != tileAssocs.end(); ++i) {
    Assoc& assoc = *i;
    hash[assoc.key] = assoc;
  }

  // Then, special-purpose faster hashes:
  // buildCreatureHash();
  // buildThingHash();
}



void Tiles::initCompatDC(CDC& src) {
  assert(false); // not active currently.
  /*
  assert(!hasCompatDC);
  compatDC.CreateCompatibleDC(&src);
  compatDC.SelectObject((HBITMAP)img2);
  hasCompatDC = true;
  */
}


Tiles::Tiles()
:imgPlus(NULL) //L"") 
,hasCompatDC(false)
{
  tileFile = L"sprites\\tiles1"; 
  bool bKeyOK = keys.load(keyFile());
  keys.buildHashes();

  bool bImgOK = (img.Load(imgFile()) > 0);
  // bImgOK = (img2.Load(imgFile()) > 0);

  Gdiplus::Image readImg(imgFile());
  imgPlus = readImg.Clone();

  Envir::initEnvirs(*this);
}


void Tiles::drawTileA(int x, int y, const char* key, CDC& dc, Graphics& gr, DrawType bTransp, int factor, COLORREF color, int& cost, int& numTints) {
  CA2T ukey(key, CP_ACP);
  CPoint tilePos = tile(CString(ukey)); //CPoint(3, 4); // 
  drawTileB(x, y, tilePos, dc, gr, bTransp, factor, colorNone,cost, numTints);
}


void Tiles::drawTileU(int x, int y, const TCHAR* key, CDC& dc, Graphics& gr, DrawType bTransp, int factor, COLORREF color, int& cost, int& numTints) {
  CPoint tilePos = tile(key); //CPoint(2, 3); // 
  drawTileB(x, y, tilePos, dc, gr, bTransp, factor, color,cost,numTints);
}

    // JG: NO, this doesn't really work, FIXME/TODO: I still lack something,
    // that will draw/stencil sprites on top of each other!
    // img.Draw(dc, tgt, src); // TransparentBlt is good(no..), AlphaBlend is 'not good', and AlphaBlend is what Draw did.
    // Alpha may still be useful for colouring effects?, e.g. color of a potion fluid.
    //img.TransparentBlt(dc, tgt, src);


/* Idea: I suspect the slow drawing is really caused by tile-lookup!
I can verify this, by assigning stock-tiles to mobs, items, envir.

sadly, it doesn't seem to be it :-(
then, we have letter-drawing.

fix: objdesc items should have tile-key assigned on start-up!
*/

void Tiles::drawTileB(int x, int y, CPoint tilePos, CDC& dc, Graphics& gr, DrawType bTransp, int factor, COLORREF color, int& cost, int& numTints) {
  CRect src( CPoint(tilePos.x*TileWidth, tilePos.y*TileHeight), CSize(TileWidth, TileHeight) );
  CRect tgt( CPoint(x*TileWidth, y*TileHeight), CSize(TileWidth, TileHeight) );

  if (color != colorNone) { //  && oneIn(2) ) {
    ++cost;
    if (bTransp == Raw) { // We are just drawing/coloring floor!
       CBrush brush(color); // Just  the color..
      dc.FillRect(&tgt, &brush);
      return;
    }

    tintTile(src, tgt, gr, color);
    ++numTints;
    return;
  } // http://www.codeproject.com/Articles/5034/How-to-implement-Alpha-blending


  // if (!hasCompatDC) { initCompatDC(dc);  } // Make sure we have dc-bitmap.

  // bTransp = Raw; // Even with raw BitBlt, I get 150ms = long time! (15% of a second.)
  COLORREF colTrans = RGB(1, 2, 3);    //COLORREF colTrans = RGB(64, 128, 192);
  switch (bTransp) {
  case Raw:   
    {
      if (1) {
        img.BitBlt(dc, tgt, src.TopLeft(), SRCCOPY); break;// Hmm sems a bit faster than img.StretchBlt(dc, tgt, src);
      } else { // Hmm, compatDC didn't seem to help/speed-up :-(
        dc.BitBlt(tgt.left, tgt.top, tgt.Width(), tgt.Height(), &compatDC, src.left, src.top, SRCCOPY);
      }
    }
    break;

  case Mask:  img.TransparentBlt(dc, tgt, src, colTrans); break; // blendOp:AC_SRC_OVER == 0.

  case Blend: img.AlphaBlend(dc, tgt, src, factor); break; // blendOp:AC_SRC_OVER == 0.
  }

} // end drawTileB.



void Tiles::tintTile(CRect& src, CRect& tgt, Gdiplus::Graphics& graphics, COLORREF matColor) {

  Gdiplus::Rect dest2( tgt.left,tgt.top,  tgt.Width(),tgt.Height()); 
  Color tintingColor( GetRValue(matColor), GetGValue(matColor), GetBValue(matColor) ); 

	float cr = tintingColor.GetRed()   / 255.0f;
  float cg = tintingColor.GetGreen() / 255.0f;
  float cb = tintingColor.GetBlue()  / 255.0f;

  ColorMatrix colorMatrix = {
    // 1,0,0,0,0, 
    cr, cg, cb, 0, 0,
    // 0,1,0,0,0, 
    cb, cr, cg, 0, 0,
    // 0,0,1,0,0, 
    cg, cb, cr, 0, 0,
    0, 0, 0, 1, 0,
    0, 0, 0, 0, 1
  }; 

  ImageAttributes  imageAttributes;
	imageAttributes.SetColorMatrix( &colorMatrix,  ColorMatrixFlagsDefault, ColorAdjustTypeBitmap );   

  // graphics.SetCompositingMode(CompositingModeSourceOver); // CompositingModeSourceCopy); // CompositingModeSourceOver);
	graphics.DrawImage(
	   imgPlus,  
	   dest2,             // destination rectangle 
	   src.left, src.top, // upper-left corner of source rectangle 
	   TileWidth,         // width of source rectangle
	   TileHeight,        // height of source rectangle
	   UnitPixel,
	   &imageAttributes
  );

}
