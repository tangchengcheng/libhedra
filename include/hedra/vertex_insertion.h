// This file is part of libhedra, a library for polyhedral mesh processing
//
// Copyright (C) 2017 Amir Vaxman <avaxman@gmail.com>
//
// This Source Code Form is subject to the terms of the Mozilla Public License
// v. 2.0. If a copy of the MPL was not distributed with this file, You can
// obtain one at http://mozilla.org/MPL/2.0/.
#ifndef HEDRA_VERTEX_INSERTION_H
#define HEDRA_VERTEX_INSERTION_H
#include <igl/igl_inline.h>
#include <hedra/polygonal_face_centers.h>
#include <Eigen/Core>
#include <string>
#include <vector>
#include <cstdio>

namespace hedra
{
    // returns a mesh after vertex insertion, which is basically vertex insertion in the barycenter of each face, connected with all midedges
    // Inputs:
    //  V  eigen double matrix     #V by 3 - vertex coordinates
    //  D  eigen int vector        #F by 1 - face degrees
    //  F  eigen int matrix        #F by max(D) - vertex indices in face
    //  FE eign int matrix         #F by max(D) - edges by order in face
    
    // Outputs:
    //  newV  eigen double matrix  new vertices
    //  newD  eigen int vector    new valences
    //  newF eigen int matrix     new faces
    IGL_INLINE bool vertex_insertion(const Eigen::MatrixXd& V,
                                    const Eigen::VectorXi& D,
                                    const Eigen::MatrixXi& F,
                                     const Eigen::MatrixXd& EV,
                                    const Eigen::MatrixXi& FE,
                                    Eigen::MatrixXd& newV,
                                    Eigen::VectorXi& newD,
                                    Eigen::VectorXi& newF)
    {
        using namespace Eigen;
        MatrixXd faceCenters;
        polygonal_face_centers(V,D,F,faceCenters);
        
        MatrixXd midEdges(EV.rows(),3);
        for (int i=0;i<EV.rows();i++)
            midEdges.row(i)<<(V.row(EV(i,0))+V.row(EV(i,1)))/2.0;
        
        
        newV.resize(V.rows()+D.rows()+FE.rows(),3);
        newV.block(0,0,V.rows(),3)=V;
        newV.block(V.rows(),0,F.rows(),3)=faceCenters;
        newV.block(V.rows()+F.rows(),0,FE.rows(),3)=midEdges;
        
        //the mesh is basicaly sum(D) quads (D per face)
        newD=VectorXi::Constant(D.sum(),4);
        newF.resize(D.sum(),4);
        
        int currNewFace=0;
        for (int i=0;i<D.rows();i++)
            for (int j=0;j<D(i);j++)
                newF.row(currNewFace++)<<V.rows()+F.rows()+FE(i,j), F(i,(j+1)%D(i)),V.rows()+F.rows()+FE(i,(j+1)%D(i)), V.rows()+i;
        
        return true;
    }
}


#endif


