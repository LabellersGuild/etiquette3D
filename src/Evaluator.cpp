/* 
 * File:   Evaluator.cpp
 * Author: paulyves
 * 
 * Created on January 14, 2014, 10:01 AM
 */

#include <time.h>
#include <vector>
#include <iostream>
#include <osgText/TextBase>
#include "../include/Evaluator.h"

Evaluator::Evaluator() {
    collision_listPointer = 0;
    collision_frameCounter = 0;
}

Evaluator::Evaluator(const Evaluator& orig) {
    collision_listPointer = orig.collision_listPointer;
    collision_frameCounter = orig.collision_frameCounter;
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

void Evaluator::setLabelList(vector<ref_ptr<lgLabel> > labelList){
    this->labelList = labelList;
}

vector<ref_ptr<lgLabel> > Evaluator::getLabelList(){
    return labelList;
}


float Evaluator::lisibility_checkAlignement(){
    int nonInternalLabel = 0;
    int wrongLabel = 0;
    for (size_t i = 0; i<labelList.size(); i++){
        lgType type = labelList[i].get()->getLabelType();
        if(type!=INTERNAL_TOP || type != INTERNAL_FACE){
            nonInternalLabel++;
            if(labelList[i].get()->getAxisAlignment()!=osgText::Text::SCREEN){
                wrongLabel++;
            }
        }
    }
    if(wrongLabel>0){
        cout<<"Il y a "<<wrongLabel<<" étiquettes pauvrement orient(ées) parmis les "<<nonInternalLabel<<" étiquettes non internes"<<endl;
    } else {
        cout<<"Pas de problème d'orientation dans les étiquettes non internes"<<endl;
    }
    return 100-((float) wrongLabel/(float) nonInternalLabel)*100;             
}

void Evaluator::computeLabelCollision(ref_ptr<osgViewer::Viewer> view, vector<ref_ptr<lgLabel> > etiquettes) {
    //we only start after a certain amount of frame
    if(collision_frameCounter>15){
        int sum = 0;
        int compteur = 0;

        for (size_t i = 0; i < etiquettes.size(); i++) {
            
            lgLabel* label1 = etiquettes[i].get();
            if (label1->getLabelType() == EXTERNAL) {
                Vec4 box1 = label1->compute2dBox(view);
                float minsize1 = std::min(box1.z() - box1.x(), box1.w() - box1.y());
                for (size_t j = i + 1; j < etiquettes.size(); j++) {
                    lgLabel* label2 = etiquettes[j].get();
                    if (label2->getLabelType() == EXTERNAL) {
                        Vec4 box2 = label2->compute2dBox(view);
                        float minsize2 = std::min(box2.z() - box2.x(), box2.w() - box2.y());
                        float minsize = std::min(minsize1, minsize2);
                        float distance = label1->distance2dBox(view, label2);
                        if (distance < 0) {
                            if (distance * (-1) > minsize / 10) {
                                sum += distance * (-1);
                                compteur++;
                            }
                        }
                    }
                }
            }
        }
        if (compteur > 0) {
            cout << "Il y a " << compteur << " chevauchement d'étiquettes" << endl;
        }
        if (collision_listDepth.size() < 2000) {
            collision_listDepth.push_back(compteur > 0 ? sum / compteur : 0);
            collision_listNumber.push_back(compteur);
        } else {
            collision_listDepth.insert(collision_listDepth.begin() + collision_listPointer, compteur > 0 ? sum / compteur : 0);
            collision_listNumber.insert(collision_listNumber.begin() + collision_listPointer, compteur);
            collision_listPointer = (collision_listPointer + 1) % 1999;
        }
        analyseLabelCollision();
    } else {
        collision_frameCounter++;
    }
}

void Evaluator::analyseLabelCollision() {
    // Compute average
    int sumDepth = 0;
    int sumNumber = 0;
    int maxDepth = 0;
    int maxNumber = 0;

    for (int i = 0; i < collision_listDepth.size(); i++) {
        int currentDepth = collision_listDepth[i];
        int currentNumber = collision_listNumber[i];
        sumDepth += currentDepth;
        sumNumber += currentNumber;
        maxDepth = currentDepth > maxDepth ? currentDepth : maxDepth;
        maxNumber = currentNumber > maxNumber ? currentNumber : maxNumber;
    }

    double averageDepth = sumDepth / (double) collision_listDepth.size();
    double averageNumber = sumNumber / (double) collision_listNumber.size();

    cout << "Moyenne des profondeur de collision : " << averageDepth << endl;
    cout << "Maximum des profondeur de collision : " << maxDepth << endl;
    cout << "Moyenne du nombre de collision : " << averageNumber << endl;
    cout << "Maximum du nombre de collision : " << maxNumber << endl;
}
