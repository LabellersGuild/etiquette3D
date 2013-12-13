/** File : test.cpp
 * Description : Test file for our library
 */

 #include "../include/test.h"

#include <osg/Group>
#include <osgDB/ReadFile>
#include <osg/PositionAttitudeTransform>
#include <osgViewer/Viewer>
#include <osgSim/MultiSwitch>

#include <osgGA/TrackballManipulator>

#include <osgText/Font>
#include <osgText/Text>
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <osg/BlendFunc>

using namespace std;
using namespace osg;

/**
 * This test is used to set a label (with idNode as input) and
 * see if we can recommand a position
 * @param model the model containing the citygml as OSG
 * @return findNode : the nodeVisitor fed with the vertex of child drawables and recommended position
 */
lgNodeVisitor test::test_lgNodeVisitor_initialisation(Group* model)
{

    // Find the node with its ID :
    cout << "Entrer the id of the node that will be labelled :" << endl;
    string idNode;
    cin >> idNode;
    //string idNode = "ID_276003000001240";

    lgNodeVisitor findNode(idNode);
    model->accept(findNode);

    if(idNode==(findNode.getFirst())->getName()){
        cout<<"Found the right node "<<endl;
    }else{
        cout<<"Wrong node"<<endl;
    }

    findNode.feedFoundPointList(*(findNode.getFirst()));
    vector<Vec3> points = findNode.getFoundPointList();
    cout << "A list of " << points.size() << " points have been found" << endl;

    Vec3 positionCalc = findNode.recommendedCoordinates();
    cout << "The recommended position is : x=" << positionCalc.x() << " y=" << positionCalc.y() << " z=" << positionCalc.z() << endl;

    return findNode;
}

/**
 * Function used to check that we got the proper linkNode
 * @param theLinkNode the Node father of the label
 * @param theLabel the label we want to link
 */
void test::test_label_setLinkNode(ref_ptr<Node> theLinkNode, lgLabel* theLabel)
{
    if(theLinkNode.get()==(theLabel->getLinkNode()).get()){
        cout<<"The linkNode has been correctly linked to the label"<<endl;
    }
}

/**
 * Test if translation of the label works
 * @param theLabel : the label we want to translate
 */
void test::test_label_translateLabel(lgLabel* theLabel)
{
    Vec3 pos = theLabel->getAbsolutePosition();
    cout << "Position before translation : ( " << pos.x() << " , " << pos.y() << " , " << pos.z() << " )" << endl;
    theLabel->translateLabel(4,-6,10);
    Vec3 pos2 = theLabel->getAbsolutePosition();
    cout << "Translation(4,-6,10) : ( " << pos2.x() << " , " << pos2.y() << " , " << pos2.z() << " )" << endl;
    theLabel->translateLabel(-4,6,-10);
}

/**
 * Test if the 2D bounding box computed for the label is correct
 * @param viewer : osgViewer::Viewer* : the viewer object of the main file
 * @param theLabel : lgLabel* : label
 */
void test::test_label_compute2dBox(osgViewer::Viewer* viewer, lgLabel* theLabel)
{
    Vec4 bbox = theLabel->compute2dBox(viewer);
    cout << "2D Bounding box of the label : xMin = " << bbox[0] << " , xMax = " << bbox[2] << " ,yMin = " << bbox[1] << " ,yMax = " << bbox[3] << endl;
}

/**
 * Test if the 2D center calculated for the label is correct
 * @param viewer : osgViewer::Viewer* : the viewer object of the main file
 * @param theLabel : lgLabel* : label
 */
void test::test_label_compute2dCenter(osgViewer::Viewer* viewer, lgLabel* theLabel)
{
    Vec2 center = theLabel->compute2dCenter(viewer);
    cout << "2D Center of the label : ( " << center.x() << " , " << center.y() << " )" << endl;
}

