/**
 * File:   lgLabel.cpp
 * Authors: Paul-Yves, Thomas
 *
 * Created on October 19, 2013, 3:30 PM
 */

#ifndef LGLABEL_H
#define	LGLABEL_H

#include "lgNodeVisitor.h"
#include <osgText/Text>
#include <osgViewer/Viewer>
#include "LGAnimation.h"

//Types of label
enum lgType{
    INTERNAL_TOP,
    INTERNAL_FACE,
    EXTERNAL
};


class lgLabel : public osgText::Text {
    public :

        lgLabel();

        lgLabel(const lgLabel& originalLabel);

        lgLabel(std::string text, osg::ref_ptr<osg::Node> linkedNode, osg::Vec3 recoPos);

        lgLabel(std::string filePath, std::string idNode);

        //getters and setters
        void setLinkNode(osg::ref_ptr<osg::Node> aNode, osg::Vec3 recoPos);

        osg::ref_ptr<osg::Node> getLinkNode()const;

        void calcAbsolutePosition() ;

        osg::Vec3 getAbsolutePosition();

        void setPosition(osg::Vec3 relativePosition);

        /** Getter for the labelType
         * @return lgType :
         */
        lgType getLabelType() const;

        /** Set the labelType of the label (EXTERNAL, INTERNAL_TOP, INTERNAL_FACE)
         * @param type : lgType : the type of label
         * @param animation : ref_ptr<LGAnimation> : the animation related to the label
         */
        void setLabelType(lgType labelType, osg::ref_ptr<LGAnimation> animation);

        /** Getter for the internal attribute
         * @return bool : internal
         */
        bool getInternal() const;

        /** Setter for the internal attribute
         * @param internal : bool
         */
        void setInternal (bool internal);

        void setPositionInit(osg::Vec3 newPositionInit);

        /** Getter of the positionInit
         * @return Vec3 : the inital position of the label
         */
        osg::Vec3 getPositionInit() const;

        float distance(osg::ref_ptr<lgLabel> otherLabel) ;

        osg::Vec3 distanceVec(osg::ref_ptr<lgLabel> otherLabel);

        float distance2d(osg::ref_ptr<osgViewer::Viewer> view, osg::ref_ptr<lgLabel> otherLabel) ;

        float distance2dBox(osg::ref_ptr<osgViewer::Viewer> view, osg::ref_ptr<lgLabel> otherLabel);

        /** Gives the distance between the camera and the label
         * @param view : ref_ptr<osgViewer::Viewer> : the viewer object of the main file
         * @return float : distance between the camera and the label
         */
        float distanceCamera(osg::ref_ptr<osgViewer::Viewer> view) const;

        /** Getter of the hindingDistance
         * @return int : the hiding distance
         */
        int getHidingDistance() const;

        /** Setter of the hidingDistance
         * @param hDistance : int : hiding distance
         */
        void setHidingDistance(int hDistance);

        /** Calculate the coordinates of the bounding box of the label on the screen
         * @param view : ref_ptr<osgViewer> : the viewer object of the main file
         * @return Vec4 : coordinates (xMin, yMin, xMax, yMax) of the bounding box.
         */
        osg::Vec4 compute2dBox(osg::ref_ptr<osgViewer::Viewer> view);

        /** Calculate the position of the center of the label on the screen
         * @param ref_ptr<osgViewer::Viewer> view : the viewer object of the main file
         * @return Vec2 : position (x,y) of the center
         */
        osg::Vec2 compute2dCenter(osg::ref_ptr<osgViewer::Viewer> view) ;

        /** Translate the label
         * @param x : int : y axis translation
         * @param y : int : y axis translation
         * @param z : int : z axis translation
         */
        void translateLabel(int x, int y , int z);

        /** Setter of the transparency of the label
         * @param alpha : float : value of the transparency. 0 is invisible, 1 is opaque
         */
        void setTransparency(float alpha);

        /** Setter of the updatedMatrix's matrix
         * @param mat : const Matrix& : the matrix to use
         */
        void setUpdatedMatrixMatrix (const osg::Matrix &mat);

        /** updatedMatrix getter
         * @return updatedMatrix : MatrixTransform*
         */
        osg::MatrixTransform* getUpdatedMatrix() const;

        /** Setter to see the label in transparency in front of or behind the objects.
         * @param b : bool : true to set the transparency, false otherwise.
         */
        void setSeeInTransparency(bool b);

        /** Setter of the defaultDrawMode
         * @param d : int : a draw mode
         */
        void setDefaultDrawMode(int d);

        /** Getter of the defaultDrawMode
         * @return int : the defaultDrawMode
         */
        int getDefaultDrawMode() const;

        /** Setter of the previousDrawMode
         * @param d : int : a draw mode
         */
        void setPreviousDrawMode(int d);

        /** Getter of the previousDrawMode
         * @return int : the previous draw mode
         */
        int getPreviousDrawMode() const;

        /** To know if the label has to change when the mouse is on it
         * @return bool
         */
        bool isChangingWhenMouse() const;

        /** Setter of changingWhenMouse
         * @param b : bool : true if the label changes when the mouse is on it, false otherwise.
         */
        void setChangingWhenMouse(bool b);

        /** Return the bounding box of the object linked to the label. Recursive funcion.
         * @param node : ref_ptr<Group> node : For the first call of the function, it is the node parent of the matrixTransform of the label
         * @param bbox : BoundingBox : For the first call, put BoundingBox(0,0,0,0,0,0)
         */
        osg::BoundingBox computeObjectBBox(osg::ref_ptr<osg::Group> node, osg::BoundingBox bbox) const;

        /** infoLabel getter
         * @return osgText::Text* : the infoLabel
         */
        osgText::Text* getInfoLabel() const;

        /** Create the infoLabel
         * @param text : String : the text on the label
         */
        void setInfoLabel(std::string text);

    protected :
        //the node who is supposed to contain the label, if it is a group
        //it has a geode child containing the label
        osg::ref_ptr<osg::Node> linkNode;

        osg::Vec3 absolutePosition;

        //this position is the recommended starting position of the label
        //at the initialization, position should be positionInit
        osg::Vec3 positionInit;

        int priority;

        /** Type of the label : EXTERNAL, INTERNAL_TOP or INTERNAL_FACe
         */
        lgType labelType;

        /** True if we have an internal label, to be developed
         */
        bool internal;

        /** Matrix transform of the label
         */
        osg::ref_ptr<osg::MatrixTransform> updatedMatrix;

        /** If the label is further than this distance, it is hidden.
         * If hidingDistance = -1, the label is never hidden.
         */
        int hidingDistance;

        /** To see the label in transparency when it is behind (and also in front of) objects
         */
        bool seeInTransparency;

        /** When the label is hidden or is selected, its drawMode is changed. To go back to the default drawMode, we record it.
         */
        int defaultDrawMode;

        /** The previous drawMode is needed too. For LGInteraction, the hiding option.
         */
        int previousDrawMode;

        /** To increase of the size of the label when the mouse is on it, if the value is true
         */
         bool changingWhenMouse;

         /** Information label
          */
         osg::ref_ptr<osgText::Text> infoLabel;
};

#endif	/* LGLABEL_H */
