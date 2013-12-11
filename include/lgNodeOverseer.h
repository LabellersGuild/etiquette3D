/** File: lgNodeOverseer.h
 *
 * Description :
 * This class is used to visualise how a cityGML file is built and
 * parsed in OSG, usefull if you are not sure about what is inside
 * your citygml file
 *
 * Author: Paul-Yves
 *
 * Created on November 5, 2013, 2:18 PM
 */

#ifndef LGNODEOVERSEER_H
#define	LGNODEOVERSEER_H

#include <osg/NodeVisitor>
#include <osg/Node>

#include <iostream>
#include <vector>

class lgNodeOverseer : public osg::NodeVisitor {
public:

    /** Default constructor
     */
    lgNodeOverseer();

    /** constructor
     * @param name : string : name of the node that is searched
     */
    lgNodeOverseer(const std::string &name);

    /** Destructor
     */
    virtual ~lgNodeOverseer();

    /** The 'apply' method for 'node' type instances.
     * we will print different information concerning the node
     * Compare the 'searchForName' data member against the node's name.
     * If the strings match, add this node to our list
     * @param searchNode : Node : node that is searched
     */
    virtual void apply(osg::Node &searchNode);

     /** The 'apply' method for 'group' type instances
     * @param searchNode : Group : node that is searched
     */
    virtual void apply(osg::Group &searchNode);

     /** The 'apply' method for 'Geode' type instances
     * @param searchNode : Geode : node that is searched
     */
    virtual void apply(osg::Geode &searchNode);

    /** Setter of searchForName
     * Clear the list of found nodes
     * @param searchName : string : name of the node
     */
    void setNameToFind(const std::string &name);

    /** Get the first found node
     * @return Node* : first found node
     */
    osg::Node* getFirst();

    /** Get the last found node
     * @return NOde* : last found node
     */
    osg::Node* getLast();

    /**  Function used when information is printed
     * @return string
     */
    std::string spaces() {
        return std::string(_level * 2, ' ');
    }

    typedef std::vector<osg::Node*> nodeListType;

    /** Getter of foundNodeList
     * @return nodeListeType : foundNodeList (nodeListType = vector<Node*>)
     */
    nodeListType& getNodeList()
    {
        return foundNodeList;
    }

    /** Setter of showDrawable
     * @param : toogle : boo : new showDrawable
     */
    void setShowDrawable (bool toggle);

    /** Getter of showDrawable
     * @return bool : showDrawable
     */
    bool getShowDrawable();

protected:

    /** Name of the node that is searched
     */
    std::string searchForName;

    /** list of the found nodes
     */
    nodeListType foundNodeList;

    /** Used by the Overseer to indent its printing of nodes
     */
    unsigned int _level;

    /** Allow the Overseer to print details about drawables, default is true
     */
    bool showDrawable;
};

#endif	/* LGNODEOVERSEER_H */

