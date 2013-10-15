#include "../include/lgNode.h"

// Default constructor - initialize the name to ""
lgNode::lgNode() : osg::NodeVisitor(TRAVERSE_ALL_CHILDREN),
                                        searchForName()
{
}

// Constructor that accepts string argument
// Initializes the name of the node
lgNode::lgNode(const std::string &name) :
                                   osg::NodeVisitor(TRAVERSE_ALL_CHILDREN),
                                   searchForName(name)
{
}

 //The 'apply' method for 'node' type instances.
 //Compare the 'searchForName' data member against the node's name.
 //If the strings match, add this node to our list
void lgNode::apply(osg::Node &searchNode)
{
    // If no node is found, return searchNode
    if (searchForName == "")
    {
        foundNodeList.push_back(&searchNode);
    }
    else
    {
       if (searchNode.getName() == searchForName)
       {
          foundNodeList.push_back(&searchNode);
       }
       traverse(searchNode);
    }
}

// Set the searchForName to user-defined string
void lgNode::setNameToFind(const std::string &searchName)
{
   searchForName = searchName;
   foundNodeList.clear();
}

osg::Node* lgNode::getFirst()
{
   return *(foundNodeList.begin());
}
