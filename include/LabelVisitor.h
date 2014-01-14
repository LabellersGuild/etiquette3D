/* 
 * File:   LabelVisitor.h
 * Author: paulyves
 *
 * Created on January 14, 2014, 11:40 AM
 */

#ifndef LABELVISITOR_H
#define	LABELVISITOR_H

#include <osg/NodeVisitor>
#include <osg/Node>

#include <iostream>
#include <vector>

#include "lgLabel.h"
using namespace std;
using namespace osg;

class LabelVisitor : public NodeVisitor{
public:
    LabelVisitor();
    LabelVisitor(const LabelVisitor& orig);
    virtual ~LabelVisitor();
    /** The 'apply' method for 'Geode' type instances
     * @param searchNode : Geode : node that is searched
     */
    virtual void apply(osg::Geode &searchNode);
    vector<ref_ptr<lgLabel> > getLabelList();
protected:
    vector<ref_ptr<lgLabel> > labelList;
};

#endif	/* LABELVISITOR_H */

