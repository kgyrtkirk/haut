#include <bb_puzzle_gen.h>
#include <stdlib.h>
#include <stdio.h>
#include <list>
#include <vector>

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
#include <Arduino.h>
#define DEBUG
#endif

enum GateType {
    INPUT,
    AND,
    OR,
    XOR,
    NOT,
};

class CGate {
    GateType type;
    int idx;
    list<CGate> operands;
public:
    CGate(int inputIdx) : type(INPUT), idx(inputIdx) {
    }
    CGate(GateType _type, list<CGate> _operands) : type(_type), operands(_operands) {
    }
    bool evaluate(int input) {
        bool val=false;
        switch (type) {
        case INPUT:
            return (input&(1<<idx)) != 0;;
        case NOT:
            return !operands.begin()->evaluate(input);
        case AND:
            for(auto it=operands.begin();it!=operands.end();it++) {
                if(!(*it).evaluate(input)) {
                    return false;
                }
            }
            return true;
        case OR:
            for(auto it=operands.begin();it!=operands.end();it++) {
                if((*it).evaluate(input)) {
                    return true;
                }
            }
        return false;
        case XOR:
            for(auto it=operands.begin();it!=operands.end();it++) {
                val^=(*it).evaluate(input);
            }
            return val;

        default:
            break;
        }
    }
    void  print() {
        switch(type) {
            case AND: DEBUG("AND"); break;
            case OR: DEBUG("OR"); break;
            case XOR: DEBUG("XOR"); break;
            case NOT: DEBUG("NOT"); break;
            case INPUT: DEBUG("I%d",idx); return;
        }
        DEBUG("(");
        for(auto it=operands.begin();it!=operands.end();it++) {
            it->print();
            printf(" ");
        }
        DEBUG(")");
    }


};

class Gate {
public:
    Gate() {};
    virtual ~Gate() {};
    virtual bool evaluate(int input) {}
    virtual void print() { DEBUG("?GATE?"); }
};

class InputGate  : public Gate {
    int idx;
public:
    InputGate(const int&_idx) : idx(_idx) {}
    bool evaluate(int input) { return (input&(1<<idx)) != 0; }
    virtual void print() { DEBUG("I%d",idx); }
};

class NotGate  : public Gate {
     Gate g;
public:
    NotGate(const Gate _g) : g(_g) {}
    bool evaluate(int input) { return !g.evaluate(input); }
    virtual void print() { DEBUG("!"); g.print(); }
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
    virtual void print() {
        DEBUG("AND ( ");
        for(auto it=inputs.begin();it!=inputs.end();it++) {
            it->print();
        }
            DEBUG(")");
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
    virtual void print() { DEBUG("OR"); }
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
    virtual void print() { DEBUG("XOR"); }
};

CGate buildIGate() {
    CGate g=CGate(random(PUZZLE_N));
    if(random(2)) {
        list<CGate> li;
        li.push_back(g);
        g=CGate(NOT, li);
    }
    return g;
}

CGate buildXGate() {
    list<CGate> li;
    for(int i=random(3);i>=0;i--) {
        li.push_back(buildIGate());
    }
    return CGate(XOR, li);
}

CGate buildAGate() {
    list<CGate> li;
    for(int i=random(3);i>=0;i--) {
        li.push_back(buildXGate());
    }
  //  AndGate ret=AndGate(li);
//    cout << ret;
    return CGate(AND,li);
}
/*
ostream& operator<<(ostream& os, const Gate& g) {
    g.print(os);
    return os;
}
ostream& operator<<(ostream& os, const AndGate& g) {
    g.print(os);
    return os;
}
*/
Puzzle genPuzzle(const PuzzleSpec&spec) {
    Puzzle p;
    randomSeed(100);
    vector<CGate> candidates;
    for(int i=0;i<10;i++) {
        CGate g=buildAGate();
        candidates.push_back(g);
        // int r=random(100);
        g.print();
        // for(int j=0;j<256;j++) {
        //     DEBUG("%08x  => %d\n", j,         g.evaluate(j));
        // }
    }
    for(int j=0;j<PUZZLE_N_STATES;j++) {
        vector<int> t;
        for(int i=0;i<10;i++) {
            if(candidates[i].evaluate(j)) {
                t.push_back(i);
            }
            if(t.size() >= PUZZLE_OUTS) {
                DEBUG("FOPUND!");
            }
        }
    }


    DEBUG("\n");
//    Serial.println();
}



void asdasd(){
    
}