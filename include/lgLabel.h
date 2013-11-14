/* 
 * File:   lgLabel.h
 * Author: paulyves
 *
 * Created on October 19, 2013, 3:30 PM
 */

#ifndef LGLABEL_H
#define	LGLABEL_H

#include "lgNodeVisitor.h"
#include <osgText/Text>
#include <osgViewer/Viewer>

class lgLabel : public osgText::Text {
    public :

        lgLabel();
        lgLabel(const lgLabel& originalLabel);
        lgLabel(std::string text, osg::ref_ptr<osg::Node> linkedNode, osgViewer::Viewer* viewer);
        lgLabel(std::string filePath, std::string idNode);
        
        //getters and setters
        void setLinkNode(osg::ref_ptr<osg::Node> aNode, osgViewer::Viewer* viewer);
        osg::ref_ptr<osg::Node> getLinkNode();
        void calcAbsolutePosition();
        osg::Vec3 getAbsolutePosition();
        void setPosition(osg::Vec3 relativePosition);
        std::string getLabelType();
        void setLabelType(std::string labelType);
        bool getInternal();
        void setInternal (bool internal);
        void setPositionInit(osg::Vec3 newPositionInit);
        osg::Vec3 getPositionInit();
        
    protected :
        //the node who is supposed to contain the label, if it is a group
        //it has a geode child containing the label
        osg::ref_ptr<osg::Node> linkNode;
        osg::Vec3 absolutePosition;
        //this position is the recommended starting position of the label
        //at the initialization, position should be positionInit
        osg::Vec3 positionInit;
        int priority;
        std::string labelType;
        bool internal; //true if we have an internal label, to be developed
};

#endif	/* LGLABEL_H */