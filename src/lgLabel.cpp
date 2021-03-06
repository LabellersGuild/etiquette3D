/** File: lgLabel.cpp
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
 * Created on October 19, 2013, 4:31 PM
 */

#include <osgViewer/Viewer>
#include <osg/MatrixTransform>
#include <osg/ShapeDrawable>
#include "../include/lgAnimation.h"
#include "../include/lgLabel.h"

using namespace std;
using namespace osg;

/** Default constructor
 */
lgLabel::lgLabel() : osgText::Text()
{
     hidingDistance = -1;
     labelType = EXTERNAL;
     internal = false;
     seeInTransparency = false;
     defaultDrawMode = osgText::Text::TEXT | osgText::Text::ALIGNMENT ;
     previousDrawMode = defaultDrawMode;
     changingWhenMouse = true;
}

/** Copy constructor
 * @param originalLabel : lgLabel : label that will be copied
 */
lgLabel::lgLabel(const lgLabel& originalLabel)
{
    linkNode = originalLabel.linkNode;
    absolutePosition = Vec3(originalLabel.absolutePosition);
    positionInit = Vec3(originalLabel.positionInit);
    labelType = originalLabel.labelType;
    updatedMatrix = originalLabel.updatedMatrix;
    hidingDistance = originalLabel.hidingDistance;
    seeInTransparency = originalLabel.seeInTransparency;
    defaultDrawMode = originalLabel.defaultDrawMode;
    previousDrawMode = originalLabel.previousDrawMode;
    changingWhenMouse = originalLabel.changingWhenMouse;
    infoLabel = originalLabel.infoLabel;
}

/** Constructor
 * @param text : string : text of the label
 * @param linkedNode : ref_ptr<Node> : node linked to the label
 * @param recoPos : Vec3 : recommended position
 */
lgLabel::lgLabel(string text, ref_ptr<Node> linkedNode, Vec3 recoPos)
{
    hidingDistance = -1;
     labelType = EXTERNAL;
     internal = false;
     seeInTransparency = false;
     defaultDrawMode = osgText::Text::TEXT | osgText::Text::ALIGNMENT ;
     previousDrawMode = defaultDrawMode;
     changingWhenMouse = true;

    setText(text);
    setLinkNode(linkedNode, recoPos);
    setPositionInit(recoPos);
}

/**
 * Set the param as linkNode attribute, then see if it got the
 * label in his children, if not it adds it (eventually creating a new
 * geode if the param is a group)
 * @param aNode : osg:ref_ptr<Node> : link node
 * @param recoPos : Vec3  : the recommended starting position of the label
 */
void lgLabel::setLinkNode(ref_ptr<Node> aNode, Vec3 recoPos)
{
    this->linkNode = aNode;
    this->setPositionInit(recoPos);
    this->setChangingWhenMouse(true);

    ref_ptr<Group> targetGroup = dynamic_cast<Group*>(linkNode.get());
    ref_ptr<Geode> targetGeode = dynamic_cast<Geode*>(linkNode.get());
    ref_ptr<MatrixTransform> mtLabel1 = new MatrixTransform(Matrixd::translate(recoPos));

    bool alreadyChild = false;
    //if we have a geode, we need to find its parent to then create a matrixTransform
    if(targetGeode&&!targetGroup)
    {
        for (unsigned i = 0; i < targetGeode->getNumDrawables(); i++)
        {
            ref_ptr<lgLabel> drawAsLabel = dynamic_cast<lgLabel*>(targetGeode->getDrawable(i));
            if(drawAsLabel && drawAsLabel.get()==this)
            {
                alreadyChild = true;
            }
        }
        targetGroup = targetGeode->getParent(0);
    }

    //when we got a group, we set the matrix transform as child and put inside a new Geode
    if(targetGroup)
    {
        targetGeode = new Geode();
        targetGroup->addChild(mtLabel1);
        mtLabel1->addChild(targetGeode);
        this->updatedMatrix=mtLabel1;
    }
    //adding the label as child of the geode
    if(targetGeode)
    {
        bool alreadyChild = false;
        for (unsigned i = 0; i < targetGeode->getNumDrawables(); i++)
        {
            ref_ptr<lgLabel> drawAsLabel = dynamic_cast<lgLabel*>(targetGeode->getDrawable(i));
            if(drawAsLabel && drawAsLabel.get()==this)
            {
                alreadyChild = true;
            }
        }
        if(!alreadyChild)
        {
            targetGeode->addDrawable(this);
        }
    }
}

