/* 
 * File:   Evaluator.h
 * Author: paulyves
 *
 * Created on January 14, 2014, 10:01 AM
 */

#ifndef EVALUATOR_H
#define	EVALUATOR_H
#include "lgLabel.h"
using namespace std;
using namespace osg;

class Evaluator {
public:
    Evaluator();
    Evaluator(const Evaluator& orig);
    virtual ~Evaluator();
    
    //fonction d'initialisation pour le calcul des FPS
    void realTime_init();
    //fonction à appeler à chaque itération du viewer
    void realTime_iter();
    //fonctions pour calculer les FPS moyens
    void realTime_calcFPS(double timer);
    double realTime_averageFPS();
    
    
    void setLabelList(vector<ref_ptr<lgLabel> > labelList);
    vector<ref_ptr<lgLabel> > getLabelList();
    
    //check if non internal label are screen aligned
    //return the number of poorly designed labels
    int lisibility_checkAlignement();
    
    
protected:
    clock_t realTime_startClock;
    clock_t realTime_previousClock;
    double realTime_timer;
    vector<double> realTime_listFPS; 
    int realTime_listPointer;
    bool realTime_offset;
    
    vector<ref_ptr<lgLabel> > labelList;
    

};

#endif	/* EVALUATOR_H */

