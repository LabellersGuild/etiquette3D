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


test::test()
{
    //ctor
}

test::~test()
{
    //dtor
}

lgNodeVisitor test::test_lgNodeVisitor_initialisation(Group* model){

    // Find the node with its ID :
    cout << "Entrer l'id du noeud à étiquetter :" << endl;
    string idNode;
    cin >> idNode;
    //string idNode = "ID_276003000001240";

    lgNodeVisitor findNode(idNode);
    model->accept(findNode);

    if(idNode==(findNode.getFirst())->getName()){
        cout<<"Le noeud trouvé correspond "<<endl;
    }else{
        cout<<"mauvaisNoeud"<<endl;
    }

    findNode.feedFoundPointList(*(findNode.getFirst()));
    vector<Vec3> points = findNode.getFoundPointList();
    cout << "On a trouvé un liste de " << points.size() << " points" << endl;

    Vec3 positionCalc = findNode.recommendedCoordinates();
    cout << "Le point recommandé est : x:" << positionCalc.x() << " y=" << positionCalc.y() << " z=" << positionCalc.z() << endl;

    return findNode;
}