/** Getter of linknode
 * @return ref_ptr<Node> linkNode
 */
ref_ptr<Node> lgLabel::getLinkNode() const
{
    return this->linkNode;
}

/** Calculate the absolute position thanks to the position of linknode
 */
void lgLabel::calcAbsolutePosition()
{
    if(this->getLinkNode() )
    {
        Vec4 extendedPosition = Vec4(this->getPosition(), 1);
        //getting the list of transposed transformation matrices, from node to root
        MatrixList matricesList = updatedMatrix->getWorldMatrices();
        for (unsigned i=0;i<matricesList.size();i++)
        {
            extendedPosition = extendedPosition*matricesList[i];
        }
        this->absolutePosition = Vec3(extendedPosition.x(),extendedPosition.y(),extendedPosition.z());
    }
}

/** Calculate the abosolute position and return it
 * @return Vec3 : absolute position
 */
Vec3 lgLabel::getAbsolutePosition()
{
    if(this->getLinkNode())
    {
        this->calcAbsolutePosition();
        return this->absolutePosition;
    }
    else
    {
        return Vec3(0,0,0);
    }
}

/**
 *Set a relative position for the label, not recommended
 * as it will be independant from the matrixTransform updatedMatrix
 * and therefore will have trouble with animations
 * @param relativePosition : Vec3 : position to set.
 */
void lgLabel::setPosition(Vec3 relativePosition)
{

    osgText::Text::setPosition(relativePosition);
    this->calcAbsolutePosition();
}

/** Getter for the labelType
 * @return lgType :
 */
lgType lgLabel::getLabelType() const
{
    return labelType;
}

/** *Set the initial position in the attribute positionInit
 * but also call the setPosition method with the same argument
 * @param newPositioinInit : Vec3 : new initial position
 */
void lgLabel::setPositionInit(Vec3 newPositionInit)
{
    positionInit = newPositionInit;
}

/** Getter of the positionInit
 * @return Vec3 : the inital position of the label
 */
Vec3 lgLabel::getPositionInit() const
{
    return positionInit;
}

/** Calculate and return the distance between two labels
 * @param otherLabel : ref_ptr<lgLabel> : other label
 * @return float : distance
 */
float lgLabel::distance(ref_ptr<lgLabel> otherLabel)
{
    Vec3 myPos = this->getAbsolutePosition();
    Vec3 otherPos = otherLabel->getAbsolutePosition();
    float distance = sqrt(pow(myPos.x()-otherPos.x(),2.0)+pow(myPos.y()-otherPos.y(),2.0)+pow(myPos.z()-otherPos.z(),2.0));
    return distance;
}

/** Calculate and return the vector formed with the position of two labels
 * @param otherLabel : ref_ptr<lgLabel> : other label
 * @return Vec3 : the vector
 */
Vec3 lgLabel::distanceVec(ref_ptr<lgLabel> otherLabel)
{
    Vec3 myPos = this->getAbsolutePosition();
    Vec3 otherPos = otherLabel->getAbsolutePosition();
    return myPos-otherPos;
}

/** Get the distance between 2 labels from the screen point of view
 * @param view : ref_ptr<osgViewer::Viewer> : viewer object of the main file
 * @param otherLabel : ref_ptr<lgLabel> : other label
 * @return float : the distance
 */
float lgLabel::distance2d(ref_ptr<osgViewer::Viewer> view, ref_ptr<lgLabel> otherLabel)
{
    //Matrix to change world coordinates in windows coordinates
    Matrix modelView = view->getCamera()->getViewMatrix();
    Matrix projection = view->getCamera()->getProjectionMatrix();

    Matrix window = view->getCamera()->getViewport()->computeWindowMatrix();
    Matrix MVPW = modelView * projection * window;
    Vec3 absPos = (this->getAbsolutePosition()) * MVPW;
    Vec3 oAbsPos = (otherLabel->getAbsolutePosition()) * MVPW;
    cout<<"abs Pos x :"<<absPos.x()<<", y: "<<absPos.y()<<", z: "<<absPos.z()<<endl;
    cout<<"other abs Pos x :"<<oAbsPos.x()<<", y: "<<oAbsPos.y()<<", z: "<<oAbsPos.z()<<endl;
    float distance = sqrt(pow(absPos.x()-oAbsPos.x(),2.0)+pow(absPos.y()-oAbsPos.y(),2.0)+pow(absPos.z()-oAbsPos.z(),2.0));
    return distance;
}

