/* 
 * File:   Evaluator.h
 * Author: paulyves
 *
 * Created on January 14, 2014, 10:01 AM
 */

#ifndef EVALUATOR_H
#define	EVALUATOR_H

using namespace std;

class Evaluator {
public:
    Evaluator();
    Evaluator(const Evaluator& orig);
    virtual ~Evaluator();
    
    void realTime_init();
    void realTime_iter();
    void realTime_calcFPS(double timer);
    double realTime_averageFPS();
    
protected:
    clock_t realTime_startClock;
    clock_t realTime_previousClock;
    double realTime_timer;
    vector<double> realTime_listFPS; 
    int realTime_listPointer;
    bool realTime_offset;

};

#endif	/* EVALUATOR_H */

