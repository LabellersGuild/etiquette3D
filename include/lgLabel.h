/** File: lgLabel.h
 * Description :
 * This class represents a label.
 * A label has a lgType that corresponds to the type of label : internal on the top of the object, internal on a face, or external.
 * In the graph scene of OpenSceneGraph, the labels are linked to the nodes they describe, thus there is a pointer to this node : linkNode.
 * lgLabel has also a pointer to a an intermediary node used for the placement of the label during animations : updatedMatrix.
 * lgLabes has several methods that calculate specific distances between two labels, in the scene or on the screen.
 * lgLabel is a sub-class of osgText::Text, used to represent text in the 3D environment of OSG.
 *
 * Authors: Paul-Yves, Thomas
 *
 * Created on October 19, 2013, 3:30 PM
 */

#ifndef LGLABEL_H
#define	LGLABEL_H

#include "lgNodeVisitor.h"
#include <osgText/Text>
#include <osg/ShapeDrawable>
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

        /** Default constructor
         */
        lgLabel();

        /** Copy constructor
         * @param originalLabel : lgLabel : label that will be copied
         */
        lgLabel(const lgLabel& originalLabel);

        /** Constructor
         * @param text : string : text of the label
         * @param linkedNode : ref_ptr<Node> : node linked to the label
         * @param recoPos : Vec3 : recommended position
         */
        lgLabel(std::string text, osg::ref_ptr<osg::Node> linkedNode, osg::Vec3 recoPos);

        /** Constructor
         * @param filePath : string
         * @param idNode : string
         */
        lgLabel(std::string filePath, std::string idNode);

        /**
         * Set the param as linkNode attribute, then see if it got the
         * label in his children, if not it adds it (eventually creating a new
         * geode if the param is a group)
         * @param aNode : osg:ref_ptr<Node> : link node
         * @param recoPos : Vec3  : the recommended starting position of the label
         */
        void setLinkNode(osg::ref_ptr<osg::Node> aNode, osg::Vec3 recoPos);

        /** Getter of linknode
         * @return ref_ptr<Node> linkNode
         */
        osg::ref_ptr<osg::Node> getLinkNode()const;

        /** Calculate the absolute position thanks to the position of linknode
         */
        void calcAbsolutePosition() ;

        /** Calculate the abosolute position and return it
         * @return Vec3 : absolute position
         */
        osg::Vec3 getAbsolutePosition();

        /**
         *Set a relative position for the label, not recommended
         * as it will be independant from the matrixTransform updatedMatrix
         * and therefore will have trouble with animations
         * @param relativePosition : Vec3 : position to set.
         */
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

        /** *Set the initial position in the attribute positionInit
         * but also call the setPosition method with the same argument
         * @param newPositioinInit : Vec3 : new initial position
         */
        void setPositionInit(osg::Vec3 newPositionInit);

        /** Getter of the positionInit
         * @return Vec3 : the inital position of the label
         */
        osg::Vec3 getPositionInit() const;

        /** Calculate and return the distance between two labels
         * @param otherLabel : ref_ptr<lgLabel> : other label
         * @return float : distance
         */
        float distance(osg::ref_ptr<lgLabel> otherLabel) ;

        /** Calculate and return the vector formed with the position of two labels
         * @param otherLabel : ref_ptr<lgLabel> : other label
         * @return Vec3 : the vector
         */
        osg::Vec3 distanceVec(osg::ref_ptr<lgLabel> otherLabel);

        /** Get the distance between 2 labels from the screen point of view
         * @param view : ref_ptr<osgViewer::Viewer> : viewer object of the main file
         * @param otherLabel : ref_ptr<lgLabel> : other label
         * @return float : the distance
         */
        float distance2d(osg::ref_ptr<osgViewer::Viewer> view, osg::ref_ptr<lgLabel> otherLabel) ;

        /** Get the shortest distance between two labels from screen point of view
         * using the bounding boxes of the labels
         * @param view : ref_ptr<osgViewer::Viewer> : viewer object of the main file
         * @param otherLabel : ref_ptr<lgLabel> : other label
         * @return float : the distance
         */
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

        void addArrow();
        osg::ref_ptr<osg::ShapeDrawable> getArrow();

    protected :
        /**The node which is supposed to contain the label, if it is a group
         * it has a geode child containing the label
         */
        osg::ref_ptr<osg::Node> linkNode;

        /** Absolute position of the label.
         * It is calculated every time its corresponding getter is used
         */
        osg::Vec3 absolutePosition;

        /**This position is the recommended starting position of the label
         * At the initialization, position should be positionInit
         */
        osg::Vec3 positionInit;

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
         osg::ref_ptr<osg::ShapeDrawable> infoArrow;
};

#endif	/* LGLABEL_H */