/** Get the shortest distance between two labels from screen point of view
 * using the bounding boxes of the labels
 * @param view : ref_ptr<osgViewer::Viewer> : viewer object of the main file
 * @param otherLabel : ref_ptr<lgLabel> : other label
 * @return float : the distance
 */
float lgLabel::distance2dBox(ref_ptr<osgViewer::Viewer> view, ref_ptr<lgLabel> otherLabel)
{
    //We first get the 2dBox of both labels
    Vec4 my2dBox = this->compute2dBox(view);
    Vec4 other2dBox = otherLabel->compute2dBox(view);
    //we deduce the box to the left
    Vec4 leftBox, rightBox;
    if(my2dBox.x()<other2dBox.x())
    {
        leftBox = my2dBox;
        rightBox = other2dBox;
    }
    else
    {
        leftBox = other2dBox;
        rightBox = my2dBox;
    }
    //we deduce the box to the top
    Vec4 topBox, botBox;
    if(my2dBox.y()<other2dBox.y())
    {
        topBox = other2dBox;
        botBox = my2dBox;
    }
    else
    {
        topBox = my2dBox;
        botBox = other2dBox;
    }
    //we then calculate the delta for both abs and ord
    float deltaX = rightBox.x()-leftBox.z();
    float deltaY = topBox.y() - botBox.w();
    float distance;
    //if box touch themselves, we set the distance as negative and a translation from
    // this distance as absolute value should fix the problem
    if(deltaX<0 && deltaY<0)
    {
        distance = -sqrt(pow(deltaX,2.0)+pow(deltaY,2.0));
    }
    if(deltaX<0 && deltaY>0)
    {
        distance = deltaY;
    }
    if(deltaX>0 && deltaY<0)
    {
        distance = deltaX;
    }
    if(deltaX>0 && deltaY>0)
    {
        distance = sqrt(pow(deltaX,2.0)+pow(deltaY,2.0));
    }
    return distance;
}

/** Gives the distance between the camera and the label
 * @param view : ref_ptr<osgViewer::Viewer> : the viewer object of the main file
 * @return float : distance between the camera and the label
 */
float lgLabel::distanceCamera(ref_ptr<osgViewer::Viewer> view) const
{
     Matrix matrixCamera = view->getCamera()->getInverseViewMatrix();
     Vec3 positionCamera= Vec3(matrixCamera(3,0), matrixCamera(3,1), matrixCamera(3,2));
     float distance = sqrt(pow(absolutePosition[0]-positionCamera[0],2.0)+pow(absolutePosition.y()-positionCamera.y(),2.0)+pow(absolutePosition[2]-positionCamera[2],2.0));
     return distance;
}

/** Getter of the hindingDistance
 * @return int : the hiding distance
 */
int lgLabel::getHidingDistance() const
{
    return hidingDistance;
}

/** Setter of the hidingDistance
 * @param hDistance : int : hiding distance
 */
void lgLabel::setHidingDistance(int hDistance)
{
    hidingDistance = hDistance;
}

/** Translate the label
 * @param x : int : y axis translation
 * @param y : int : y axis translation
 * @param z : int : z axis translation
 */
void lgLabel::translateLabel(int x, int y , int z)
{
    Matrix matrixTransform= updatedMatrix->getMatrix();
    updatedMatrix->setMatrix(matrixTransform * Matrix::translate(x,y,z));
}

/** Calculate the coordinates of the bounding box of the label on the screen
 * @param view : ref_ptr<osgViewer> : the viewer object of the main file
 * @return Vec4 : coordinates (xMin, yMin, xMax, yMax) of the bounding box.
 */
