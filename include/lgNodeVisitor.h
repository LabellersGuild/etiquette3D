/** File: lgNodeVisitor.h
 *
 * Description :
 * The goal of this class is to help finding a specific node inside a Group
 * created by reading a citygml file.
 * It also helps to find the coordinates of what is drawn in this node and its children.
 * To use this, create a group from the node read by the citygml plugin
 * use the accept method of this group with an instance of lgNodeVisitor as argument to find the node you want
 *
 * Author: Paul-Yves
 */

#ifndef LGNODE_H_INCLUDED
#define LGNODE_H_INCLUDED

#include <osg/NodeVisitor>
#include <osg/Node>

#include <iostream>
#include <vector>

class lgNodeVisitor : public osg::NodeVisitor {
public:

    /** Default constructor - initialize the name to ""
     */
    lgNodeVisitor();

    /** Constructor that accepts string argument
     * Initializes the name of the node
     * @param name : string : name of the node
     */
    lgNodeVisitor(const std::string &name);

    /** The 'apply' method for 'node' type instances.
     * Compare the 'searchForName' data member against the node's name.
     * If the strings match, add this node to our list
     * @param searchNode : Node : node to search
     */
    virtual void apply(osg::Node &searchNode);

    /** Setter of searchForName
     * Clear the list of found nodes
     * @param searchName : string : name of the node
     */
    void setNameToFind(const std::string &name);

    /** Retrunr the first node of the found nodes list
     * @return Node* : the node
     */
    osg::Node* getFirst();

    /** Retrunr the last node of the found nodes list
     * @return Node* : the node
     */
    osg::Node* getLast();

    typedef std::vector<osg::Node*> nodeListType;

    /** Getter of foundNodeList
     * @return nodeListeType : foundNodeList (nodeListType = vector<Node*>)
     */
    nodeListType& getNodeList()
    {
        return foundNodeList;
    }

    /** This method recursively feed the list of points with the
     * members of the vertex arrays inside the Geode children of the first targetNode
     * The first target node should be the found node's value : *(findNode.getFirst())
     * warning, using this method before calling an instance of this class through the
     * accept method of a Group will probably lead to errors
     * @param targetNode : Node :  Should be the found node's value
     */
    void feedFoundPointList(osg::Node& targetNode);

    /** Getter of foundPointList
     * @return vector<Vec3> : foundPointList
     */
    std::vector<osg::Vec3> getFoundPointList();

    /** Calculate a Vec3 made from the foundPointList points that is recommended
     * as the positionInit of a label linked to foundNode
     * @return Vec3 : recommended position
     */
    osg::Vec3 recommendedCoordinates();

protected:
    /** The name of the node we are looking for
     */
    std::string searchForName;

    /** List of found nodes
     */
    nodeListType foundNodeList;

    /** This vector contains all the osg::Vec3 corresponding
     * to the points in the vertex array of all drawables
     * inside the found node's and its children geodes
     */
    std::vector<osg::Vec3> foundPointList;
};

#endif // LGNODE_H_INCLUDED
