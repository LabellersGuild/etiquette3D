/* 
 * File:   Evaluator.cpp
 * Author: paulyves
 * 
 * Created on January 14, 2014, 10:01 AM
 */

#include <time.h>
#include <vector>
#include <iostream>
#include "../include/Evaluator.h"

Evaluator::Evaluator() {
}

Evaluator::Evaluator(const Evaluator& orig) {
}

Evaluator::~Evaluator() {
}

//invoke before the loop on viewer
void Evaluator::realTime_init(){
    realTime_startClock = clock();
    realTime_timer = 0;
    realTime_offset = false;
    realTime_listPointer = 0;
}

//invoke inside the loop on viewer, at the end
void Evaluator::realTime_iter(){
    if(realTime_offset == false){
        realTime_timer = (clock() - realTime_startClock) / (double) CLOCKS_PER_SEC;
        if(realTime_timer>5){
            realTime_offset=true;
        }
    }
    //we start the process after a certain time
    if(realTime_offset){
        realTime_timer = (clock() - realTime_previousClock) / (double) CLOCKS_PER_SEC;
        realTime_calcFPS(realTime_timer);
    }
    realTime_previousClock = clock();
}

void Evaluator::realTime_calcFPS(double timer){
    double FPS = 1 / timer;
    if(realTime_listFPS.size()<100){
        realTime_listFPS.push_back(FPS);
    } else {
        realTime_listFPS.insert(realTime_listFPS.begin()+realTime_listPointer, FPS);
        realTime_listPointer = (realTime_listPointer+1)%99; 
    }
    cout<<"FPS moyen: "<<realTime_averageFPS()<<endl;
}

double Evaluator::realTime_averageFPS(){
    double sum = 0;
    double average;
    for(size_t i = 0; i < realTime_listFPS.size(); i++){
        sum += realTime_listFPS[i];
    }
    average = sum / realTime_listFPS.size();
    return average;
}
