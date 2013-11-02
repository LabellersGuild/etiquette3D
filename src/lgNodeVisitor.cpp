#include "../include/lgNodeVisitor.h"

// Default constructor - initialize the name to ""
lgNodeVisitor::lgNodeVisitor() : osg::NodeVisitor(TRAVERSE_ALL_CHILDREN),
                                        searchForName()
{
}

// Constructor that accepts string argument
// Initializes the name of the node
lgNodeVisitor::lgNodeVisitor(const std::string &name) :
                                   osg::NodeVisitor(TRAVERSE_ALL_CHILDREN),
                                   searchForName(name)
{
}

 //The 'apply' method for 'node' type instances.
 //Compare the 'searchForName' data member against the node's name.
 //If the strings match, add this node to our list
void lgNodeVisitor::apply(osg::Node &searchNode)
{
    // If no node is found, return searchNode
    if (searchForName == "")
    {
        foundNodeList.push_back(&searchNode);
    }
    else
    {
       if (searchNode.getName() == searchForName) {
            foundNodeList.push_back(&searchNode);
//            osg::MatrixList ml = (searchNode).getWorldMatrices();
//            int size = ml.size();
//            std::cout << "il y a " << size << " matrices de transfo" << std::endl;
//            for (int i = 0; i < 4; i++) {
//                for (int j = 0; j < 4; j++) {
//                    std::cout << ml[0](j, i) << "\t";
//                }
//                std::cout << "\n";
//            }
        }
        traverse(searchNode);
    }
}

// Set the searchForName to user-defined string
void lgNodeVisitor::setNameToFind(const std::string &searchName)
{
   searchForName = searchName;
   foundNodeList.clear();
}

osg::Node* lgNodeVisitor::getFirst() {
    return (foundNodeList.at(0));
}

osg::Node* lgNodeVisitor::getLast() {
    int taille = foundNodeList.size();
    return (foundNodeList.at(taille-1));
}