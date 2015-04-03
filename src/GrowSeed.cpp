#include "stdafx.h"
#include "GrowSeed.h"

#include <vector>
#include <map>
#include <assert.h>
#include "util/debstr.h"


GrowSeed::GrowSeed()
{
}


GrowSeed::~GrowSeed()
{
}




const CPoint dirs[4] = { { 0, -1 }, { 1, 0 }, { 0, 1 }, { -1,0 } };
const CPoint diag[4] = { { -1, -1 }, { 1, -1 }, { -1, 1 }, { 1,1 } };


class SeedID { 
public:  
  SeedID(CPoint pos_, int ID_) :pos(pos_), ID(ID_){}
  CPoint pos; 
  int ID; 
};


class PoolItem { 
public:  int ix;  SeedID seed;
  PoolItem() :seed(CPoint(0, 0), -1){}
};


extern void drawCell(CDC& dc, SeedID seed); // { CPoint pos, COLORREF color);

class Simu {
public:
  Simu(GrCanvas& cells_) :cells(cells_) {}

  std::vector<SeedID> pool;
  GrCanvas& cells;
  ACell& cell(SeedID seed) { return cells[seed.pos];  }

  int rndC(int from, int to)  {
    int val = from + rand() % (1 + to - from);
    return val;
  }

  void addpool1() { for (int i = 1; i < GrCanvas::MaxPool; ++i) { 
    CPoint newSeed(rand() % cells.SideW, rand() % cells.SideH); 
    addSeed(SeedID(newSeed,i)); } 
  }

  void addpool2() {
    CSize dim(9, 9);
    int i = 1;
    for (int x = dim.cx / 2; x < cells.SideW; x += dim.cx) {
      for (int y = dim.cy / 2; y < cells.SideH; y += rndC(dim.cy-1,dim.cy+1), ++i) {
        CPoint newSeed(x+rndC(-1,1), y+rndC(-1,1));
        addSeed(SeedID(newSeed,i));
      }
    }
  }

  PoolItem pickRandomSeed() { 
    assert(!pool.empty());
    PoolItem choice;
    choice.ix = rand() % pool.size();
    choice.seed = pool[choice.ix];
    return choice;
  }

  int findOpenAdj(SeedID seed, std::vector<CPoint>& nears) {
    for (int i = 0; i < 4; ++i) {
      CPoint cand = seed.pos + dirs[i];
      if (vacant2(cand, seed.ID)) {
        nears.push_back(cand);
      }
    }
    return nears.size();
  }

  // spell-book, wrong colors compared to potions?
  bool addSeed(SeedID seed) { 
    if (!cells.legalPos(seed.pos)) { debstr() << "illegal pos.";  return false; }
    if (cell(seed).isMarked()) { debstr() << "seed pos was taken." << seed.ID << "\n"; return false; }
    pool.push_back(seed);
    setMark(seed);
    if (dc) { drawCell(*dc, seed); }
    return true;
  }

  void removeSeed(PoolItem seed) { 
    if ((int)pool.size() > 1 && seed.ix < (int) pool.size()-1) {
      pool[seed.ix] = pool.back();
    }
    pool.pop_back();
  }

  CPoint pickAdj(std::vector<CPoint>& nears) {
    assert(!nears.empty());
    int choice = rand() % nears.size();
    return nears[choice];
  }

  bool vacant(CPoint pos) {
    if (pos.x < 0 || pos.y < 0 || pos.x >= cells.SideW || pos.y >= cells.SideH) { return false;  }
    return cells[pos].isClear();
  }

  int idOfCell(CPoint pos) {
    if (pos.x < 0 || pos.y < 0 || pos.x >= cells.SideW || pos.y >= cells.SideH) { return -1;  }
    return cells[pos].c;
  }

  bool vacant2(CPoint pos, int ownSeed) {
    if (!vacant(pos)) { return false; }
    for (int i = 0; i < 4; ++i) {
      CPoint delta = pos + dirs[i];
      if (!vacant(delta) && idOfCell(delta) != ownSeed) { return false; }
    }
    for (int i = 0; i < 4; ++i) {
      CPoint delta = pos + diag[i];
      if (!vacant(delta) && idOfCell(delta) != ownSeed) { return false; }
    }
    return true;
  }

  void setMark(SeedID seed) {    
    ACell& cell = cells[seed.pos]; 
    if (cell.isMarked()) { DebugBreak(); }
    cell.mark(seed.ID); 
  }


  void processPoolItem() {
    PoolItem cur = pickRandomSeed(); ++count;
    std::vector<CPoint> nears;
    if (findOpenAdj(cur.seed, nears) == 0) {
      removeSeed(cur);
    } else {
      addSeed( SeedID( pickAdj(nears), cur.seed.ID) );
    }

    if ((count % 10000) == 0) {  
      debstr() << "lapse:" << count << "/" << pool.size() << "/" << nears.size() 
        << "a " << cur.seed.pos.x << "/" << cur.seed.pos.y
        << "\n"; 
    }
  }

  CDC* dc; 

  void flood(CDC* pDC) {
    dc = pDC; 
    count = 0; remover = 0;
    addpool2();
    while (!pool.empty()) {
      processPoolItem();
    }
    debstr() << "total:" << count << "\n";
  }

  int count, remover;

};





bool runSimu(GrCanvas& cells, CDC* dc) {
  Simu sim(cells); 
  sim.flood(dc);
  return true;
}






std::map< int, COLORREF > idColors;
COLORREF rndColor() { return RGB(rand() % 256, rand() % 256, rand() % 256); }

COLORREF rndColor2(int id) { return 
  !(id%1) ? RGB(0,0,128) : RGB(255,255,128); 
}

COLORREF colorForID(int id) {
  std::map< int, COLORREF >::iterator i; 
  i = idColors.find(id);
  if (i == idColors.end()) {
    idColors[id] = rndColor2(id);
  }
  return idColors[id];
}


void drawCell(CDC& dc, SeedID seed) { // CPoint pos, COLORREF color) { // sim.colorForID(next.ID);
  // dc.SetPixel(pos, color); 
  COLORREF color = colorForID(seed.ID);
  CBrush b(color);
  CSize dim(4, 4);
  CPoint corner(seed.pos.x * dim.cx, seed.pos.y* dim.cy);
  CRect r(corner, dim);
  dc.FillRect(r, &b);
}
