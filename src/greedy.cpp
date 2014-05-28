#include <vector>
#include <map>
#include <algorithm>
#include <limits>
#include <cstdio>
#include <cstdlib>
#include "greedy.h"

using namespace std;

int current_time = 0;

bool sortTest(const Test* tl, const Test* tr){
    if(tl->length != tr->length)
        return tl->length < tr->length;
    else if(tl->power != tr-> power)
        return tl->power < tr->power;
    else if(tl->TAM_width < tr->TAM_width)
        return tl->TAM_width < tr->TAM_width;
}

void updateTest(Test*& test){
    test->begin = current_time;
    test->end = current_time + test->length;
}

int greedy(System system){
    // Setup
    vector<Test*> tests = system.test;
    int w_avail = system.sys_TAM_width;
    int p_avail = system.sys_power;
    vector<Core*> cores = system.cores;
    map<string, Bist*> bists = bists;
    vector<bool> tam_bus = system.tam_bus;

    int next_time = numeric_limits<int>::max();
    vector<Test*> scheduled;
    
    sort(tests.begin(), tests.end(), sortTest);


    while(!tests.empty()){
        Test* test = NULL;
        int i;
        
        for(i = tests.size()-1; i>=0 ; i--){
            if((tests[i]->power <= p_avail) && (tests[i]->TAM_width <= w_avail) && (current_time + tests[i]->length <= next_time)){
                printf("Name = %s\n", tests[i]->name.c_str());
                test = tests[i];
                updateTest(tests[i]);
                p_avail -= test->power;
                w_avail -= test->TAM_width;
                next_time = current_time + test->length;
                scheduled.push_back(test);
                tests.erase(tests.begin() + i);
                break;
            }
        }

        //printf("(P, W) = (%d, %d)\n", p_avail, w_avail);

        if(test == NULL){
            p_avail += scheduled.back()->power;
            w_avail += scheduled.back()->TAM_width;
            if(!scheduled.empty())
                current_time = scheduled.back()->end;
            scheduled.pop_back();
            if(!scheduled.empty())
                next_time = scheduled.back()->end;
            else
                next_time = numeric_limits<int>::max();
        }
    }

    return next_time;
}
