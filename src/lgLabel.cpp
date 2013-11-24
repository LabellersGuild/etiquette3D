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
     hidingDistance = -1;
}

lgLabel::lgLabel(const lgLabel& originalLabel) {
     hidingDistance = -1;
}

lgLabel::lgLabel(std::string text, osg::ref_ptr<osg::Node> linkedNode, osgViewer::Viewer* viewer, osg::Vec3 recoPos) {
    this->setText(text);
    this->setLinkNode(linkedNode, viewer, recoPos);
    hidingDistance = -1;
}

lgLabel::lgLabel(std::string filePath, std::string idNode) {
     hidingDistance = -1;
}

//getters and setters
/**
 * set the param as linkNode attribute, then see if it got the
 * label in his children, if not it adds it (eventually creating a new
 * geode if the param is a group)
 * @param aNode, osg:ref_ptr<osg::Node> to the node
 */
void lgLabel::setLinkNode(osg::ref_ptr<osg::Node> aNode, osgViewer::Viewer* viewer, osg::Vec3 recoPos){
    this->linkNode = aNode;
    osg::ref_ptr<osg::Group> targetGroup = dynamic_cast<osg::Group*>(linkNode.get());
    osg::ref_ptr<osg::Geode> targetGeode = dynamic_cast<osg::Geode*>(linkNode.get());
    osg::ref_ptr<osg::MatrixTransform> mtLabel1 = new osg::MatrixTransform(osg::Matrixd::translate(recoPos));
    
    bool alreadyChild = false;
    //if we have a geode, we need to find its parent to then create a matrixTransform
    if(targetGeode&&!targetGroup){
        for (unsigned i = 0; i < targetGeode->getNumDrawables(); i++){
            osg::ref_ptr<lgLabel> drawAsLabel = dynamic_cast<lgLabel*>(targetGeode->getDrawable(i));
            if(drawAsLabel && drawAsLabel.get()==this){
                alreadyChild = true;
            }
        }
        targetGroup = targetGeode->getParent(0);
    }
    
    //when we got a group, we set the matrix transform as child and put inside a new Geode
    if(targetGroup){
        targetGeode = new osg::Geode();
        targetGroup->addChild(mtLabel1);
        mtLabel1->addChild(targetGeode);
        this->updatedMatrix=mtLabel1;
        mtLabel1->setUpdateCallback( new myLGAnimation(viewer));
    }
    //adding the label as child of the geode
    if(targetGeode){
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
        for (unsigned i=0;i<matricesList.size();i++){
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

/*
 *set a relative position for the label, not recommended
 * as it will be independant from the matrixTransform updatedMatrix
 * and therefore will have trouble with animations 
 */
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
 * get the shortest distance between two labels from screen point of view
 * using the bounding boxes of the labels
 */
float lgLabel::distance2dBox(osg::ref_ptr<osgViewer::Viewer> view, osg::ref_ptr<lgLabel> otherLabel){
    //We first get the 2dBox of both labels 
    osg::Vec4 my2dBox = this->compute2dBox(view);
    osg::Vec4 other2dBox = otherLabel->compute2dBox(view);
    //we deduce the box to the left
    osg::Vec4 leftBox, rightBox;
    if(my2dBox.x()<other2dBox.x()){
        leftBox = my2dBox;
        rightBox = other2dBox;
    } else {
        leftBox = other2dBox;
        rightBox = my2dBox;
    }
    //we deduce the box to the top
    osg::Vec4 topBox, botBox;
    if(my2dBox.y()<other2dBox.y()){
        topBox = other2dBox;
        botBox = my2dBox;
    } else {
        topBox = my2dBox;
        botBox = other2dBox;
    }
    //we then calculate the delta for both abs and ord
    float deltaX = rightBox.x()-leftBox.z();
    float deltaY = topBox.y() - botBox.w();
    float distance;
    //if box touch themselves, we set the distance as negative and a translation from
    // this distance as absolute value should fix the problem
    if(deltaX<0 && deltaY<0){
        distance = -sqrt(pow(deltaX,2.0)+pow(deltaY,2.0));
    }
    if(deltaX<0 && deltaY>0){
        distance = deltaY;
    }
    if(deltaX>0 && deltaY<0){
        distance = deltaX;
    }
    if(deltaX>0 && deltaY>0){
        distance = sqrt(pow(deltaX,2.0)+pow(deltaY,2.0));
    }
    return distance;
}

float lgLabel::distanceCamera(osg::ref_ptr<osgViewer::Viewer> view)
{
     osg::Matrix matrixCamera = view->getCamera()->getInverseViewMatrix();
     osg::Vec3 positionCamera= osg::Vec3(matrixCamera(3,0), matrixCamera(3,1), matrixCamera(3,2));
     float distance = sqrt(pow(absolutePosition[0]-positionCamera[0],2.0)+pow(absolutePosition[1]-positionCamera[1],2.0)+pow(absolutePosition[2]-positionCamera[2],2.0));
     return distance;
}

int lgLabel::getHidingDistance()
{
    return hidingDistance;
}

void lgLabel::setHidingDistance(int hDistance)
{
    hidingDistance = hDistance;
}

/**
* Function to move the label
* @param node, Node* : the matrix transformation node of the label
* @param x, int : X axis translation
* @param y, int : y axis translation
* @param z, int : z axis translation
*/
void lgLabel::translateLabel(int x, int y , int z)
{
    osg::Matrix matrixTransform= updatedMatrix->getMatrix();
    updatedMatrix->setMatrix(matrixTransform * osg::Matrix::translate(x,y,z));
}

/*
 *return the xmin, ymin, xmax, ymax screen coordinates of the bounding box 
 */
osg::Vec4 lgLabel::compute2dBox(osg::ref_ptr<osgViewer::Viewer> view)
{
     // Find the world coordinates of the node :
    osg::Vec3 center = getAbsolutePosition();

    //Bounding box of the label :
    osg::BoundingBox bbox = computeBound();

    //Matrix to change world coordinates in windows coordinates
    osg::Matrix modelView = view->getCamera()->getViewMatrix();
    osg::Matrix projection = view->getCamera()->getProjectionMatrix();
    osg::Matrix window = view->getCamera()->getViewport()->computeWindowMatrix();
    osg::Matrix MVPW = modelView * projection * window;

    //Projection of the bounding box of the label :
    osg::Vec3 screenxyz, screenXyz, screenxYz, screenxyZ, screenXYz, screenXyZ, screenxYZ, screenXYZ;

    //If info label :
    if (updatedMatrix->getNumChildren() > 1)
    {
        if (this->getDrawMode() != 0)
        {
            osg::BoundingBox bboxInfo = dynamic_cast<osg::Geode*>(updatedMatrix->getChild(1))->getDrawable(0)->getBound();
            int bboxXMin = (bbox.xMin() < bboxInfo.xMin() ? bbox.xMin() : bboxInfo.xMin());
            int bboxXMax = (bbox.xMax() > bboxInfo.xMax() ? bbox.xMax() : bboxInfo.xMax());
            int bboxYMin = (bbox.yMin() < bboxInfo.yMin() ? bbox.yMin() : bboxInfo.yMin());
            int bboxYMax = (bbox.yMax() > bboxInfo.yMax() ? bbox.yMax() : bboxInfo.yMax());
            int bboxZMin = (bbox.zMin() < bboxInfo.zMin() ? bbox.zMin() : bboxInfo.zMin());
            int bboxZMax = (bbox.zMax() > bboxInfo.zMax() ? bbox.zMax() : bboxInfo.zMax());

            screenxyz = (center + osg::Vec3(bboxXMin,bboxYMin,bboxZMin)) * MVPW;
            screenXyz = (center + osg::Vec3(bboxXMax,bboxYMin,bboxZMin)) * MVPW;
            screenxYz = (center + osg::Vec3(bboxXMin,bboxYMax,bboxZMin)) * MVPW;
            screenxyZ = (center + osg::Vec3(bboxXMin,bboxYMin,bboxZMax)) * MVPW;
            screenXYz = (center + osg::Vec3(bboxXMax,bboxYMax,bboxZMin)) * MVPW;
            screenXyZ = (center + osg::Vec3(bboxXMax,bboxYMin,bboxZMax)) * MVPW;
            screenxYZ = (center + osg::Vec3(bboxXMin,bboxYMax,bboxZMax)) * MVPW;
            screenXYZ = (center + osg::Vec3(bboxXMax,bboxYMax,bboxZMax)) * MVPW;
        }
    }
    // If no info label :
    else
    {
         //Projection of the bounding box of the label :
        screenxyz = (center + osg::Vec3(bbox.xMin(),bbox.yMin(),bbox.zMin())) * MVPW;
        screenXyz = (center + osg::Vec3(bbox.xMax(),bbox.yMin(),bbox.zMin())) * MVPW;
        screenxYz = (center + osg::Vec3(bbox.xMin(),bbox.yMax(),bbox.zMin())) * MVPW;
        screenxyZ = (center + osg::Vec3(bbox.xMin(),bbox.yMin(),bbox.zMax())) * MVPW;
        screenXYz = (center + osg::Vec3(bbox.xMax(),bbox.yMax(),bbox.zMin())) * MVPW;
        screenXyZ = (center + osg::Vec3(bbox.xMax(),bbox.yMin(),bbox.zMax())) * MVPW;
        screenxYZ = (center + osg::Vec3(bbox.xMin(),bbox.yMax(),bbox.zMax())) * MVPW;
        screenXYZ = (center + osg::Vec3(bbox.xMax(),bbox.yMax(),bbox.zMax())) * MVPW;
    }

     // The bounds of the polytope are determined by the two projected points the most on the left and on the right on the screen.

    osg::Vec3 mostLeft = screenxyz[0] < screenXyz[0] ? screenxyz : screenXyz;
    mostLeft = mostLeft[0] < screenxYz[0] ? mostLeft : screenxYz;
    mostLeft = mostLeft[0] < screenxyZ[0] ? mostLeft : screenxyZ;
    mostLeft = mostLeft[0] < screenXYz[0] ? mostLeft : screenXYz;
    mostLeft = mostLeft[0] < screenXyZ[0] ? mostLeft : screenXyZ;
    mostLeft = mostLeft[0] < screenxYZ[0] ? mostLeft : screenxYZ;
    mostLeft = mostLeft[0] < screenXYZ[0] ? mostLeft : screenXYZ;

    osg::Vec3 mostRight = screenxyz[0] > screenXyz[0] ? screenxyz : screenXyz;
    mostRight = mostRight[0] > screenxYz[0] ? mostRight : screenxYz;
    mostRight = mostRight[0] > screenxyZ[0] ? mostRight : screenxyZ;
    mostRight = mostRight[0] > screenXYz[0] ? mostRight : screenXYz;
    mostRight = mostRight[0] > screenXyZ[0] ? mostRight : screenXyZ;
    mostRight = mostRight[0] > screenxYZ[0] ? mostRight : screenxYZ;
    mostRight = mostRight[0] > screenXYZ[0] ? mostRight : screenXYZ;

    osg::Vec4 bounds = osg::Vec4(mostLeft[0], mostLeft[1] < mostRight[1] ? mostLeft[1] : mostRight[1], mostRight[0], mostLeft[1] > mostRight[1] ? mostLeft[1] : mostRight[1]);
    return bounds;
}