Vec4 lgLabel::compute2dBox(ref_ptr<osgViewer::Viewer> view)
{
    // Find the world coordinates of the node :
    Vec3 center = getAbsolutePosition();

    //Bounding box of the label :
    BoundingBox bbox = computeBound();

    //Matrix to change world coordinates in windows coordinates
    Matrix modelView = view->getCamera()->getViewMatrix();
    Matrix projection = view->getCamera()->getProjectionMatrix();
    Matrix window = view->getCamera()->getViewport()->computeWindowMatrix();
    Matrix MVPW = modelView * projection * window;

    //Projection of the bounding box of the label :
    Vec3 screenxyz, screenXyz, screenxYz, screenxyZ, screenXYz, screenXyZ, screenxYZ, screenXYZ;

    //If there is an info label :
    if (getInfoLabel() != NULL)
    {
        //And this label is not invisible
        if (this->getDrawMode() != 0)
        {
            BoundingBox bboxInfo = getInfoLabel()->getBound();
            int bboxXMin = (bbox.xMin() < bboxInfo.xMin() ? bbox.xMin() : bboxInfo.xMin());
            int bboxXMax = (bbox.xMax() > bboxInfo.xMax() ? bbox.xMax() : bboxInfo.xMax());
            int bboxYMin = (bbox.yMin() < bboxInfo.yMin() ? bbox.yMin() : bboxInfo.yMin());
            int bboxYMax = (bbox.yMax() > bboxInfo.yMax() ? bbox.yMax() : bboxInfo.yMax());
            int bboxZMin = (bbox.zMin() < bboxInfo.zMin() ? bbox.zMin() : bboxInfo.zMin());
            int bboxZMax = (bbox.zMax() > bboxInfo.zMax() ? bbox.zMax() : bboxInfo.zMax());

            //Projection of the 8 corners of the bounding box
            screenxyz = (center + Vec3(bboxXMin,bboxYMin,bboxZMin)) * MVPW;
            screenXyz = (center + Vec3(bboxXMax,bboxYMin,bboxZMin)) * MVPW;
            screenxYz = (center + Vec3(bboxXMin,bboxYMax,bboxZMin)) * MVPW;
            screenxyZ = (center + Vec3(bboxXMin,bboxYMin,bboxZMax)) * MVPW;
            screenXYz = (center + Vec3(bboxXMax,bboxYMax,bboxZMin)) * MVPW;
            screenXyZ = (center + Vec3(bboxXMax,bboxYMin,bboxZMax)) * MVPW;
            screenxYZ = (center + Vec3(bboxXMin,bboxYMax,bboxZMax)) * MVPW;
            screenXYZ = (center + Vec3(bboxXMax,bboxYMax,bboxZMax)) * MVPW;

        }
    }
    // If no info label :
    else
    {
         //Projection of the 8 corners of the bounding box of the label :
        screenxyz = (center + Vec3(bbox.xMin(),bbox.yMin(),bbox.zMin())) * MVPW;
        screenXyz = (center + Vec3(bbox.xMax(),bbox.yMin(),bbox.zMin())) * MVPW;
        screenxYz = (center + Vec3(bbox.xMin(),bbox.yMax(),bbox.zMin())) * MVPW;
        screenxyZ = (center + Vec3(bbox.xMin(),bbox.yMin(),bbox.zMax())) * MVPW;
        screenXYz = (center + Vec3(bbox.xMax(),bbox.yMax(),bbox.zMin())) * MVPW;
        screenXyZ = (center + Vec3(bbox.xMax(),bbox.yMin(),bbox.zMax())) * MVPW;
        screenxYZ = (center + Vec3(bbox.xMin(),bbox.yMax(),bbox.zMax())) * MVPW;
        screenXYZ = (center + Vec3(bbox.xMax(),bbox.yMax(),bbox.zMax())) * MVPW;
    }

    Vec3 pointA, pointB;
    pointA = screenxyz;
    pointB = screenXYZ;
    float deltaYmin = fabs(screenxyz.y() - screenXYZ.y());
    if (deltaYmin > fabs(screenXyz.y() - screenxYZ.y()))
    {
         deltaYmin = fabs(screenXyz.y() - screenxYZ.y());
         pointA = screenXyz;
         pointB = screenxYZ;
    }
    if (deltaYmin > fabs(screenxYz.y() - screenXyZ.y()))
    {
        deltaYmin = fabs(screenxYz.y() - screenXyZ.y());
        pointA = screenxYz;
        pointB = screenXyZ;
    }
    if (deltaYmin > fabs(screenxyZ.y() - screenXYZ.y()))
    {
        deltaYmin = fabs(screenxyZ.y() - screenXYz.y());
        pointA = screenxyZ;
        pointB = screenXYz;
    }

    float minx = pointA.x() < pointB.x() ? pointA.x() : pointB.x();
    float miny = pointA.y() < pointB.y() ? pointA.y() : pointB.y();
    float maxx = pointA.x() > pointB.x() ? pointA.x() : pointB.x();
    float maxy = pointA.y() > pointB.y() ? pointA.y() : pointB.y();

    Vec4 bounds = Vec4(minx, miny, maxx, maxy);

    return bounds;
}

