#ifndef LGNODE_H_INCLUDED
#define LGNODE_H_INCLUDED

#include <osg/NodeVisitor>
#include <osg/Node>

#include <iostream>
#include <vector>
/*
 *The goal of this class is to help finding a specific node inside a Group
 * created by reading a citygml file.
 * It also helps to find the coordinates of what is drawn in this node and its children. 
 * To use this, create a group from the node read by the citygml plugin
 * use the accept method of this group with an instance of lgNodeVisitor as arg to find the node you want
 */
class lgNodeVisitor : public osg::NodeVisitor {
public:

    lgNodeVisitor();

    lgNodeVisitor(const std::string &name);

    virtual void apply(osg::Node &searchNode);
    void setNameToFind(const std::string &name);

    osg::Node* getFirst();
    osg::Node* getLast();

    typedef std::vector<osg::Node*> nodeListType;

    nodeListType& getNodeList() {
        return foundNodeList;
    }
    
    void feedFoundPointList(osg::Node& targetNode);
    std::vector<osg::Vec3> getFoundPointList();
    osg::Vec3 recommendedCoordinates();
    
protected:
    /*
     *The name of the node we are looking for 
     */
    std::string searchForName;
    nodeListType foundNodeList;
    /*
     *This vector contains all the osg::Vec3 corresponding
     * to the points in the vertex array of all drawables
     * inside the found node's and its children geodes
     */
    std::vector<osg::Vec3> foundPointList;
};

#endif // LGNODE_H_INCLUDED
