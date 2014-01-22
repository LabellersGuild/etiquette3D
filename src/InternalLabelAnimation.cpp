/** File: InternalLabelAnimation.cpp
 * Description : file that creates a sub-class of LGAnimation for internal labels.
 * Author : Thomas
 */

#include "../include/InternalLabelAnimation.h"
#include "../include/lgLabel.h"

using namespace osg;
using namespace std;

/** ConstructorID_311-TheMagdalena
 */
InternalLabelAnimation::InternalLabelAnimation(osgViewer::Viewer* viewer, ref_ptr<lgLabel> label) :lgAnimation(viewer)
{
    //label
    this->label = label;

     //Bounding box of the object
    this->bbox = label->computeObjectBBox(label->getUpdatedMatrix()->getParent(0),BoundingBox(0,0,0,0,0,0) );

    //Corners of the boundingbx, local :
    lCorners[0]=Vec3(bbox.xMin(),bbox.yMin(),bbox.zMin());
    lCorners[1]=Vec3(bbox.xMin(),bbox.yMin(),bbox.zMax());
    lCorners[2]=Vec3(bbox.xMin(),bbox.yMax(),bbox.zMin());
    lCorners[3]=Vec3(bbox.xMin(),bbox.yMax(),bbox.zMax());
    lCorners[4]=Vec3(bbox.xMax(),bbox.yMin(),bbox.zMin());
    lCorners[5]=Vec3(bbox.xMax(),bbox.yMin(),bbox.zMax());
    lCorners[6]=Vec3(bbox.xMax(),bbox.yMax(),bbox.zMin());
    lCorners[7]=Vec3(bbox.xMax(),bbox.yMax(),bbox.zMax());

    //Find the absolute coordinates of the corners
    Vec4 extendedlCorners[8];
    for(int i=0;i<8;i++)
        extendedlCorners[i]=Vec4(lCorners[i].x(),lCorners[i].y(),lCorners[i].z(),1);

    MatrixList matricesList = label->getUpdatedMatrix()->getParent(0)->getWorldMatrices();

    for (unsigned i=0;i<matricesList.size();i++)
    {
        for (int j=0;j<8;j++)
             extendedlCorners[j] = extendedlCorners[j]*matricesList[i];
    }

    //Absolute position
    for (int i=0;i<8;i++)
        aCorners[i]= Vec3(extendedlCorners[i].x(), extendedlCorners[i].y(), extendedlCorners[i].z());

    //Planes of each face
    Plane planesFaces[5];
    planesFaces[0]= Plane(aCorners[3], aCorners[7], aCorners[6]);
    planesFaces[1]= Plane(aCorners[7], aCorners[5], aCorners[4]);
    planesFaces[2]= Plane(aCorners[5], aCorners[1], aCorners[0]);
    planesFaces[3]= Plane(aCorners[1], aCorners[3], aCorners[2]);
    planesFaces[4]= Plane(aCorners[1], aCorners[5], aCorners[7]);

    //Normals
    for (int i=0;i<5;i++)
        normals[i]=planesFaces[i].getNormal();

    //corners linked to each face,
    faceCorners[0][0] = 3;
    faceCorners[0][1] = 7;
    faceCorners[0][2] = 6;
    faceCorners[0][3] = 2;
    faceCorners[1][0] = 7;
    faceCorners[1][1] = 5;
    faceCorners[1][2] = 4;
    faceCorners[1][3] = 6;
    faceCorners[2][0] = 5;
    faceCorners[2][1] = 1;
    faceCorners[2][2] = 0;
    faceCorners[2][3] = 4;
    faceCorners[3][0] = 1;
    faceCorners[3][1] = 3;
    faceCorners[3][2] = 2;
    faceCorners[3][3] = 0;
    faceCorners[4][0] = 7;
    faceCorners[4][1] = 3;
    faceCorners[4][2] = 1;
    faceCorners[4][3] = 5;
}


 /** Override of operator()
  */
