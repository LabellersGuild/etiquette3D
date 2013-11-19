/* Fichier : myLGAnimation.cpp
 * Description : Fichier créant une sous classe de LGAnimation, pour redéfinir la méthode operator()
 * Auteur : Thomas Brunel
 */

#include "../include/myLGAnimation.h"
#include "../include/lgLabel.h"

using namespace osg;

void myLGAnimation::operator()(Node* node, NodeVisitor* nv)
{
    // Find the world coordinates of the node :
    Matrix worldMatrix = node->getWorldMatrices()[0];
    Vec3 center = Vec3(worldMatrix(3,0), worldMatrix(3,1), worldMatrix(3,2));

    if (!isFree(node, nv))
    {
       translateLabel(node, 0, 0, 1);
    }
    else if (center[2]>0)
    {
        //Look if below the label is free space
        if (isFree(node, nv, 0,30,0,0))
        {
            translateLabel(node, 0, 0, -1);
        }
    }

    //Allow OSG to continue the node traversal
    traverse(node, nv);
}