/** Setter of the transparency of the label
 * @param alpha : float : value of the transparency. 0 is invisible, 1 is opaque
 */
void lgLabel::setTransparency(float alpha)
{
    Vec4 currentColor = getColor();
    setColor( Vec4(currentColor[0], currentColor[1], currentColor[2],alpha) );
    Vec4 currentColorBbox = getBoundingBoxColor();
    currentColorBbox[3]= alpha;
    setBoundingBoxColor(currentColorBbox);
}

/** Setter of the updatedMatrix's matrix
 * @param mat : const Matrix& : the matrix to use
 */
void lgLabel::setUpdatedMatrixMatrix (const Matrix &mat)
{
    updatedMatrix->setMatrix(mat);
}

/** Calculate the position of the center of the label on the screen
 * @param ref_ptr<osgViewer::Viewer> view : the viewer object of the main file
 * @return Vec2 : position (x,y) of the center
 */
Vec2 lgLabel::compute2dCenter(ref_ptr<osgViewer::Viewer> view)
{
     // Find the world coordinates of the node :
    Vec3 center = getAbsolutePosition();

    //Matrix to change world coordinates in windows coordinates
    Matrix modelView = view->getCamera()->getViewMatrix();
    Matrix projection = view->getCamera()->getProjectionMatrix();
    Matrix window = view->getCamera()->getViewport()->computeWindowMatrix();
    Matrix MVPW = modelView * projection * window;

    Vec3 center2d = center * MVPW;

   return Vec2(center2d.x(), center2d.y());
}

/** Setter to see the label in transparency in front of or behind the objects.
 * @param b : bool : true to set the transparency, false otherwise.
 */
void lgLabel::setSeeInTransparency(bool b)
{
    if (b)
    {
        setTransparency(0.7);

        // Disable depth testing so geometry is draw regardless of depth values of geometry already draw.
        ref_ptr<StateSet> stateSet = updatedMatrix->getOrCreateStateSet();
        stateSet->setMode(GL_DEPTH_TEST,StateAttribute::OFF);
        stateSet->setRenderingHint( StateSet::TRANSPARENT_BIN );

        // Make sure this geometry is draw last. RenderBins are handled in numerical order so set bin number to 11
        stateSet->setRenderBinDetails(11, "DepthSortedBin");
    }
    else
    {
        setTransparency(1);

        //Reset the stateset of the matrix
        updatedMatrix->setStateSet(new StateSet());
    }
}

/** Setter of the previousDrawMode
 * @param d : int : a draw mode
 */
void lgLabel::setPreviousDrawMode(int d)
{
    previousDrawMode = d;
}

/** Getter of the previousDrawMode
 * @return int : the previous draw mode
 */
int lgLabel::getPreviousDrawMode() const
{
    return previousDrawMode;
}

/** Setter of the defaultDrawMode
 * @param d : int : a draw mode
 */
void lgLabel::setDefaultDrawMode(int d)
{
    defaultDrawMode = d;
}

/** Getter of the defaultDrawMode
 * @return int : the defaultDrawMode
 */
int lgLabel::getDefaultDrawMode() const
{
    return defaultDrawMode;
}

/** To know if the label has to change when the mouse is on it
 * @return bool
 */
bool lgLabel::isChangingWhenMouse() const
{
   return changingWhenMouse;
}

/** Setter of changingWhenMouse
 * @param b : bool : true if the label changes when the mouse is on it, false otherwise.
 */
void lgLabel::setChangingWhenMouse(bool b)
{
    changingWhenMouse = b;
}

