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
     labelType = EXTERNAL;
}

lgLabel::lgLabel(const lgLabel& originalLabel) {
     hidingDistance = -1;
     labelType = EXTERNAL;
}

lgLabel::lgLabel(std::string text, ref_ptr<Node> linkedNode, Vec3 recoPos) {
    this->setText(text);
    this->setLinkNode(linkedNode, recoPos);
    hidingDistance = -1;
    labelType = EXTERNAL;
}

lgLabel::lgLabel(std::string filePath, std::string idNode) {
     hidingDistance = -1;
     labelType = EXTERNAL;
}

//getters and setters
/**
 * set the param as linkNode attribute, then see if it got the
 * label in his children, if not it adds it (eventually creating a new
 * geode if the param is a group)
 * @param aNode, osg:ref_ptr<Node> to the node
 */
void lgLabel::setLinkNode(ref_ptr<Node> aNode, Vec3 recoPos){
    this->linkNode = aNode;
    this->setPositionInit(recoPos);
    this->setChangingWhenMouse(true);

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

        /// This is a new way to find a recommended position :
        BoundingBox objectBBox = computeObjectBBox(updatedMatrix->getParent(0),BoundingBox(0,0,0,0,0,0));
        osg::Matrixd matrixPosition = osg::Matrixd::translate((objectBBox.xMax()+objectBBox.xMin())/2.0, (objectBBox.yMax()+objectBBox.yMin())/2.0, objectBBox.zMax()+10.0);
        updatedMatrix->setMatrix(matrixPosition);
        ///

    }
    //adding the label as child of the geode
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

/*
 *set a relative position for the label, not recommended
 * as it will be independant from the matrixTransform updatedMatrix
 * and therefore will have trouble with animations
 */
void lgLabel::setPosition(osg::Vec3 relativePosition) {

    osgText::Text::setPosition(relativePosition);
    this->calcAbsolutePosition();
}

lgType lgLabel::getLabelType(){
    return this->labelType;
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

/*
 *return the xmin, ymin, xmax, ymax screen coordinates of the bounding box
 */
osg::Vec4 lgLabel::compute2dBox(osg::ref_ptr<osgViewer::Viewer> view)
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

void lgLabel::setSeeInTransparency(bool b)
{
    if (b)
    {
        setTransparency(0.5);

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

        updatedMatrix->setStateSet(new StateSet());
    }
}

void lgLabel::setPreviousDrawMode(int d)
{
    previousDrawMode = d;
}

int lgLabel::getPreviousDrawMode()
{
    return previousDrawMode;
}

void lgLabel::setDefaultDrawMode(int d)
{
    defaultDrawMode = d;
}

int lgLabel::getDefaultDrawMode()
{
    return defaultDrawMode;
}

bool lgLabel::isChangingWhenMouse()
{
   return changingWhenMouse;
}

void lgLabel::setChangingWhenMouse(bool b)
{
    changingWhenMouse = b;
}


BoundingBox lgLabel::computeObjectBBox(ref_ptr<Group> node, BoundingBox bbox)
{
    for(unsigned i=0;i< node->getNumChildren();i++)
    {
        if (node->getChild(i)->isSameKindAs(new Geode()))
        {
            ref_ptr<Geode> child = dynamic_cast<Geode*>(node->getChild(i));
            for(unsigned j=0;j< child->getNumDrawables();j++)
            {
                if (! child->getDrawable(i)->isSameKindAs(new lgLabel()))
                {
                    BoundingBox childbbox = child->getDrawable(j)->getBound();

                    if (bbox.xMax()==0 && bbox.xMin() == 0 && bbox.yMax()==0 && bbox.yMin()==0 && bbox.zMax()==0 && bbox.zMin() ==0)
                    {
                        bbox.xMax() = childbbox.xMax();
                        bbox.yMax() = childbbox.yMax();
                        bbox.zMax() = childbbox.zMax();
                        bbox.xMin() = childbbox.xMin();
                        bbox.yMin() = childbbox.yMin();
                        bbox.zMin() = childbbox.zMin();
                    }
                    else
                    {
                        if (bbox.xMax() < childbbox.xMax()) bbox.xMax() = childbbox.xMax();
                        if (bbox.yMax() < childbbox.yMax()) bbox.yMax() = childbbox.yMax();
                        if (bbox.zMax() < childbbox.zMax()) bbox.zMax() = childbbox.zMax();
                        if (bbox.xMin() > childbbox.xMin()) bbox.xMin() = childbbox.xMin();
                        if (bbox.yMin() > childbbox.yMin()) bbox.yMin() = childbbox.yMin();
                        if (bbox.zMin() > childbbox.zMin()) bbox.zMin() = childbbox.zMin();
                    }
                }
            }
            return bbox;
        }
        else if (node->getChild(i)->isSameKindAs(new Group()))
        {
            return computeObjectBBox(dynamic_cast<Group*>(node->getChild(i)), bbox);
        }
    }
    return bbox;
}

void lgLabel::setLabelType(lgType type, ref_ptr<LGAnimation> animation){
   this->labelType=labelType;
   updatedMatrix->setUpdateCallback(animation);
   labelType = type;

   if (type == EXTERNAL)
   {
       setAxisAlignment(osgText::Text::SCREEN);
       setAlignment(osgText::Text::CENTER_TOP);
       updatedMatrix->setMatrix(Matrixd::translate(getPositionInit()));
   }
   else if (type == INTERNAL_TOP)
   {
       setAxisAlignment(osgText::Text::XY_PLANE);
       setAlignment(osgText::Text::CENTER_CENTER);

        BoundingBox bbox = computeObjectBBox(updatedMatrix->getParent(0),BoundingBox(0,0,0,0,0,0) );

        //Put the label on the object
        updatedMatrix->setMatrix(Matrixd::translate((bbox.xMax()+bbox.xMin())/2.0,(bbox.yMax()+bbox.yMin())/2, bbox.zMax()));
   }
   else //INTERNAL_FACE
   {
       setAxisAlignment(osgText::Text::XZ_PLANE);
       setAlignment(osgText::Text::CENTER_BOTTOM);

       BoundingBox bbox = computeObjectBBox(updatedMatrix->getParent(0),BoundingBox(0,0,0,0,0,0) );

       updatedMatrix->setMatrix(Matrixd::translate((bbox.xMax()+bbox.xMin())/2.0,bbox.yMin(), (bbox.zMax()+bbox.zMin())/2));
   }
}