void InternalLabelAnimation::operator()(Node* node, NodeVisitor* nv)
{
    lgAnimation::operator()(node, nv);

    // We want the label to be :
    // - on a face with the best orientation to the camera
    // - where there is space
    // - high on a face (the higher the label, the more we are likely to see it)

    //Position of the camera in the absolute frame of reference
    Matrix matrixCamera = view->getCamera()->getInverseViewMatrix();
    Vec3 positionCamera= Vec3(matrixCamera(3,0), matrixCamera(3,1), matrixCamera(3,2));

    //Vectors Camera-faces :
    vector<Vec3> cameraFaceVectors;
    for (int i=0;i<5;i++)
        cameraFaceVectors.push_back(Vec3(positionCamera-(aCorners[faceCorners[i][0]]+aCorners[faceCorners[i][1]]+aCorners[faceCorners[i][2]]+aCorners[faceCorners[i][3]])/4));

    //Orientations
    map<double,int> orientations;
    for(int i=0;i<5;i++)
         orientations.insert(acos((cameraFaceVectors[i]*normals[i])/(cameraFaceVectors[i].length()*normals[i].length())), i);


    //Sort the corners of the faces
    int cornersSorted[5][4];
    for(int i=0;i<5;i++ )

    {
        double minOrientation=3.16; //rad
        int index=0;
        for (int j=0;j<5;j++)
        {
            if (orientations[j]<=minOrientation)
            {
                minOrientation = orientations[j];
                index=j;
            }
        }
        for (int j=0;j<4;j++)
            cornersSorted[i][j]=faceCorners[index][j];
        orientations[index]=3.17;
    }

    //we divide the face into small portions that are represented by points.
    const int iMax = 10; //Number of points per line
    const int jMax = 10; //Number of points per column

    int freePoints=0;
    int maxFreePoints=0;
    Vec3 beginningFreePosition;
    Vec3 endFreePosition;
    Vec3 localBeginningFreePosition;
    Vec3 localEndFreePosition;

    //Calculate the occlusions
    // Find the line which have the more consecutive free points. If there are several lines with this maximum, take the first one. The higher the label, the better
    // We could use different criteria.
    for (int k=0;k<5;k++)
    {
        for (int i=0;i<iMax;i++)
        {
            freePoints=0;
            for (int j=0;j<jMax;j++)
            {
                Vec3 currentPoint = ((aCorners[cornersSorted[k][0]]) * (iMax-i)*(jMax-j) + (aCorners[cornersSorted[k][1]]) * (iMax-i)*j + (aCorners[cornersSorted[k][2]]) * i*(jMax-j) + (aCorners[cornersSorted[k][3]])*i*j)/(iMax*jMax);
                osg::ref_ptr<osgUtil::LineSegmentIntersector> intersector = new osgUtil::LineSegmentIntersector(positionCamera,currentPoint);
                osgUtil::IntersectionVisitor iv( intersector.get() );
                iv.setTraversalMask( ~0x1 );
                view->getCamera()->accept( iv );
                //If there is nothing, or the label
                if ( !(intersector->containsIntersections()) || (intersector->containsIntersections() && (*(intersector->getIntersections().begin())).drawable == label))
                {
                    freePoints++;
                    if (maxFreePoints<freePoints)
                    {
                        maxFreePoints=freePoints;

                        beginningFreePosition = ((aCorners[cornersSorted[k][0]])*(iMax-i)*(jMax-j+maxFreePoints-1) + (aCorners[cornersSorted[k][1]])*(iMax-i)*(j-maxFreePoints+1) + (aCorners[cornersSorted[k][2]])*i*(jMax-j+maxFreePoints-1) + (aCorners[cornersSorted[k][3]])*i*(j-maxFreePoints+1))/(iMax*jMax);
                        endFreePosition = currentPoint;

                        localBeginningFreePosition = ((lCorners[cornersSorted[k][0]])*(iMax-i)*(jMax-j+maxFreePoints-1) + (lCorners[cornersSorted[k][1]])*(iMax-i)*(j-maxFreePoints+1) + (lCorners[cornersSorted[k][2]])*i*(jMax-j+maxFreePoints-1) + (lCorners[cornersSorted[k][3]])*i*(j-maxFreePoints+1))/(iMax*jMax);
                        localEndFreePosition = ((lCorners[cornersSorted[k][0]]) * (iMax-i)*(jMax-j) + (lCorners[cornersSorted[k][1]]) * (iMax-i)*j + (lCorners[cornersSorted[k][2]]) * i*(jMax-j) + (lCorners[cornersSorted[k][3]])*i*j)/(iMax*jMax);
                    }
                }
                else
                    freePoints=0;
            }

        }

        // There is a minimum size on screen between left and right points of the maxFreePoints
        //This minimum should change according to the label, the number of characters and its size
        Matrix modelView = view->getCamera()->getViewMatrix();
        Matrix projection = view->getCamera()->getProjectionMatrix();
        Matrix window = view->getCamera()->getViewport()->computeWindowMatrix();
        Matrix MVPW = modelView * projection * window;

        Vec3 beginningFreePositionProjection =  beginningFreePosition * MVPW ;
        Vec3 endFreePositionProjection = endFreePosition * MVPW ;
        double space = sqrt(pow(beginningFreePositionProjection.x()-endFreePositionProjection.x(),2)+pow(beginningFreePositionProjection.y()-endFreePositionProjection.y(),2));

        if (space>50) //Condition on the size on the screen
        {
            label->getUpdatedMatrix()->setMatrix(Matrixd::translate((localBeginningFreePosition+localEndFreePosition)/2));

            //Alignment
            if (lCorners[cornersSorted[k][0]].x() == lCorners[cornersSorted[k][1]].x() && lCorners[cornersSorted[k][1]].x() == lCorners[cornersSorted[k][2]].x() && lCorners[cornersSorted[k][2]].x() == lCorners[cornersSorted[k][3]].x())
            {
                if (lCorners[cornersSorted[k][0]].x() == bbox.xMin())
                    label->setAxisAlignment(osgText::TextBase::REVERSED_YZ_PLANE);
                else
                    label->setAxisAlignment(osgText::TextBase::YZ_PLANE);
            }
            else if (lCorners[cornersSorted[k][0]].y() ==lCorners[cornersSorted[k][1]].y() && lCorners[cornersSorted[k][1]].y() == lCorners[cornersSorted[k][2]].y() && lCorners[cornersSorted[k][2]].y() == lCorners[cornersSorted[k][3]].y())
            {
                if ( lCorners[cornersSorted[k][0]].y() == bbox.yMin())
                    label->setAxisAlignment(osgText::TextBase::XZ_PLANE);
            else
                label->setAxisAlignment(osgText::TextBase::REVERSED_XZ_PLANE);
            }
            else if (lCorners[cornersSorted[k][0]].z() == lCorners[cornersSorted[k][1]].z() && lCorners[cornersSorted[k][1]].z() == lCorners[cornersSorted[k][2]].z() && lCorners[cornersSorted[k][2]].z() == lCorners[cornersSorted[k][3]].z())
            {
                label->setAxisAlignment(osgText::TextBase::XY_PLANE);
            }
            else
            {
                cerr << "error with the faces definition of the bounding box an object linked to a label. The faces are not on (XY), (YZ) ot (XY) planes." << endl;
            }
            break;
            }
    }

    //Allow OSG to continue the node traversal
    traverse(node, nv);
}