/** Return the bounding box of the object linked to the label. Recursive funcion.
 * @param node : ref_ptr<Group> node : For the first call of the function, it is the node parent of the matrixTransform of the label
 * @param bbox : BoundingBox : For the first call, put BoundingBox(0,0,0,0,0,0)
 */
BoundingBox lgLabel::computeObjectBBox(ref_ptr<Group> node, BoundingBox bbox) const
{
    //Look at the children
    for(unsigned i=0;i< node->getNumChildren();i++)
    {
        //If the child is a geode
        if (node->getChild(i)->isSameKindAs(new Geode()))
        {
            ref_ptr<Geode> child = dynamic_cast<Geode*>(node->getChild(i));

            //Look at the drawables that are not a lgLabel, and adapt the bbox
            for(unsigned j=0;j< child->getNumDrawables();j++)
            {
                if (! child->getDrawable(j)->isSameKindAs(new lgLabel()))
                {
                    BoundingBox childbbox = child->getDrawable(j)->getBound();

                    //First call of the function
                    if (bbox.xMax()==0 && bbox.xMin() == 0 && bbox.yMax()==0 && bbox.yMin()==0 && bbox.zMax()==0 && bbox.zMin() ==0)
                    {
                        //adapt the bbox
                        bbox.xMax() = childbbox.xMax();
                        bbox.yMax() = childbbox.yMax();
                        bbox.zMax() = childbbox.zMax();
                        bbox.xMin() = childbbox.xMin();
                        bbox.yMin() = childbbox.yMin();
                        bbox.zMin() = childbbox.zMin();
                    }
                    else
                    {
                        //adapt the bbox
                        if (bbox.xMax() < childbbox.xMax()) bbox.xMax() = childbbox.xMax();
                        if (bbox.yMax() < childbbox.yMax()) bbox.yMax() = childbbox.yMax();
                        if (bbox.zMax() < childbbox.zMax()) bbox.zMax() = childbbox.zMax();
                        if (bbox.xMin() > childbbox.xMin()) bbox.xMin() = childbbox.xMin();
                        if (bbox.yMin() > childbbox.yMin()) bbox.yMin() = childbbox.yMin();
                        if (bbox.zMin() > childbbox.zMin()) bbox.zMin() = childbbox.zMin();
                    }
                }
            }

        }
        //If the child is a group, call this function on it
        else if (node->getChild(i)->isSameKindAs(new Group()))
        {
            return computeObjectBBox(dynamic_cast<Group*>(node->getChild(i)), bbox);
        }
    }
    return bbox;
}

/** Calculate the coordinates of the bounding box of the object attached to the label on the screen
 * @param view : ref_ptr<osgViewer> : the viewer object of the main file
 * @return Vec4 : coordinates (xMin, yMin, xMax, yMax) of the bounding box.
 */
