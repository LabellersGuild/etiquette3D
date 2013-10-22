/* 
 * File:   lgLabel.cpp
 * Author: paulyves
 * 
 * Created on October 19, 2013, 4:31 PM
 */

#include "../include/lgLabel.h"


//constructors
lgLabel::lgLabel() : osgText::Text(){
}

lgLabel::lgLabel(const lgLabel& originalLabel) {
}

lgLabel::lgLabel(std::string text, osg::ref_ptr<lgNode> linkedNode) {
    
}

lgLabel::lgLabel(std::string filePath, std::string idNode) {
    
}

//getters and setters
void lgLabel::setLinkNode(osg::ref_ptr<lgNode> aNode){
    this->linkNode = aNode;
}

osg::ref_ptr<lgNode> lgLabel::getLinkNode() {
    return this->linkNode;
}

//calculate the absolute position thanks to the position of linknode
void lgLabel::calcAbsolutePosition() {
    if(this->getLinkNode() ){
        osg::Vec4 extendedPosition = osg::Vec4(this->getPosition(), 1);
        //getting the list of transposed transformation matrices, from node to root
        osg::MatrixList matricesList = this->getLinkNode()->getWorldMatrices();
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
