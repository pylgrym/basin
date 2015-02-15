#pragma once

enum CreatureEnum {
  CR_None=0,
  CR_Player=1,
  CR_Kobold=2,
  CR_Dragon=3,
  CR_MaxLimit // highest nr to size arrays.
};

class Creature {
public:
  CreatureEnum type() const;
  class Mob* m;
  Creature():m(NULL){}

  const TCHAR* typeS() { return typeAsStr(type());  }

  bool empty() const { return type() == CR_None; }
  bool blocked() const { return isBlockingCreature(type());  }

  void clearMob() { m = NULL; }  
  void setMob(Mob* m_) { m = m_; }

  static const TCHAR* typeAsStr(CreatureEnum type);
  static const TCHAR* ctypeAsDesc(CreatureEnum type);

  static bool isBlockingCreature(CreatureEnum type);
};