Vec4 lgLabel::computeObject2dBBox(ref_ptr<osgViewer::Viewer> view)
{
    //Bounding box of the object :
    BoundingBox bbox = computeObjectBBox(updatedMatrix->getParent(0),BoundingBox(0,0,0,0,0,0));

    // 8 Corners of the bbox :
    Vec4 xyz = Vec4(bbox.xMin(),bbox.yMin(),bbox.zMin(),1) ;
    Vec4 Xyz = Vec4(bbox.xMax(),bbox.yMin(),bbox.zMin(),1) ;
    Vec4 xYz = Vec4(bbox.xMin(),bbox.yMax(),bbox.zMin(),1) ;
    Vec4 xyZ = Vec4(bbox.xMin(),bbox.yMin(),bbox.zMax(),1) ;
    Vec4 XYz = Vec4(bbox.xMax(),bbox.yMax(),bbox.zMin(),1) ;
    Vec4 XyZ = Vec4(bbox.xMax(),bbox.yMin(),bbox.zMax(),1) ;
    Vec4 xYZ = Vec4(bbox.xMin(),bbox.yMax(),bbox.zMax(),1) ;
    Vec4 XYZ = Vec4(bbox.xMax(),bbox.yMax(),bbox.zMax(),1) ;

    // Absolute position of the 8 corners :

    MatrixList matricesList = updatedMatrix->getParent(0)->getWorldMatrices();
    for (unsigned i=0;i<matricesList.size();i++)
    {
        xyz = xyz*matricesList[i];
        Xyz = Xyz*matricesList[i];
        xYz = xYz*matricesList[i];
        xyZ = xyZ*matricesList[i];
        XYz = XYz*matricesList[i];
        XyZ = XyZ*matricesList[i];
        xYZ = xYZ*matricesList[i];
        XYZ = XYZ*matricesList[i];
    }

    //Matrix to change world coordinates in windows coordinates
    Matrix modelView = view->getCamera()->getViewMatrix();
    Matrix projection = view->getCamera()->getProjectionMatrix();
    Matrix window = view->getCamera()->getViewport()->computeWindowMatrix();
    Matrix MVPW = modelView * projection * window;

   //Projection of the 8 corners of the bounding box  :
    Vec3 screenxyz = (Vec3(xyz.x(),xyz.y(),xyz.z())) * MVPW;
    Vec3 screenXyz = (Vec3(Xyz.x(),Xyz.y(),Xyz.z())) * MVPW;
    Vec3 screenxYz = (Vec3(xYz.x(),xYz.y(),xYz.z())) * MVPW;
    Vec3 screenxyZ = (Vec3(xyZ.x(),xyZ.y(),xyZ.z())) * MVPW;
    Vec3 screenXYz = (Vec3(XYz.x(),XYz.y(),XYz.z())) * MVPW;
    Vec3 screenXyZ = (Vec3(XyZ.x(),XyZ.y(),XyZ.z())) * MVPW;
    Vec3 screenxYZ = (Vec3(xYZ.x(),xYZ.y(),xYZ.z())) * MVPW;
    Vec3 screenXYZ = (Vec3(XYZ.x(),XYZ.y(),XYZ.z())) * MVPW;

    //cout << screenxyz.x() << " " << screenxyz.y() << " " << screenxyz.z() << endl;
    //cout << screenXYZ.x() << " " << screenXYZ.y() << " " << screenXYZ.z() << endl;

    float minx = min(screenxyz.x(),
                     min(screenXyz.x(),
                         min(screenxYz.x(),
                             min(screenxyZ.x(),
                                 min(screenXYz.x(),
                                     min(screenXyZ.x(),
                                         min( screenxYZ.x(), screenXYZ.x())
                                         )
                                     )
                                 )
                             )
                         )
                     );
    float miny = min(screenxyz.y(),
                     min(screenXyz.y(),
                         min(screenxYz.y(),
                             min(screenxyZ.y(),
                                 min(screenXYz.y(),
                                     min(screenXyZ.y(),
                                         min( screenxYZ.y(), screenXYZ.y())
                                         )
                                     )
                                 )
                             )
                         )
                     );
    float maxx = max(screenxyz.x(),
                     max(screenXyz.x(),
                         max(screenxYz.x(),
                             max(screenxyZ.x(),
                                 max(screenXYz.x(),
                                     max(screenXyZ.x(),
                                         max( screenxYZ.x(), screenXYZ.x())
                                         )
                                     )
                                 )
                             )
                         )
                     );

    float maxy = max(screenxyz.y(),
                     max(screenXyz.y(),
                         max(screenxYz.y(),
                             max(screenxyZ.y(),
                                 max(screenXYz.y(),
                                     max(screenXyZ.y(),
                                         max( screenxYZ.y(), screenXYZ.y())
                                         )
                                     )
                                 )
                             )
                         )
                     );

    Vec4 bounds = Vec4(minx, miny, maxx, maxy);

    return bounds;
}
/** Set the labelType of the label (EXTERNAL, INTERNAL_TOP, INTERNAL_FACE)
 * @param type : lgType : the type of label
 */
