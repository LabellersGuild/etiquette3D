#ifndef TEST_H
#define TEST_H

#include <osg/NodeVisitor>
#include <osg/Node>
#include "../include/lgNodeVisitor.h"
#include "../include/lgNodeOverseer.h"
#include "../include/lgLabel.h"
#include "../include/LGAnimation.h"
#include "../include/LGInteraction.h"
#include "../include/myLGAnimation.h"
#include "../include/myLGAnimation2.h"
#include <iostream>
#include <vector>

using namespace std;
using namespace osg;

class test
{
    public:
        test();
        virtual ~test();
        lgNodeVisitor test_lgNodeVisitor_initialisation(Group* model);
        void test_label_setLinkNode(ref_ptr<Node> theLinkNode, lgLabel* theLabel);

    protected:

    private:
};

#endif // TEST_H
