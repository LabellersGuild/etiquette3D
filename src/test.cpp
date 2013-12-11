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


test::test()
{
    //ctor
}

test::~test()
{
    //dtor
}

/**
 * This test is used to set a label (with idNode as input) and
 * see if we can recommand a position
 * @param model the model containing the citygml as OSG
 * @return findNode : the nodeVisitor fed with the vertex of child drawables and recommended position
 */
lgNodeVisitor test::test_lgNodeVisitor_initialisation(Group* model){

    // Find the node with its ID :
    cout << "Entrer l'id du noeud � �tiquetter :" << endl;
    string idNode;
    cin >> idNode;
    //string idNode = "ID_276003000001240";

    lgNodeVisitor findNode(idNode);
    model->accept(findNode);

    if(idNode==(findNode.getFirst())->getName()){
        cout<<"Le noeud trouv� correspond "<<endl;
    }else{
        cout<<"mauvaisNoeud"<<endl;
    }

    findNode.feedFoundPointList(*(findNode.getFirst()));
    vector<Vec3> points = findNode.getFoundPointList();
    cout << "On a trouv� un liste de " << points.size() << " points" << endl;

    Vec3 positionCalc = findNode.recommendedCoordinates();
    cout << "Le point recommand� est : x:" << positionCalc.x() << " y=" << positionCalc.y() << " z=" << positionCalc.z() << endl;

    return findNode;
}

/**
 * Function used to check that we got the proper linkNode
 * @param theLinkNode the Node father of the label
 * @param theLabel the label we want to link
 */
void test::test_label_setLinkNode(ref_ptr<Node> theLinkNode, lgLabel* theLabel){
    if(theLinkNode.get()==(theLabel->getLinkNode()).get()){
        cout<<"Le linkNode a bien �t� assign� � l'�tiquette"<<endl;
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

void test::test_label_compute2dBox(osgViewer::Viewer* viewer, lgLabel* theLabel)
{
    Vec4 bbox = theLabel->compute2dBox(viewer);
    cout << "2D Bounding box of the label : xMin = " << bbox[0] << " , xMax = " << bbox[2] << " ,yMin = " << bbox[1] << " ,yMax = " << bbox[3] << endl;
}

void test::test_label_compute2dCenter(osgViewer::Viewer* viewer, lgLabel* theLabel)
{
    Vec2 center = theLabel->compute2dCenter(viewer);
    cout << "2D Center of the label : ( " << center.x() << " , " << center.y() << " )" << endl;
}