void lgLabel::setLabelType(lgType type){
   this->labelType= type;

   if (type == EXTERNAL || type == SWITCH)
   {
       setAxisAlignment(osgText::Text::SCREEN);
       setAlignment(osgText::Text::CENTER_BOTTOM);

       //Put the label on the initial position. It is the recommended position by default.
       updatedMatrix->setMatrix(Matrixd::translate(getPositionInit()));
   }
   else if (type == INTERNAL_TOP)
   {
       setAxisAlignment(osgText::Text::XY_PLANE);
       setAlignment(osgText::Text::CENTER_CENTER);

       //Bounding box of the linked object
       BoundingBox bbox = computeObjectBBox(updatedMatrix->getParent(0),BoundingBox(0,0,0,0,0,0) );

       //Put the label on the object
       updatedMatrix->setMatrix(Matrixd::translate((bbox.xMax()+bbox.xMin())/2.0,(bbox.yMax()+bbox.yMin())/2, bbox.zMax()));
   }
   else if (type == INTERNAL_FACE)
   {
       setAxisAlignment(osgText::Text::XZ_PLANE);
       setAlignment(osgText::Text::CENTER_CENTER);

       //Bounding box of the linked object
       BoundingBox bbox = computeObjectBBox(updatedMatrix->getParent(0),BoundingBox(0,0,0,0,0,0) );
      cout <<"bouding box " << bbox.xMin() <<" " << bbox.xMax() << " "<< bbox.yMin()<<" "<<bbox.yMax()<<" "<<bbox.zMin()<<" "<<bbox.zMax()<< endl;

       //Put the label on one of the faces of the bounding box.
       updatedMatrix->setMatrix(Matrixd::translate((bbox.xMax()+bbox.xMin())/2.0,bbox.yMin(), (bbox.zMax()+bbox.zMin())/2));
   }
}

/** Create the infoLabel
 * @param text : String : the text on the label
 */
void lgLabel::setInfoLabel(string text)
{
    ref_ptr<Geode> geodeInfoLabel = new Geode;
    infoLabel = new osgText::Text;

    updatedMatrix->addChild(geodeInfoLabel.get());
    geodeInfoLabel->addDrawable(infoLabel.get());
    infoLabel->setText(text, osgText::String::ENCODING_UTF8 );
    infoLabel->setAxisAlignment(osgText::Text::SCREEN);
    infoLabel->setAlignment(osgText::Text::CENTER_TOP);
    infoLabel->setCharacterSize(getCharacterHeight()-1);
    infoLabel->setDrawMode(0); // Hidden by default
    geodeInfoLabel->setNodeMask( 0x1 );
}

/** infoLabel getter
 * @return osgText::Text* : the infoLabel
 */
osgText::Text* lgLabel::getInfoLabel() const
{
   return infoLabel;
}

/** updatedMatrix getter
 * @return updatedMatrix : MatrixTransform*
 */
MatrixTransform* lgLabel::getUpdatedMatrix() const
{
    return updatedMatrix;
}

/** adding an informative arrow under the label
 *  the arrow is a ShapeDrawable of a cylinder saved under the attribute infoArrow
 */
void lgLabel::addArrow()
{
    if(updatedMatrix!=NULL)
    {
        ref_ptr<Cylinder> theArrow = new Cylinder(Vec3(0,0,-25),0.2,50);
        ref_ptr<ShapeDrawable> arrowDrawable = new ShapeDrawable(theArrow);
        ref_ptr<Geode> emptyGeode = new Geode();
        ref_ptr<Geode> theGeode = new Geode();
        ref_ptr<Switch> theSwitch = new Switch(); // used to hide the arrow
        theGeode->addDrawable(arrowDrawable);
        theSwitch->insertChild(0,theGeode, true);
        theSwitch->insertChild(1,emptyGeode,false);
        theSwitch->setNewChildDefaultValue(true);
        updatedMatrix->addChild(theSwitch);
        arrowDrawable->setColor(Vec4(0,0,0,1));
        infoArrow = arrowDrawable;
        theSwitch->setNodeMask( 0x1 );
        arrowSwitcher = theSwitch;
    }
}

/** infoArrow getter, the info arrow is a ShapeDrawable of a cylinder
 * @return infoArrow : ref_ptr<ShapeDrawable>
 */
ref_ptr<ShapeDrawable> lgLabel::getArrow()
{
    return infoArrow;
}

/** Setter of the priority
 * @param priority : int : the priority
 */
 void lgLabel::setPriority(int priority)
 {
     this->priority = priority;
 }

/** Getter of the priority
 * @return int : the priority
 */
 int lgLabel::getPriority()
 {
     return priority;
 }

/**
*/
ref_ptr<osg::Switch> lgLabel::getArrowSwitcher()
{
  return arrowSwitcher;
}
