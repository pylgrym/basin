#include "stdafx.h"
#include "Tilemap.h"

#include <fstream>
#include "../util/debstr.h"



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





/*
(Moria-specific) void Tilemap::buildCreatureHash() { 

  for (int j = 0; j < MAX_CREATURES; ++j) {
    creature_type& creatureType = c_list[j];
    // CT2A ac(assoc.key, CP_UTF8); // CP_ACP);  // s.c_str()
    CA2T uc(creatureType.name, CP_ACP);  // s.c_str()
    CString myKey=uc; 

    Assoc* pAssoc = getAssoc(myKey);
    if (pAssoc != NULL) {
      creatureTile[j] = pAssoc->tilepos;
    }
  }

}



(Moria-specific) void Tilemap::buildThingHash() {

  for (int j = 0; j < MAX_OBJECTS; ++j) {
    treasure_type& objectType = object_list[j]; // c_list
    // CT2A ac(assoc.key, CP_UTF8); // CP_ACP);  // s.c_str()
    CA2T uc(objectType.name, CP_ACP);  // s.c_str()
    CString myKey=uc; 

    Assoc* pAssoc = getAssoc(myKey);
    if (pAssoc != NULL) {
      thingTile[j] = pAssoc->tilepos;
    }
  }

}


(Moria-specific) bool Tilemap::charFromHash(int myChar, int& x, int& y, int creatureThingIndex, TileEnum tileType) {
  if (creatureThingIndex > 1 && tileType == Ti_Creature) {
    std::map<int, CPoint>::iterator j;
    j = creatureTile.find(creatureThingIndex); // Maps from monster-index to tilepos.
    if (j != creatureTile.end()) {
      x = j->second.x;
      y = j->second.y;
      return true;
    }
  }

  if (creatureThingIndex > -1 && (tileType == Ti_Thing || tileType == Ti_Potion) ) {
    std::map<int, CPoint>::iterator j;
    j = thingTile.find(creatureThingIndex); // Maps from thing-index to tilepos.
    if (j != thingTile.end()) {
      x = j->second.x;
      y = j->second.y;
      return true;
    }
  }


  std::map<CString, Assoc>::iterator i;

  char buf[2] = "a"; buf[0] = myChar;

  CA2T uc(buf, CP_ACP);  
  CString myKey=uc; 

  i = hash.find(myKey);
  if (i == hash.end()) {
    x = 0; y = 0;
    return false;
  }

  x = i->second.tilepos.x;
  y = i->second.tilepos.y;
  return true;
}
*/

void Tiles::drawTileA(int x, int y, const char* key, CDC& dc, bool bTransp, int factor) {
  CA2T ukey(key, CP_ACP);
  CPoint tilePos = tile(CString(ukey));
  drawTileB(x, y, tilePos, dc, bTransp, factor);
}


void Tiles::drawTile(int x, int y, const TCHAR* key, CDC& dc, bool bTransp, int factor) {
  CPoint tilePos = tile(key);
  drawTileB(x, y, tilePos, dc, bTransp, factor);
}

void Tiles::drawTileB(int x, int y, CPoint tilePos, CDC& dc, bool bTransp, int factor) {
  CRect src( CPoint(tilePos.x*TileWidth, tilePos.y*TileHeight), CSize(TileWidth, TileHeight) );
  CRect tgt( CPoint(x*TileWidth, y*TileHeight), CSize(TileWidth, TileHeight) );

  if (bTransp) {
    // JG: NO, this doesn't really work, FIXME/TODO: I still lack something,
    // that will draw/stencil sprites on top of each other!
    // img.Draw(dc, tgt, src); // TransparentBlt is good(no..), AlphaBlend is 'not good', and AlphaBlend is what Draw did.
    // Alpha may still be useful for colouring effects?, e.g. color of a potion fluid.
    //img.TransparentBlt(dc, tgt, src);
    img.AlphaBlend(dc, tgt, src, factor); // blendOp:AC_SRC_OVER == 0.
  } else {
    img.StretchBlt(dc, tgt, src);
  }

}

