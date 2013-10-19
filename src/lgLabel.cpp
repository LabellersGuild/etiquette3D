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

osg::Vec3 lgLabel::getAbsolutePosition() {
    return this->absolutePosition;
}

osg::Vec3 lgLabel::getPostion() {
    return this->position;
}

void lgLabel::setPostion(osg::Vec3 relativePosition) {
    this->absolutePosition = relativePosition;
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
