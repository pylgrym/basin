#include "stdafx.h"
#include "MobQueue.h"

#include "Mob.h"

//MobQueue CL->mobs;


MobQueue::MobQueue():globalClock(0)// ,meat1(0xDEADBEEF), meat2(0xDEADBEEF)   // HOLD OP MED DE SATANS GLOBALE VARIABLE! .. :-).
{}




bool MobReady::operator > (const MobReady& rhs) const { 
  if (when == rhs.when) {
    return mob > rhs.mob;
  }

  return when > rhs.when; 
}




void MobQueue::deleteMob(Mob* toDelete) {

  CL->map[toDelete->pos].creature.clearMob(); // Remove it from the map description.
  toDelete->invalidateGfx(); // Tell graphics system it needs to be redrawn.

  /// std::vector<Mob*>::iterator i;
  /// i = std::find(globalMobs.begin(), globalMobs.end(), toDelete);
  /// if (i != globalMobs.end()) { globalMobs.erase(i); }

  // JG, : I couldn't implement, because I need my own prio-queue-deque-heap to allow removal.
  ReadyQueue::iterator j; 
  for (j = queue.begin(); j != queue.end(); ++j) { // FIXME - is there a better way to search?
    MobReady& mr = *j;
    if (mr.mob == toDelete) { break;  }
  }
  if (j != queue.end()) { 
    queue.erase(j); 
    queue.makeHeap(); // re-establish heap.
    // http://www.linuxtopia.org/online_books/programming_books/c++_practical_programming/c++_practical_programming_189.html
  }
  delete toDelete;
}




bool MobQueue::dispatchFirst() {
  if (queue.empty()) { debstr() << "mob queue is empty, bailing out.\n"; return false; }

  MobReady cur = queue.top();
  globalClock = cur.when; // update clock to next 'threshold time'.
  // debstr() << "dispatchFirst begin, mob:" << cur.mob->rep() << ", time:" << globalClock << " " << (void*) cur.mob << "\n";

  // Keep doing actions until a non-zero action is done:
  double duration=0;
  for ( ; duration==0 && !cur.mob->isDead(); ) { duration = cur.mob->act(); } 

  // We don't pop until after 'act()' are finished (ensures queue always contains 'everybody'.)
  queue.pop(); // pop_heap / take us out of the queue.

  if (!cur.mob->isDead()) { // As long as you are not dead, you get a next turn:
    double nextReady = globalClock += duration;
    queue.push(MobReady(nextReady,cur.mob));
  } // if-not-dead.
  // NB, something needs to clean up/delete dead mobs!

  return !cur.mob->isDead(); 
} // dispatchFirst.




bool MobReady::persist(Persist& p) {
  p.transfer(when, "when");

  int isPlayer = (mob ? mob->isPlayer() : 0);
  p.transfer(isPlayer, "isPlayer");

  /* FIXME/refactor: figure out a more elegant way to persist player a single time,
  and still keep him in many queues.
  */
  bool bFirstPlayer = false;
  if (!p.bOut) { // IE it's read-in.
    if (isPlayer) {
      if (PlayerMob::ply == NULL) { // Only create a single instance.
        mob = new PlayerMob;
        bFirstPlayer = true;
      } else {
        mob = PlayerMob::ply;
      }
    } else {
      mob = new MonsterMob(1);
    } // in MobReady::persist.
  }
  bool bOK = true;

  // Sorry - all this messy code, to handle  that we transfer player multiple times :-(.
  if (isPlayer && !p.bOut && !bFirstPlayer) {
    // do nothing - don't read in player more than once.
    static MonsterMob dummyPlayer(1);
    dummyPlayer.persist(p); // We still need to 'eat/parse' that object.
  } else { // if monster, or first-time-the-player, read in:
    bOK = mob->persist(p);
  }


  return bOK;
}


bool MobQueue::persist(class Persist& p) {
  int mobCount = queue.size();
  p.transfer(mobCount, "mobCount");

  if (p.bOut) {
    ReadyQueue::iterator i;
    for (i = queue.begin(); i != queue.end(); ++i) {
      MobReady& mr = *i;
      mr.persist(p);
    }
  } else {
    for (int i = 0; i < mobCount; ++i) {
      MobReady mr;
      mr.persist(p);
      queueMob(mr.mob, mr.when);      
    }
  }
  return true;
}
