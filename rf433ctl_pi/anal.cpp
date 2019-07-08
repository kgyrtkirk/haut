
//#include <wiringSerial.h>
//#include <Arduino.h>
#include <wiringPi.h>
#include "rf433ctl.h"
#include <stdlib.h>
#include <stdio.h>
#include <vector>

using namespace std;

#define OUT 8
#define IN 9
#define K	2

void loop();



struct Capture {
	int	    value;
	long	t;
	int		error;

	void	doCapture(){
		value=digitalRead(IN);
		t=micros();
	}
};

struct  Delta {
    int     value;
    long     dt;
    int     error;
};
struct  Signal {
    int     tOn;
    int     tOff;
    int     error;

    bool isStart(){
        return tOff>7000 ;
    }
    bool isNoise(){
        return tOff<100 || tOn<100;
    }
    bool isHead1() {
        int k= tOff+tOn ;
        return 10500<k && k<11000;
    }
    bool isHead2() {
        int k= tOff+tOn ;
        return 6000<k && k<6500;
    }
    bool isX() {
        int k= tOff+tOn ;
        return 1000<k && k<1100;
    }
    bool is0(){
        return isX() && tOn<tOff;
    }
    bool is1(){
        return isX() && tOn>=tOff;
    }
};

vector<Delta>    calcDeltas(vector<Capture>   cap){
    vector<Delta> ret;
    for(unsigned int i=1;i<cap.size();i++){
        Delta   d;
        d.value=cap[i-1].value;
        d.dt=cap[i].t - cap[i-1].t;
        d.error=cap[i].error+cap[i-1].error;

    ret.push_back(d);
    }
    return ret;
}

vector<Signal>    calcSignals(vector<Capture>   cap){
    vector<Signal> ret;
    for(unsigned int i=2;i<cap.size();i++){
        Capture c0=cap[i-2];
        Capture c1=cap[i-1];
        Capture c2=cap[i-0];
        if(c0.value==0)
            continue;
        Signal  s;
        s.tOn=c1.t-c0.t;
        s.tOff=c2.t-c1.t;
        s.error=c0.error+c1.error+c2.error;

        ret.push_back(s);
    }
    return ret;
}

void anal(vector<Signal> v0s){
    vector<int> on;
    Signal agg;
    agg.tOn=agg.tOff=0;
    for(auto it=v0s.begin();it!=v0s.end();++it) {
        Signal   s=(*it);
        agg.tOn+=s.tOn;
        agg.tOff+=s.tOff;
    }

    sort(    v0.begin(),v0.end());
    sort(    v1.begin(),v1.end());
    int n=v0s.size();
    int n2=n/2;
    printf("med: %d/%d  l=%d\n",v0[n2],v1[n2],v0[n2]+v1[n2]);


int n=v0s.size();
    printf("avg: %d/%d\n",agg.tOn/n,agg.tOff/n);

}
int main(int argc,char**argv) {
    vector<Capture>   capList;
    printf("st\n");

    FILE *f=fopen(argv[1],"r");
    if(!f)
    {
    printf("erropen %s\n",argv[1]);
    exit(2);

    }
    while(!feof(f)) {
        Capture c;
        fscanf(f,"%d,%ld,%d",&c.value,&c.t,&c.error);
        if(feof(f))
        break;
        capList.push_back(c);
    }
    fclose(f);
    printf("read: %d\n",capList.size());

    auto deltas= calcDeltas(capList);
    for(auto it=deltas.begin();it!=deltas.end();++it) {
        Delta   d=(*it);
//        printf("dt:  %d %d\n",d.value, d.dt);
    }

    auto signals= calcSignals(capList);
    for(auto it=signals.begin();it!=signals.end();++it) {
        Signal   s=(*it);
//        printf("On\t%d\t%d\t%d\n",s.tOn, s.tOff, s.error);
    }

    int write=0;
    vector<Signal> v0s,v1s,head1,head2;
    vector<char> tokens;
    for(auto it=signals.begin();it!=signals.end();++it) {
        Signal   s=(*it);
        if(s.isStart()) {
            write=1;
        }
        if(write==0){
            continue;
        }   
        if(s.isNoise()) {
            printf(" $\n");
            write=0;
            tokens.clear();
            continue;
        }
//        printf("%d/%d(%d) ",s.tOn,s.tOff,s.error);
        // printf("%d(%d) ",s.tOn+s.tOff,s.error);
        char token=0;
        if(s.isHead1())
            token='H';
        else if(s.isHead2())
            token='I';
        else if(s.is1())
            token='1';
        else if(s.is0())
            token='0';
        else
            printf(" ~%d/%d(%d) ",s.tOn,s.tOff,s.error);

        if(token!=0){
            if(token=='H'){
                if(tokens.size()>0 && tokens[0]=='H') {
                    printf("\nFOUND: ");
                    for(auto it=tokens.begin();it!=tokens.end();++it){
                        printf("%c",*it);
                    }
                    printf("\n");
                }
                tokens.clear();
            }
            printf("%c",token);
            tokens.push_back(token);
        }else{
            tokens.clear();
        }

        if(write==1&& s.isHead1()) {
            head1.push_back(s);
        }
        if(write==2 && s.isHead2()) {
            head2.push_back(s);
        }
        if(s.is1()) {
            v1s.push_back(s);
        }
        if(s.is0()) {
            v0s.push_back(s);
        }
        write++;
//        if(isStart(s));
    }


anal(head1);
anal(head2);
anal(v0s);
anal(v1s);


}