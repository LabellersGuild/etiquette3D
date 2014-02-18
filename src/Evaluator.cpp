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
#include <numeric>
#include "../include/Evaluator.h"
#include <fstream>

Evaluator::Evaluator() {
    collision_listPointer = 0;
    collision_frameCounter = 0;
    OOCPointer = 0;
    OOCFrameCounter = 0;
	file_output = false;
}

Evaluator::Evaluator(bool file_output_mode) {
    collision_listPointer = 0;
    collision_frameCounter = 0;
    OOCPointer = 0;
    OOCFrameCounter = 0;
	file_output = file_output_mode;
	if(file_output){
		filestream.open ("Evaluation.txt");
		if(NULL == filestream){
			cerr << "Cannot open Evaluator output file";
			file_output = false;
		}
	}
}

Evaluator::Evaluator(const Evaluator& orig) {
    collision_listPointer = orig.collision_listPointer;
    collision_frameCounter = orig.collision_frameCounter;
    OOCPointer = orig.OOCPointer;
    OOCFrameCounter = orig.OOCFrameCounter;
}

Evaluator::~Evaluator() {
}

std::ostream* Evaluator::stream(){
	if(file_output){
		return &filestream;
	}
	return &std::cout;
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
	*stream()<<"FPS moyen: "<<realTime_averageFPS()<<endl;
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


float Evaluator::lisibility_checkAlignement(bool debug){
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
    if(debug){
        if(wrongLabel>0){
            *stream()<<"Il y a "<<wrongLabel<<" étiquettes pauvrement orient(ées) parmis les "<<nonInternalLabel<<" étiquettes non internes"<<endl;
        } else {
            *stream()<<"Pas de problème d'orientation dans les étiquettes non internes"<<endl;
        }
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
            if (label1->getLabelType() == EXTERNAL || label1->getLabelType() == SWITCH) {
                Vec4 box1 = label1->compute2dBox(view);
                float minsize1 = std::min(box1.z() - box1.x(), box1.w() - box1.y());
                for (size_t j = i + 1; j < etiquettes.size(); j++) {
                    lgLabel* label2 = etiquettes[j].get();
                    if (label2->getLabelType() == EXTERNAL || label1->getLabelType() == SWITCH) {
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
            *stream() << "Il y a " << compteur << " chevauchement d'étiquettes" << endl;
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

    *stream() << "Moyenne des profondeur de collision : " << averageDepth << endl;
    *stream() << "Maximum des profondeur de collision : " << maxDepth << endl;
    *stream() << "Moyenne du nombre de collision : " << averageNumber << endl;
    *stream() << "Maximum du nombre de collision : " << maxNumber << endl;
}

void Evaluator::visibilityFilterCalculator(){
    int nbFilter = 0;
    int maxHidingDistance = 0;
    vector<int> distanceVector;
    float averageHidingDistance = 0;
    
    for (size_t i = 0; i<labelList.size(); i++){
        if(labelList[i].get()->getHidingDistance()!=-1){
            nbFilter++;
            distanceVector.push_back(labelList[i].get()->getHidingDistance());
            if(labelList[i].get()->getHidingDistance() > maxHidingDistance){
                maxHidingDistance = labelList[i].get()->getHidingDistance();
            }
        }
    }
    float percent = ((float)nbFilter/(float)labelList.size())*100; 
    *stream()<<"Pourcentage d'étiquettes filtrés à distance : "<<percent<<endl;
    
    if(nbFilter>0){
        averageHidingDistance = (float) std::accumulate(distanceVector.begin(), distanceVector.end(), 0) / (float) distanceVector.size();
        *stream()<<"Distance d'occultation moyenne "<<averageHidingDistance<<endl;
        *stream()<<"Distance d'occultation maximale "<<maxHidingDistance<<endl;
    }
}

void Evaluator::visibilityOutOfCamera(ref_ptr<osgViewer::Viewer> view){
    //we only process after a certain amount of frames
    if(OOCFrameCounter>15){
        int camHeight = view->getCamera()->getViewport()->height();
        int camWidht = view->getCamera()->getViewport()->width();
        int sum = 0;
        for (size_t i = 0; i < labelList.size(); i++) {
            Vec4 boundings = labelList[i].get()->compute2dBox(view);
            if(boundings.x()<0 || boundings.y()<0 || boundings.z()>camWidht || boundings.w()>camHeight){
                sum ++;
            }
        }
        if(OOCNumber.size()<2000){
            OOCNumber.push_back(sum);
        } else {
            OOCNumber.insert(OOCNumber.begin() + OOCPointer, sum);
            OOCPointer = (OOCPointer + 1) % 1999;
        }
        int sumOOC = 0;
        for(int i = 0; i<OOCNumber.size();i++){
            sumOOC += OOCNumber[i];
        }
        float average = ((float)sumOOC)/((float)OOCNumber.size()*labelList.size())*100;
        *stream()<<"Pourcentage d'étiquettes hors camera moyen: "<<average<<endl;
    } else {
        OOCFrameCounter++;
    }
}

/** Compute the 2D distance between the BoundingBox of the Label and the one of the linked Object.
 * used in computeLabelObjectDistance.
 * @param view : ref_ptr<osgViewer::Viewer> : viewer object of the main file
 * @param etiquette : ref_ptr<lgLabel> : the label
 * @return float : the distance
 */
float Evaluator::distanceLabelObject(ref_ptr<osgViewer::Viewer> view, ref_ptr<lgLabel> etiquette) {
    Vec4 ObjectBox = etiquette->computeObject2dBBox(view);
    Vec4 LabelBox = etiquette->compute2dBox(view);

    Vec4 leftBox, rightBox;
    if (ObjectBox.x() < LabelBox.x()) {
        leftBox = ObjectBox;
        rightBox = LabelBox;
    } else {
        leftBox = LabelBox;
        rightBox = ObjectBox;
    }
    //we deduce the box to the top
    Vec4 topBox, botBox;
    if (ObjectBox.y() < LabelBox.y()) {
        topBox = LabelBox;
        botBox = ObjectBox;
    } else {
        topBox = ObjectBox;
        botBox = LabelBox;
    }
    //we then calculate the delta for both abs and ord
    float deltaX = rightBox.x() - leftBox.z();
    float deltaY = topBox.y() - botBox.w();
    float distance;
    //if box touch themselves, we set the distance as negative and a translation from
    // this distance as absolute value should fix the problem
    if (deltaX < 0 && deltaY < 0) {
        distance = -sqrt(pow(deltaX, 2.0) + pow(deltaY, 2.0));
    }
    if (deltaX < 0 && deltaY > 0) {
        distance = deltaY;
    }
    if (deltaX > 0 && deltaY < 0) {
        distance = deltaX;
    }
    if (deltaX > 0 && deltaY > 0) {
        distance = sqrt(pow(deltaX, 2.0) + pow(deltaY, 2.0));
    }
    return distance;
}

/** Compute the average 2D distance between the BoundingBox of the Label and the one of the linked Object.
 * Should be used in the main loop.
 * @param view : ref_ptr<osgViewer::Viewer> : viewer object of the main file
 * @param etiquettes : vector<ref_ptr<lgLabel>> : list of labels
 * @return float : the average distance
 */
float Evaluator::computeLabelObjectDistance(ref_ptr<osgViewer::Viewer> view, vector<ref_ptr<lgLabel> > etiquettes) {
    int sum = 0;
    int compteur = 0;

    for (size_t i = 0; i < etiquettes.size(); i++) {
        lgLabel* label1 = etiquettes[i].get();
        if (label1->getLabelType() == EXTERNAL || label1->getLabelType() == SWITCH) {
            float distance = distanceLabelObject(view, label1);
            if(distance>0)
            {
                sum += distance;
                compteur++;
            }
        }
    }

    float distMoy = (0 == compteur) ? 0 : sum / (float) compteur;
    *stream()<<"Distance moyenne au bâtiment à l'écran "<<distMoy<<endl;
    return distMoy;
}