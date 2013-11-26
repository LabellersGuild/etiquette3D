/*
 * File:   lgNodeOverseer.cpp
 * Author: paulyves
 *
 * Created on November 5, 2013, 2:18 PM
 *
 * This class is used to visualise how a cityGML file is built and
 * parsed in OSG, usefull if you are not sure about what is inside
 * your citygml file
 */



#include <osg/Geode>
#include <osg/Geometry>

#include "../include/lgNodeOverseer.h"

using namespace std;
using namespace osg;

lgNodeOverseer::lgNodeOverseer() : NodeVisitor(TRAVERSE_ALL_CHILDREN),
                                        searchForName(){
    _level = 0;
    showDrawable = true;
}

lgNodeOverseer::lgNodeOverseer(const string &name) :
                                   NodeVisitor(TRAVERSE_ALL_CHILDREN),
                                   searchForName(name){
    _level = 0;
    showDrawable = true;
}

//The 'apply' method for 'node' type instances.
//we will print different information concerning the node
//Compare the 'searchForName' data member against the node's name.
//If the strings match, add this node to our list

void lgNodeOverseer::apply(Node &searchNode) {
    cout << spaces() << "Node simple : " << searchNode.getName() << endl;
    // If no node is found, return searchNode
    if (searchForName == "") {
        foundNodeList.push_back(&searchNode);
    } else {
        if (searchNode.getName() == searchForName) {
            foundNodeList.push_back(&searchNode);
    }
        _level++;
        traverse(searchNode);
        _level--;
    }
}

void lgNodeOverseer::apply(Group &searchNode) {
    cout << spaces() << "Groupe :"<< &(searchNode.getName()) << ", " << searchNode.getNumChildren() << " noeuds fils" << endl;
    // If no node is found, return searchNode
    if (searchForName == "") {
        foundNodeList.push_back(&searchNode);
    } else {
        if (searchNode.getName() == searchForName) {
            foundNodeList.push_back(&searchNode);
        }
        _level++;
        traverse(searchNode);
        _level--;
    }
}

void lgNodeOverseer::apply(Geode &searchNode) {
    cout << spaces() << "Géode :"<< searchNode.getName() << ", " << searchNode.getNumDrawables() << " drawables" << endl;
    if(searchNode.getNumDrawables()>0 && showDrawable){
        for (unsigned i=0;i<searchNode.getNumDrawables();i++){
            _level++;
            cout << spaces() << "Drawable " << i << " : " << searchNode.getDrawable(i)->getName() << endl;
            ref_ptr<Geometry> myGeom = dynamic_cast<Geometry*>(searchNode.getDrawable(i));
            if(myGeom){
                ref_ptr<Vec3Array> arrayVertex = (Vec3Array*) myGeom->getVertexArray();
                _level++;
                int size = arrayVertex->size();
                cout << spaces() << "Il y a " << size << " sommets" << endl;
                for (int j=0; j<size;j++){
                    cout << spaces() << "Sommet " << j+1 << " : x=" << arrayVertex->at(j).x() <<
                            ", y=" << arrayVertex->at(j).y() << ", z=" << arrayVertex->at(j).z() << endl;
                }
                _level--;
            } else {
                cout << "Pas de géométrie" << endl;
            }
            _level--;
        }
        cout << spaces() << endl;
    }
    // If no node is found, return searchNode
    if (searchForName == "") {
        foundNodeList.push_back(&searchNode);
    } else {
        if (searchNode.getName() == searchForName) {
            foundNodeList.push_back(&searchNode);
        }
        _level++;
        traverse(searchNode);
        _level--;
    }
}

lgNodeOverseer::~lgNodeOverseer() {
}

Node* lgNodeOverseer::getFirst() {
    return (foundNodeList.at(0));
}

Node* lgNodeOverseer::getLast() {
    int taille = foundNodeList.size();
    return (foundNodeList.at(taille-1));
}

bool lgNodeOverseer::getShowDrawable(){
    return showDrawable;
}

void lgNodeOverseer::setShowDrawable(bool toggle){
    showDrawable = toggle;
}
