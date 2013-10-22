/* 
 * File:   lgLabel.h
 * Author: paulyves
 *
 * Created on October 19, 2013, 3:30 PM
 */

#ifndef LGLABEL_H
#define	LGLABEL_H

#include "lgNodeVisitor.h"
#include "lgNode.h"
#include <osgText/Text>

class lgLabel : public osgText::Text {
    public :

        lgLabel();
        lgLabel(const lgLabel& originalLabel);
        lgLabel(std::string text, osg::ref_ptr<lgNode> linkedNode);
        lgLabel(std::string filePath, std::string idNode);
        
        //getters and setters
        void setLinkNode(osg::ref_ptr<lgNode> aNode);
        osg::ref_ptr<lgNode> getLinkNode();
        void calcAbsolutePosition();
        osg::Vec3 getAbsolutePosition();
        osg::Vec3 getPostion();
        void setPostion(osg::Vec3 relativePosition);
        std::string getLabelType();
        void setLabelType(std::string labelType);
        bool getInternal();
        void setInternal (bool internal);
        
    protected :
        osg::ref_ptr<lgNode> linkNode;
        osg::Vec3 absolutePosition;
        int priority;
        std::string labelType;
        bool internal; //true if we have an internal label, to be developed
        osg::Vec3 position; //relative position to linkNode
};

#endif	/* LGLABEL_H */