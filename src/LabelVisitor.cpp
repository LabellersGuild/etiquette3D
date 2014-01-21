/* 
 * File:   LabelVisitor.cpp
 * Author: paulyves
 * 
 * Created on January 14, 2014, 11:40 AM
 */

#include "../include/LabelVisitor.h"

LabelVisitor::LabelVisitor() : NodeVisitor(TRAVERSE_ALL_CHILDREN){
}

LabelVisitor::LabelVisitor(const LabelVisitor& orig) : NodeVisitor(TRAVERSE_ALL_CHILDREN){
}

LabelVisitor::~LabelVisitor() {
}

void LabelVisitor::apply(osg::Geode& searchNode){
    if(searchNode.getNumDrawables()>0){
        for (unsigned i=0;i<searchNode.getNumDrawables();i++){
            ref_ptr<lgLabel> aLabel = dynamic_cast<lgLabel*>(searchNode.getDrawable(i));
            if(aLabel){
                labelList.push_back(aLabel);
            }
        }
    }
    traverse(searchNode);
}

vector<ref_ptr<lgLabel> > LabelVisitor::getLabelList(){
    return labelList;
}