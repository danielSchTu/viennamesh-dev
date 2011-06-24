/* =============================================================================
   Copyright (c) 2011, Institute for Microelectronics, TU Wien
   http://www.iue.tuwien.ac.at
                             -----------------
                 ViennaMesh - The Vienna Mesh Generation Framework
                             -----------------

   authors:    Josef Weinbub                         weinbub@iue.tuwien.ac.at


   license:    see file LICENSE in the base directory
============================================================================= */

#ifndef VIENNAMESH_ADAPTORS_HULLQUALITY_HPP
#define VIENNAMESH_ADAPTORS_HULLQUALITY_HPP

// *** vienna includes
#include "viennamesh/adaptation/base.hpp"
#include "viennamesh/tags.hpp"
#include "viennagrid/domain.hpp"
#include "viennagrid/config/simplex.hpp"

// *** boost includes
#include <boost/shared_ptr.hpp>

namespace viennamesh {

template <>
struct mesh_adaptor <viennamesh::tag::hull_quality>
{

   typedef viennagrid::domain<viennagrid::config::triangular_3d>     domain_type;
   typedef boost::shared_ptr< domain_type >                          input_type;
   typedef input_type                                                result_type;

   typedef domain_type::config_type                     DomainConfiguration;

   typedef DomainConfiguration::numeric_type            CoordType;
   typedef DomainConfiguration::dimension_tag           DimensionTag;
   typedef DomainConfiguration::cell_tag                CellTag;

   typedef domain_type::segment_type                                                                  SegmentType;
   typedef viennagrid::result_of::ncell_type<DomainConfiguration, CellTag::topology_level>::type      CellType;   
   typedef viennagrid::result_of::ncell_container<domain_type, 0>::type                               GeometryContainer;      
   typedef viennagrid::result_of::iterator<GeometryContainer>::type                                   GeometryIterator;         
   typedef viennagrid::result_of::ncell_container<SegmentType, CellTag::topology_level>::type         CellContainer;      
   typedef viennagrid::result_of::iterator<CellContainer>::type                                       CellIterator;         
   typedef viennagrid::result_of::ncell_container<CellType, 0>::type                                  VertexOnCellContainer;
   typedef viennagrid::result_of::iterator<VertexOnCellContainer>::type                               VertexOnCellIterator;         
   typedef viennagrid::result_of::point_type<DomainConfiguration>::type                               PointType;   

   static const int DIMT = DomainConfiguration::cell_tag::topology_level;   
   static const int CELLSIZE = DIMT+1;      

   // --------------------------------------------------------------------------         
   mesh_adaptor();
  ~mesh_adaptor();
   // --------------------------------------------------------------------------     
   result_type operator()(input_type domain);
   // --------------------------------------------------------------------------     
private:
   // --------------------------------------------------------------------------         
   std::string id;
   // --------------------------------------------------------------------------     
};

} // end namespace viennamesh

#endif


