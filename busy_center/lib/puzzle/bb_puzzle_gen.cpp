#include <bb_puzzle_gen.h>
#include <stdlib.h>
#include <stdio.h>
#include <list>

using namespace std;

#ifdef UNIT_TEST
#include <iostream>
void randomSeed(unsigned long seed)
{
  if (seed != 0) {
    srand(seed);
  }
}

long random(long howbig)
{
  if (howbig == 0) {
    return 0;
  }
  return rand() % howbig;
}

#define DEBUG printf
#else
#define DEBUG
#endif

class Gate {
public:
    Gate() {};
    virtual ~Gate() {};
    virtual bool evaluate(int input) {}
    virtual void print(ostream&os) const  { os << "?GATE?";}
};

class InputGate  : public Gate {
    int idx;
public:
    InputGate(const int&_idx) : idx(_idx) {}
    bool evaluate(int input) { return (input&(1<<idx)) != 0; }
};

class NotGate  : public Gate {
     Gate g;
public:
    NotGate(const Gate _g) : g(_g) {}
    bool evaluate(int input) { return !g.evaluate(input); }
};

class AndGate : public Gate {
    list<Gate> inputs;
public:
    AndGate(list<Gate>&_inputs) : inputs(_inputs) {}
    bool evaluate(int input) { 
        for(auto it=inputs.begin();it!=inputs.end();it++) {
            if(!(*it).evaluate(input)) {
                return false;
            }
        }
        return true;
    }
};
class OrGate : public Gate{
    list<Gate> inputs;
public:
    OrGate(list<Gate>&_inputs) : inputs(_inputs) {}
    bool evaluate(int input) { 
        for(auto it=inputs.begin();it!=inputs.end();it++) {
            if((*it).evaluate(input)) {
                return true;
            }
        }
        return false;
    }
};
class XorGate : public Gate{
    list<Gate> inputs;
public:
    XorGate(list<Gate>&_inputs) : inputs(_inputs) {}
    bool evaluate(int input) { 
        bool val=false;
        for(auto it=inputs.begin();it!=inputs.end();it++) {
            val^=(*it).evaluate(input);
        }
        return val;
    }
};

Gate buildIGate() {
    Gate g=InputGate(random(PUZZLE_N));
    if(random(2)) {
        g=NotGate(g);
    }
    return g;
}

Gate buildXGate() {
    list<Gate> li;
    for(int i=random(3);i>=0;i--) {
        li.push_back(buildIGate());
    }
    return XorGate(li);
}

Gate buildAGate() {
    list<Gate> li;
    for(int i=random(3);i>=0;i--) {
        li.push_back(buildXGate());
    }
  //  AndGate ret=AndGate(li);
//    cout << ret;
    return AndGate(li);
}

ostream& operator<<(ostream& os, const Gate& g) {
    g.print(os);
    return os;
}
ostream& operator<<(ostream& os, const AndGate& g) {
    g.print(os);
    return os;
}

Puzzle genPuzzle(const PuzzleSpec&spec) {
    Puzzle p;
    randomSeed(100);
    for(int i=0;i<10;i++) {
        Gate g=buildAGate();

        int r=random(100);
        cout << g;
        DEBUG("%d ", r);

        
    }
    DEBUG("\n");
//    Serial.println();
}



void asdasd(){
    
}