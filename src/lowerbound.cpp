#include "lowerbound.h"

using namespace std;

bool sortTestByWidth(const Test* t1, const Test* t2){
    return t1->TAM_width < t2->TAM_width;
}

long int lowerbound2(vector<Test*>& tests, const int& width){
    long int partA = 0;
    long int partB = 0;
    long int partC = 0;    
    
    sort(tests.begin(), tests.end(), sortTestByWidth);
    
    int i = tests.size()-1;
    while(tests[i]->TAM_width > (width/2)){
        partA += tests[i--]->length;
    }
    int tokenC;
    int tokenB= -1;
    // Count L(1)
    for(i = 0; i<tests.size(); i++){
        int w = tests[i]->TAM_width;
        if( ((width-1)>=w) && (w>(width/2)) ){
            tokenC = i;
            partC += (width-w)*tests[i]->length;
        }else if( ((width/2)>=w) && (w>=1) ){
            partB += w*tests[i]->length;
            if(tokenB == -1){ 
                tokenB = i;
            }
        }
    }
    long int lowerbound = partA + max((long)0, (partB - partC)/width);

    for(i = 2 ; i<= width/2 ; i++){
        while(tests[tokenC]->TAM_width > (width-i)){
            partC -= (width - tests[tokenC]->TAM_width)*tests[tokenC]->length;
            tokenC--;
        }
        while(tests[tokenB]->TAM_width < i){
            partB -= tests[tokenB]->TAM_width * tests[tokenB]->length;
            tokenB++;
        }
        lowerbound = max(lowerbound, partA + max((long)0, (partB-partC)/width));
    }
    //return 0;
    return lowerbound;
}
