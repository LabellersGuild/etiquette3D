/* 
 * File:   lgNodeOverseer.h
 * Author: paulyves
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
    lgNodeOverseer();
    lgNodeOverseer(const std::string &name);
    virtual ~lgNodeOverseer();
    virtual void apply(osg::Node &searchNode);
    virtual void apply(osg::Group &searchNode);
    virtual void apply(osg::Geode &searchNode);
    
    void setNameToFind(const std::string &name);

    osg::Node* getFirst();
    osg::Node* getLast();

    typedef std::vector<osg::Node*> nodeListType;

    std::string spaces() {
        return std::string(_level * 2, ' ');
    }
    
    nodeListType& getNodeList() {
        return foundNodeList;
    }
    
    void setShowDrawable (bool toggle);
    bool getShowDrawable();
    
protected:
    std::string searchForName;
    nodeListType foundNodeList;
    /*
     * used by the Overseer to indent its printing of nodes
     */
    unsigned int _level;
    /**
     * allow the Overseer to print details about drawables, default is true
     **/
    bool showDrawable;
};

#endif	/* LGNODEOVERSEER_H */

