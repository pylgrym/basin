#pragma once

#include <queue>
#include <functional> // for greater.


struct MobReady {
  MobReady(double when_, class Mob* mob_) :when(when_), mob(mob_) {}
  MobReady():when(0),mob(NULL) {}

  double when;
  Mob* mob;

  bool operator > (const MobReady& rhs) const;
  
  //  bool operator > (const MobReady& rhs) const { return when > rhs.when; }
  bool persist(class Persist& p); // , class Map* anyMap);
};



template<class T, class Compare>
class PQV : public std::vector<T> { // Needed because std priority-queue is immutable.
  Compare comp;
public:
  PQV(Compare cmp = Compare()) : comp(cmp) {
    makeHeap();
  }
  void makeHeap() { std::make_heap(this->begin(), this->end(), comp); }

  const T& top() { return this->front(); }
  void push(const T& x) {
    this->push_back(x);
    //std::vector<T>::iterator a = this->begin();
    //std::vector<T>::iterator b = this->end();
    std::push_heap(this->begin(), this->end(), comp);
  }
  void pop() {
    std::pop_heap(this->begin(), this->end(), comp);
    this->pop_back();
  }
};



typedef std::deque<MobReady> ReadyDeque;
//typedef std::priority_queue<MobReady, ReadyDeque, std::greater<MobReady> > ReadyQueueB;
//typedef std::priority_queue<MobReady, ReadyDeque, std::less<MobReady>    > ReadyQueue;
typedef PQV<MobReady, std::greater<MobReady> > ReadyQueue;

class MobQueue {
public:
  //int meat1;
  ReadyQueue queue;
  //int meat2;
  // FIXME, maybe should be a set, not a vector:
  // std::vector<Mob*> globalMobs; // an 'owner' of all mobs, no priority.
  double globalClock;

  MobQueue(); // :globalClock(0), meat1(0xDEADBEEF), meat2(0xDEADBEEF) { }

  void queueMob(Mob* mob, double delay) {
    double when = globalClock + delay;
    // MobQueue* that = this;
    queue.push(MobReady(when, mob)); // push_heap.
    // globalMobs.push_back(mob);
    /* push, top, pop. push_heap, make_heap, pop_heap. (sort_heap?)
    */
  }

  void deleteMob(Mob* toDelete);
  bool dispatchFirst();

  bool persist(class Persist& p); // , class Map* anyMapHack);
};

