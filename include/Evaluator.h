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
    
    //function to set the list of labels that will be the target of evaluation
    void setLabelList(vector<ref_ptr<lgLabel> > labelList);
    vector<ref_ptr<lgLabel> > getLabelList();
    
    //check if non internal label are screen aligned
    //return the percentage of well oriented labels among non internal labels
    //call it outside the viewer loop
    //set debug to true to print details
    float lisibility_checkAlignement(bool debug);
    
    void computeLabelCollision(ref_ptr<osgViewer::Viewer> view, vector<ref_ptr<lgLabel> > labels);
    
    void analyseLabelCollision();
    
    //print some numbers about labels being occluded at a particular distance
    //call this function out of the viewer loop
    void visibilityFilterCalculator();
    
    //print the average percentage of out of camera labels
    //call it inside the viewer loop
    void visibilityOutOfCamera(ref_ptr<osgViewer::Viewer> view);
    
protected:
    clock_t realTime_startClock;
    clock_t realTime_previousClock;
    double realTime_timer;
    vector<double> realTime_listFPS; 
    int realTime_listPointer;
    bool realTime_offset;
    
    vector<int> collision_listNumber;
    vector<int> collision_listDepth;
    int collision_listPointer;
    int collision_frameCounter;
    
    //number of labels out of camera's sight
    vector<int> OOCNumber;
    int OOCPointer;
    int OOCFrameCounter;
    vector<ref_ptr<lgLabel> > labelList;
    

};

#endif	/* EVALUATOR_H */

