#include <bb_puzzle_gen.h>
#include <stdlib.h>
#include <stdio.h>
#include <list>
#include <vector>

using namespace std;

#ifndef ESP8266
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
#define DEBUG printf
#endif

enum GateType {
    GATE_INPUT,
    GATE_AND,
    GATE_OR,
    GATE_XOR,
    GATE_NOT,
};

class CGate {
    GateType type;
    int idx;
    list<CGate> operands;
public:
    CGate(int inputIdx) : type(GATE_INPUT), idx(inputIdx) {
    }
    CGate(GateType _type, list<CGate> _operands) : type(_type), operands(_operands) {
    }
    bool evaluate(int input) {
        bool val=false;
        switch (type) {
        case GATE_INPUT:
            return (input&(1<<idx)) != 0;;
        case GATE_NOT:
            return !operands.begin()->evaluate(input);
        case GATE_AND:
            for(auto it=operands.begin();it!=operands.end();it++) {
                if(!(*it).evaluate(input)) {
                    return false;
                }
            }
            return true;
        case GATE_OR:
            for(auto it=operands.begin();it!=operands.end();it++) {
                if((*it).evaluate(input)) {
                    return true;
                }
            }
        return false;
        case GATE_XOR:
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
            case GATE_AND: DEBUG("AND"); break;
            case GATE_OR: DEBUG("OR"); break;
            case GATE_XOR: DEBUG("XOR"); break;
            case GATE_NOT: DEBUG("NOT"); break;
            case GATE_INPUT: DEBUG("I%d",idx); return;
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
        g=CGate(GATE_NOT, li);
    }
    return g;
}

CGate buildXGate() {
    list<CGate> li;
    for(int i=random(3);i>=0;i--) {
        li.push_back(buildIGate());
    }
    return CGate(GATE_XOR, li);
}

CGate buildAGate() {
    list<CGate> li;
    for(int i=random(3)+1;i>=0;i--) {
        li.push_back(buildXGate());
    }
  //  AndGate ret=AndGate(li);
//    cout << ret;
    return CGate(GATE_AND,li);
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

#define GEN_K (PUZZLE_OUTS+1)

Puzzle fillPuzzle(    vector<CGate> candidates ) {
    Puzzle p;
for(int i=0;i<candidates.size();i++) {
            candidates[i].print();
        DEBUG("\r\n");
}

    for(int j=0;j<PUZZLE_N_STATES;j++) {
        int s=0;
        for(int i=0;i<candidates.size();i++) {
            if(candidates[i].evaluate(j)) {
                s|=1<<i;
            }
        }
        p.state[j]=s;
        if(s==(1<<PUZZLE_OUTS) - 1) {
            DEBUG("solution: %04x\n",j);
        }
    }
    p.valid=true;
    return p;
}

Puzzle genOnePuzzle(const PuzzleSpec&spec) {
    Puzzle p;
    vector<CGate> candidates;
    for(int i=0;i<GEN_K;i++) {
        CGate g=buildAGate();
        candidates.push_back(g);
        g.print();
        DEBUG("\r\n");
    }
    for(int j=0;j<PUZZLE_N_STATES;j++) {
        vector<int> t;
        for(int i=0;i<GEN_K;i++) {
            if(candidates[i].evaluate(j)) {
                t.push_back(i);
            }
            if(t.size() >= PUZZLE_OUTS) {
                DEBUG("FOUND!\r\n");
                // populate puzzle
                vector<CGate> pfn;
                for(int k=0;k<PUZZLE_OUTS;k++) {
                    pfn.push_back(candidates[t[k]]);
                }
                return fillPuzzle(pfn);
            }
        }
    }
    p.valid=false;
    return p;
}

Puzzle genPuzzle(const PuzzleSpec&spec) {
    Puzzle p;
    randomSeed(spec.seed);
    for(int t=0;t<10;t++) {
        p = genOnePuzzle(spec);
        if(p.valid) {
            return p;
        }
    }
    return p;
}

