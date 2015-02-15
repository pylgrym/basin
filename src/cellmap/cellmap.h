#ifndef CELLMAP_H
#define CELLMAP_H

#include <vector>

#include "util/debstr.h"

#include "Envir.h"
#include "Obj.h"
#include "Creature.h"






class Cell {
public:
  Envir envir;
  ObjSlot item; 
  Creature creature;
  char c;

  Cell() {
    clearChar(); 
  }

  bool charEmpty() const { return c == '\0';  }
  void clearChar() { c = '\0';  }

  bool light() const { return envir.permLight;  } // So far, only envir/floor can contribute to 'cell lighted', but it might be e.g. a glowing monster instead.
  void markWalls() { envir.markWalls();  }

  bool blocked() const {
    // debstr() << "blocked, envir:" << envir.blocked() << "/" << envir.type << ", mob:" << creature.blocked() << "\n";
    return envir.blocked() || creature.blocked();
  }
};



class CellColumn {
public:
  enum ColumnConst { Height = 30 };
  Cell cells[Height];
  Cell& operator [] (int y); // { return cells[y];  }
};



class Map {
public:
  enum MapConst { 
    Width = 48, //16, 
    Height = CellColumn::Height 
  };
private:
  CellColumn cellColumns[Width];
public:
  CellColumn& operator [] (int x); // { return cellColumns[x];  }
  Cell& operator [] (CPoint p); // { return cellColumns[p.x][p.y];  }

  void moveMob(class Mob& m, CPoint newpos);
  void addObj(class Obj& o, CPoint pos);

  Map();

  static Map map;
};



#endif // CELLMAP_H
