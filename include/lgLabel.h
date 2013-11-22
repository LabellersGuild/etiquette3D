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
        lgLabel(std::string text, osg::ref_ptr<osg::Node> linkedNode, osgViewer::Viewer* viewer, osg::Vec3 recoPos);
        lgLabel(std::string filePath, std::string idNode);

        //getters and setters
        void setLinkNode(osg::ref_ptr<osg::Node> aNode, osgViewer::Viewer* viewer, osg::Vec3 recoPos);
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
        float distance(osg::ref_ptr<lgLabel> otherLabel);
        osg::Vec3 distanceVec(osg::ref_ptr<lgLabel> otherLabel);
        float distance2d(osg::ref_ptr<osgViewer::Viewer> view, osg::ref_ptr<lgLabel> otherLabel);
        float distance2dBox(osg::ref_ptr<osgViewer::Viewer> view, osg::ref_ptr<lgLabel> otherLabel);
        float distanceCamera(osg::ref_ptr<osgViewer::Viewer> view);
        int getHidingDistance();
        void setHidingDistance(int hDistance);
        osg::Vec4 compute2dBox(osg::ref_ptr<osgViewer::Viewer> view);

        /**
        * Function to move the label
        * @param node, Node* : the matrix transformation node of the label
        * @param x, int : X axis translation
        * @param y, int : y axis translation
        * @param z, int : z axis translation
        */
        void translateLabel(int x, int y , int z);

        void setTransparency(float alpha);

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
        osg::ref_ptr<osg::MatrixTransform> updatedMatrix;
        int hidingDistance;
};

#endif	/* LGLABEL_H */
