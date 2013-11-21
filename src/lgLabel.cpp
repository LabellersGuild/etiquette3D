/*
 * File:   lgLabel.cpp
 * Author: paulyves
 *
 * Created on October 19, 2013, 4:31 PM
 */

#include <osgViewer/Viewer>
#include <osg/MatrixTransform>
#include "../include/LGAnimation.h"
#include "../include/myLGAnimation.h"
#include "../include/lgLabel.h"
using namespace std;

//constructors
lgLabel::lgLabel() : osgText::Text(){
}

lgLabel::lgLabel(const lgLabel& originalLabel) {
}

lgLabel::lgLabel(std::string text, osg::ref_ptr<osg::Node> linkedNode, osgViewer::Viewer* viewer) {
    this->setText(text);
    this->setLinkNode(linkedNode, viewer);
}

lgLabel::lgLabel(std::string filePath, std::string idNode) {

}

//getters and setters
/**
 * set the param as linkNode attribute, then see if it got the
 * label in his children, if not it adds it (eventually creating a new
 * geode if the param is a group)
 * @param aNode, osg:ref_ptr<osg::Node> to the node
 */
void lgLabel::setLinkNode(osg::ref_ptr<osg::Node> aNode, osgViewer::Viewer* viewer){
    this->linkNode = aNode;
    osg::ref_ptr<osg::Group> targetGroup = dynamic_cast<osg::Group*>(linkNode.get());
    osg::ref_ptr<osg::Geode> targetGeode = dynamic_cast<osg::Geode*>(linkNode.get());
    osg::ref_ptr<osg::MatrixTransform> mtLabel1 = new osg::MatrixTransform;
    if(!targetGeode && targetGroup){
        targetGeode = new osg::Geode();
        targetGroup->addChild(mtLabel1);
        mtLabel1->addChild(targetGeode);
        this->updatedMatrix=mtLabel1;
        mtLabel1->setUpdateCallback( new myLGAnimation(viewer));
    }
    if(targetGeode){
        //todo gérer le cas où on a direct une géode
        bool alreadyChild = false;
        for (int i = 0; i < targetGeode->getNumDrawables(); i++){
            osg::ref_ptr<lgLabel> drawAsLabel = dynamic_cast<lgLabel*>(targetGeode->getDrawable(i));
            if(drawAsLabel && drawAsLabel.get()==this){
                alreadyChild = true;
            }
        }
        if(!alreadyChild){
            targetGeode->addDrawable(this);
        }
    }
}


osg::ref_ptr<osg::Node> lgLabel::getLinkNode() {
    return this->linkNode;
}

//calculate the absolute position thanks to the position of linknode
void lgLabel::calcAbsolutePosition() {
    if(this->getLinkNode() ){
        osg::Vec4 extendedPosition = osg::Vec4(this->getPosition(), 1);
        //getting the list of transposed transformation matrices, from node to root
        osg::MatrixList matricesList = updatedMatrix->getWorldMatrices();
        for (int i=0;i<matricesList.size();i++){
            extendedPosition = extendedPosition*matricesList[i];
        }
        this->absolutePosition = osg::Vec3(extendedPosition.x(),extendedPosition.y(),extendedPosition.z());
    }
}

osg::Vec3 lgLabel::getAbsolutePosition() {
    if(this->getLinkNode()){
        this->calcAbsolutePosition();
        return this->absolutePosition;
    } else {
        return osg::Vec3(0,0,0);
    }
}


void lgLabel::setPosition(osg::Vec3 relativePosition) {
    osgText::Text::setPosition(relativePosition);
    this->calcAbsolutePosition();
}

std::string lgLabel::getLabelType(){
    return this->labelType;
}

void lgLabel::setLabelType(std::string labelType){
    this->labelType=labelType;
}

bool lgLabel::getInternal() {
    return this->internal;
}
void lgLabel::setInternal (bool internal) {
    this->internal = internal;
}

/*
 *Set the initial position in the attribute positionInit
 * but also call the setPosition method with the same argument
 */
void lgLabel::setPositionInit(osg::Vec3 newPositionInit){
    positionInit = newPositionInit;
    this->setPosition(positionInit);
}

osg::Vec3 lgLabel::getPositionInit(){
    return positionInit;
}


float lgLabel::distance(osg::ref_ptr<lgLabel> otherLabel){
    osg::Vec3 myPos = this->getAbsolutePosition();
    osg::Vec3 otherPos = otherLabel->getAbsolutePosition();
    float distance = sqrt(pow(myPos.x()-otherPos.x(),2.0)+pow(myPos.y()-otherPos.y(),2.0)+pow(myPos.z()-otherPos.z(),2.0));
    return distance;
}

osg::Vec3 lgLabel::distanceVec(osg::ref_ptr<lgLabel> otherLabel){
    osg::Vec3 myPos = this->getAbsolutePosition();
    osg::Vec3 otherPos = otherLabel->getAbsolutePosition();
    return myPos-otherPos;
}

/**
 * get the distance between 2 labels from the screen point of view
 */
float lgLabel::distance2d(osg::ref_ptr<osgViewer::Viewer> view, osg::ref_ptr<lgLabel> otherLabel){
    //Matrix to change world coordinates in windows coordinates
    osg::Matrix modelView = view->getCamera()->getViewMatrix();
    osg::Matrix projection = view->getCamera()->getProjectionMatrix();
    
    osg::Matrix window = view->getCamera()->getViewport()->computeWindowMatrix();
    osg::Matrix MVPW = modelView * projection * window;
    osg::Vec3 absPos = (this->getAbsolutePosition()) * MVPW;
    osg::Vec3 oAbsPos = (otherLabel->getAbsolutePosition()) * MVPW;
    cout<<"abs Pos x :"<<absPos.x()<<", y: "<<absPos.y()<<", z: "<<absPos.z()<<endl;
    cout<<"other abs Pos x :"<<oAbsPos.x()<<", y: "<<oAbsPos.y()<<", z: "<<oAbsPos.z()<<endl;
    float distance = sqrt(pow(absPos.x()-oAbsPos.x(),2.0)+pow(absPos.y()-oAbsPos.y(),2.0)+pow(absPos.z()-oAbsPos.z(),2.0));
    return distance;
}

/*
 * get the shortest distance between two labels froms screen point of view 
 * using the bounding boxes of the labels
 */
float lgLabel::distance2dBox(osg::ref_ptr<osgViewer::Viewer> view, osg::ref_ptr<lgLabel> otherLabel){
    //Matrix to change world coordinates in windows coordinates
    osg::Matrix modelView = view->getCamera()->getViewMatrix();
    osg::Matrix projection = view->getCamera()->getProjectionMatrix();
    osg::Matrix window = view->getCamera()->getViewport()->computeWindowMatrix();
    osg::Matrix MVPW = modelView * projection * window;
    
    osg::BoundingBox myBound = this->computeBound();
    osg::BoundingBox otherBound = otherLabel->computeBound();
    
    osg::Vec3 oAbsPos = (otherLabel->getAbsolutePosition()) * MVPW;
    osg::Vec3 absPos = (this->getAbsolutePosition()) * MVPW;
    
    float deltaX = absPos.x()-oAbsPos.x();
    float deltaY = absPos.y()-oAbsPos.y();
    float deltaZ = absPos.z()-oAbsPos.z();
    
    
    float distance = sqrt(pow(absPos.x()-oAbsPos.x(),2.0)+pow(absPos.y()-oAbsPos.y(),2.0)+pow(absPos.z()-oAbsPos.z(),2.0));
    return distance;
}

