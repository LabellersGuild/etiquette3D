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
using namespace osg;

//constructors
lgLabel::lgLabel() : osgText::Text(){
     hidingDistance = -1;
}

lgLabel::lgLabel(const lgLabel& originalLabel) {
     hidingDistance = -1;
}

lgLabel::lgLabel(std::string text, ref_ptr<Node> linkedNode, Vec3 recoPos, ref_ptr<LGAnimation> animation) {
    this->setText(text);
    this->setLinkNode(linkedNode, recoPos, animation);
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
 * @param aNode, osg:ref_ptr<Node> to the node
 */
void lgLabel::setLinkNode(ref_ptr<Node> aNode, Vec3 recoPos, ref_ptr<LGAnimation> myLGAnimation){
    this->linkNode = aNode;
    ref_ptr<Group> targetGroup = dynamic_cast<Group*>(linkNode.get());
    ref_ptr<Geode> targetGeode = dynamic_cast<Geode*>(linkNode.get());
    ref_ptr<MatrixTransform> mtLabel1 = new MatrixTransform(Matrixd::translate(recoPos));
    this->setPositionInit(recoPos);
    if(!targetGeode && targetGroup){
        targetGeode = new Geode();
        targetGroup->addChild(mtLabel1);
        mtLabel1->addChild(targetGeode);
        this->updatedMatrix=mtLabel1;
        mtLabel1->setUpdateCallback(myLGAnimation);
    }
    if(targetGeode){
        //todo gérer le cas où on a direct une géode
        bool alreadyChild = false;
        for (unsigned i = 0; i < targetGeode->getNumDrawables(); i++){
            ref_ptr<lgLabel> drawAsLabel = dynamic_cast<lgLabel*>(targetGeode->getDrawable(i));
            if(drawAsLabel && drawAsLabel.get()==this){
                alreadyChild = true;
            }
        }
        if(!alreadyChild){
            targetGeode->addDrawable(this);
        }
    }
}


ref_ptr<Node> lgLabel::getLinkNode() {
    return this->linkNode;
}

//calculate the absolute position thanks to the position of linknode
void lgLabel::calcAbsolutePosition() {
    if(this->getLinkNode() ){
        Vec4 extendedPosition = Vec4(this->getPosition(), 1);
        //getting the list of transposed transformation matrices, from node to root
        MatrixList matricesList = updatedMatrix->getWorldMatrices();
        for (unsigned i=0;i<matricesList.size();i++){
            extendedPosition = extendedPosition*matricesList[i];
        }
        this->absolutePosition = Vec3(extendedPosition.x(),extendedPosition.y(),extendedPosition.z());
    }
}

Vec3 lgLabel::getAbsolutePosition() {
    if(this->getLinkNode()){
        this->calcAbsolutePosition();
        return this->absolutePosition;
    } else {
        return Vec3(0,0,0);
    }
}


void lgLabel::setPosition(Vec3 relativePosition) {
    osgText::Text::setPosition(relativePosition);
    this->calcAbsolutePosition();
}

lgType lgLabel::getLabelType(){
    return this->labelType;
}

void lgLabel::setLabelType(lgType labelType){
    this->labelType=labelType;

    if (labelType == external)
    {

    }
    else if (labelType == internal_top)
    {

    }
    else
    {

    }
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
void lgLabel::setPositionInit(Vec3 newPositionInit){
    positionInit = newPositionInit;
    //this->setPosition(positionInit);
}

Vec3 lgLabel::getPositionInit(){
    return positionInit;
}


float lgLabel::distance(ref_ptr<lgLabel> otherLabel){
    Vec3 myPos = this->getAbsolutePosition();
    Vec3 otherPos = otherLabel->getAbsolutePosition();
    float distance = sqrt(pow(myPos.x()-otherPos.x(),2.0)+pow(myPos.y()-otherPos.y(),2.0)+pow(myPos.z()-otherPos.z(),2.0));
    return distance;
}

Vec3 lgLabel::distanceVec(ref_ptr<lgLabel> otherLabel){
    Vec3 myPos = this->getAbsolutePosition();
    Vec3 otherPos = otherLabel->getAbsolutePosition();
    return myPos-otherPos;
}

/**
 * get the distance between 2 labels from the screen point of view
 */
float lgLabel::distance2d(ref_ptr<osgViewer::Viewer> view, ref_ptr<lgLabel> otherLabel){
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

/*
 * get the shortest distance between two labels froms screen point of view
 * using the bounding boxes of the labels
 */
float lgLabel::distance2dBox(ref_ptr<osgViewer::Viewer> view, ref_ptr<lgLabel> otherLabel){
    //Matrix to change world coordinates in windows coordinates
    Matrix modelView = view->getCamera()->getViewMatrix();
    Matrix projection = view->getCamera()->getProjectionMatrix();
    Matrix window = view->getCamera()->getViewport()->computeWindowMatrix();
    Matrix MVPW = modelView * projection * window;

    BoundingBox myBound = this->computeBound();
    BoundingBox otherBound = otherLabel->computeBound();

    Vec3 oAbsPos = (otherLabel->getAbsolutePosition()) * MVPW;
    Vec3 absPos = (this->getAbsolutePosition()) * MVPW;

    float deltaX = absPos.x()-oAbsPos.x();
    float deltaY = absPos.y()-oAbsPos.y();
    float deltaZ = absPos.z()-oAbsPos.z();


    float distance = sqrt(pow(absPos.x()-oAbsPos.x(),2.0)+pow(absPos.y()-oAbsPos.y(),2.0)+pow(absPos.z()-oAbsPos.z(),2.0));
    return distance;
}

float lgLabel::distanceCamera(ref_ptr<osgViewer::Viewer> view)
{
     Matrix matrixCamera = view->getCamera()->getInverseViewMatrix();
     Vec3 positionCamera= Vec3(matrixCamera(3,0), matrixCamera(3,1), matrixCamera(3,2));
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
    Matrix matrixTransform= updatedMatrix->getMatrix();
    updatedMatrix->setMatrix(matrixTransform * Matrix::translate(x,y,z));
}

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

    //If info label :
    if (updatedMatrix->getNumChildren() > 1)
    {
        if (this->getDrawMode() != 0)
        {
            BoundingBox bboxInfo = dynamic_cast<Geode*>(updatedMatrix->getChild(1))->getDrawable(0)->getBound();
            int bboxXMin = (bbox.xMin() < bboxInfo.xMin() ? bbox.xMin() : bboxInfo.xMin());
            int bboxXMax = (bbox.xMax() > bboxInfo.xMax() ? bbox.xMax() : bboxInfo.xMax());
            int bboxYMin = (bbox.yMin() < bboxInfo.yMin() ? bbox.yMin() : bboxInfo.yMin());
            int bboxYMax = (bbox.yMax() > bboxInfo.yMax() ? bbox.yMax() : bboxInfo.yMax());
            int bboxZMin = (bbox.zMin() < bboxInfo.zMin() ? bbox.zMin() : bboxInfo.zMin());
            int bboxZMax = (bbox.zMax() > bboxInfo.zMax() ? bbox.zMax() : bboxInfo.zMax());

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
         //Projection of the bounding box of the label :
        screenxyz = (center + Vec3(bbox.xMin(),bbox.yMin(),bbox.zMin())) * MVPW;
        screenXyz = (center + Vec3(bbox.xMax(),bbox.yMin(),bbox.zMin())) * MVPW;
        screenxYz = (center + Vec3(bbox.xMin(),bbox.yMax(),bbox.zMin())) * MVPW;
        screenxyZ = (center + Vec3(bbox.xMin(),bbox.yMin(),bbox.zMax())) * MVPW;
        screenXYz = (center + Vec3(bbox.xMax(),bbox.yMax(),bbox.zMin())) * MVPW;
        screenXyZ = (center + Vec3(bbox.xMax(),bbox.yMin(),bbox.zMax())) * MVPW;
        screenxYZ = (center + Vec3(bbox.xMin(),bbox.yMax(),bbox.zMax())) * MVPW;
        screenXYZ = (center + Vec3(bbox.xMax(),bbox.yMax(),bbox.zMax())) * MVPW;
    }

     // The bounds of the polytope are determined by the two projected points the most on the left and on the right on the screen.

    Vec3 mostLeft = screenxyz[0] < screenXyz[0] ? screenxyz : screenXyz;
    mostLeft = mostLeft[0] < screenxYz[0] ? mostLeft : screenxYz;
    mostLeft = mostLeft[0] < screenxyZ[0] ? mostLeft : screenxyZ;
    mostLeft = mostLeft[0] < screenXYz[0] ? mostLeft : screenXYz;
    mostLeft = mostLeft[0] < screenXyZ[0] ? mostLeft : screenXyZ;
    mostLeft = mostLeft[0] < screenxYZ[0] ? mostLeft : screenxYZ;
    mostLeft = mostLeft[0] < screenXYZ[0] ? mostLeft : screenXYZ;

    Vec3 mostRight = screenxyz[0] > screenXyz[0] ? screenxyz : screenXyz;
    mostRight = mostRight[0] > screenxYz[0] ? mostRight : screenxYz;
    mostRight = mostRight[0] > screenxyZ[0] ? mostRight : screenxyZ;
    mostRight = mostRight[0] > screenXYz[0] ? mostRight : screenXYz;
    mostRight = mostRight[0] > screenXyZ[0] ? mostRight : screenXyZ;
    mostRight = mostRight[0] > screenxYZ[0] ? mostRight : screenxYZ;
    mostRight = mostRight[0] > screenXYZ[0] ? mostRight : screenXYZ;

    Vec4 bounds = Vec4(mostLeft[0], mostLeft[1] < mostRight[1] ? mostLeft[1] : mostRight[1], mostRight[0], mostLeft[1] > mostRight[1] ? mostLeft[1] : mostRight[1]);
    return bounds;
}

void lgLabel::setTransparency(float alpha)
{
    Vec4 currentColor = getColor();
    setColor( Vec4(currentColor[0], currentColor[1], currentColor[2],alpha) );
    Vec4 currentColorBbox = getBoundingBoxColor();
    currentColorBbox[3]= alpha;
    setBoundingBoxColor(currentColorBbox);
}

 void lgLabel::setUpdatedMatrixMatrix (const Matrix &mat)
 {
     updatedMatrix->setMatrix(mat);
 }

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

