/* =============================================================================
   Copyright (c) 2011, Institute for Microelectronics, TU Wien
   http://www.iue.tuwien.ac.at
                             -----------------
                 ViennaMesh - The Vienna Mesh Generation Framework
                             -----------------

   authors:    Josef Weinbub                         weinbub@iue.tuwien.ac.at
               Karl Rupp                                rupp@iue.tuwien.ac.at


   license:    see file LICENSE in the base directory
============================================================================= */

#include <iostream>

#include "viennamesh/mesh.hpp"
#include "viennamesh/interfaces/triangle.hpp"   // a specific mesher

#include <boost/array.hpp>



int main(int argc, char * argv[])
{
   typedef boost::array< double, 2 >     point_type;
   point_type pnt1 = {{0,0}};
   point_type pnt2 = {{1,0}};   
   point_type pnt3 = {{1,1}};
   point_type pnt4 = {{0,1}};      
   
   typedef boost::array< std::size_t, 2 >    constraint_type;
   constraint_type c1 = {{0,1}};
   constraint_type c2 = {{1,2}};
   constraint_type c3 = {{2,3}};
   constraint_type c4 = {{3,0}};   

   typedef viennamesh::triangle<double>  mesher_type;
   mesher_type mesher;
   typedef viennamesh::add<mesher_type>  add_type;
   add_type add(mesher);
   
   add(pnt1);
   add(pnt2);
   add(pnt3);
   add(pnt4);   

   add(c1);
   add(c2);
   add(c3);
   add(c4);
   
   //add(viennamesh::methods::conforming_delaunay());
   
   mesher.start();
   
   return 0;
}
