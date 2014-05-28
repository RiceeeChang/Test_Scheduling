#include <vector>
#include <map>
#include <algorithm>
#include <limits>
#include <cstdio>
#include <cstdlib>
#include "greedy.h"

using namespace std;

int                current_time = 0;
int                next_time = numeric_limits<int>::max();

int                w_avail;
int                p_avail;
vector<Core*>      cores;
map<string, Bist*> bists;
vector<bool>       tam_bus;

vector<Test*>      tests;
vector<Test*>      scheduled;

bool sortTest(const Test* tl, const Test* tr){
    if(tl->length != tr->length)
        return tl->length < tr->length;
    else if(tl->power != tr-> power)
        return tl->power < tr->power;
    else if(tl->TAM_width < tr->TAM_width)
        return tl->TAM_width < tr->TAM_width;
}

void assignTAM(vector<int>& core_bus){
    for(int i = 0; i< core_bus.size(); ){
        for(int j = 0; j<tam_bus.size(); j++){
            if(!tam_bus[j])
                core_bus[i++] = j;
        }
    }
}

void pushIntoScheduled(const int& p){
    Test* t = tests[p];
    t->begin = current_time;
    t->end   = current_time + t->length;
    
    next_time   = current_time + t->length;
    
    p_avail    -= t->power;
    w_avail    -= t->TAM_width;
  
    assignTAM(cores[t->core]->bus);
 
    cores[t->core]->occupied    = true;
    if(t->category == BIST){
        bists[t->resource]->occupied = true;
    }/*else{
        for(int i = 0; i<cores[t->core]->bus.size(); i++)
            tam_bus[cores[t->core]->bus[i]] = true;
    }*/

    scheduled.push_back(t);
    tests.erase(tests.begin() + p);
}

void popFromScheduled(){
     if(!scheduled.empty()){
         Test* t = scheduled.back();
         cores[t->core]->occupied    = false;
         if(t->category == BIST){
             bists[t->resource]->occupied = false;
         }/*else{
             for(int i = 0; i<cores[t->core]->bus.size(); i++)
                 tam_bus[cores[t->core]->bus[i]] = false;
         }*/
         current_time  = t->end;
         p_avail      += t->power;
         w_avail      += t->TAM_width;
         scheduled.pop_back();
     }

     if(!scheduled.empty())
         next_time = scheduled.back()->end;
     else
         next_time = numeric_limits<int>::max();
}

int greedy(System system){
    // Setup
    tests     = system.test;
    w_avail   = system.sys_TAM_width;
    p_avail   = system.sys_power;
    cores     = system.cores;
    bists     = system.bists;
    tam_bus   = system.tam_bus;
    
    sort(tests.begin(), tests.end(), sortTest);

    while(!tests.empty()){
        bool notFindTestFlag = true;
        
        for(int i = tests.size()-1; i>=0 ; i--){
            if((tests[i]->power <= p_avail) && (tests[i]->TAM_width <= w_avail) && (current_time + tests[i]->length <= next_time)){
                //printf("Name = %s\n", tests[i]->name.c_str());
                pushIntoScheduled(i);
                notFindTestFlag = false;
                break;
            }
        }

        if(notFindTestFlag){
            popFromScheduled();
        }
    }

    return next_time;
}
