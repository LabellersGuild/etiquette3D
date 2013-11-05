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

lgNodeOverseer::lgNodeOverseer() : osg::NodeVisitor(TRAVERSE_ALL_CHILDREN),
                                        searchForName(){
    _level = 0;
}

lgNodeOverseer::lgNodeOverseer(const std::string &name) :
                                   osg::NodeVisitor(TRAVERSE_ALL_CHILDREN),
                                   searchForName(name){
    _level = 0;
}

//The 'apply' method for 'node' type instances.
//we will print different information concerning the node
//Compare the 'searchForName' data member against the node's name.
//If the strings match, add this node to our list

void lgNodeOverseer::apply(osg::Node &searchNode) {
    std::cout << spaces() << "Node simple : " << searchNode.getName() << std::endl;
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

void lgNodeOverseer::apply(osg::Group &searchNode) {
    std::cout << spaces() << "Groupe :"<< &(searchNode.getName()) << ", " << searchNode.getNumChildren() << " noeuds fils" << std::endl;
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

void lgNodeOverseer::apply(osg::Geode &searchNode) {
    std::cout << spaces() << "Géode :"<< searchNode.getName() << ", " << searchNode.getNumDrawables() << " drawables" << std::endl;
    if(searchNode.getNumDrawables()>0){
        for (int i=0;i<searchNode.getNumDrawables();i++){
            _level++;
            std::cout << spaces() << "Drawable " << i << " : " << searchNode.getDrawable(i)->getName() << std::endl;
            osg::Geometry* myGeom = (osg::Geometry*) searchNode.getDrawable(i);
            if(myGeom){
                osg::Vec3Array* arrayVertex = (osg::Vec3Array*) myGeom->getVertexArray();
                _level++;
                int size = arrayVertex->size();
                std::cout << spaces() << "Il y a " << size << " sommets" << std::endl;
                for (int j=0; j<size;j++){
                    std::cout << spaces() << "Sommet " << j+1 << " : x=" << arrayVertex->at(j).x() <<
                            ", y=" << arrayVertex->at(j).y() << ", z=" << arrayVertex->at(j).z() << std::endl;
                }
                _level--;
            } else {
                std::cout << "Pas de géométrie" << std::endl;
            }
            _level--;
        }
        std::cout << spaces() << std::endl;
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

osg::Node* lgNodeOverseer::getFirst() {
    return (foundNodeList.at(0));
}

osg::Node* lgNodeOverseer::getLast() {
    int taille = foundNodeList.size();
    return (foundNodeList.at(taille-1));
}

