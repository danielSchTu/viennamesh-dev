#include "viennamesh/algorithm/seed_point_locator.hpp"
#include "viennamesh/algorithm/file_reader.hpp"
#include "viennamesh/algorithm/file_writer.hpp"


int main()
{
  typedef viennagrid::vertex_1d_mesh GeometryMeshType;
  // Typedefing vertex handle and point type for geometry creation
  typedef viennagrid::result_of::point<GeometryMeshType>::type PointType;
  typedef viennagrid::result_of::vertex_handle<GeometryMeshType>::type GeometryVertexHandle;

  // creating the geometry mesh
  viennamesh::result_of::parameter_handle< GeometryMeshType >::type geometry = viennamesh::make_parameter<GeometryMeshType>();

  double s = 10.0;
  viennagrid::make_vertex( geometry->get(), PointType(s) );
  viennagrid::make_vertex( geometry->get(), PointType(-s) );
  viennagrid::make_vertex( geometry->get(), PointType(0) );
  viennagrid::make_vertex( geometry->get(), PointType(0) );
  viennagrid::make_vertex( geometry->get(), PointType(0) );
  viennagrid::make_vertex( geometry->get(), PointType(2*s) );
  viennagrid::make_vertex( geometry->get(), PointType(3*s) );




  // creating the seed point locator algorithm
  viennamesh::algorithm_handle seed_point_locator( new viennamesh::seed_point_locator::algorithm() );


  seed_point_locator->set_input( "default", geometry );

  viennamesh::point_1d_container hole_points;
  hole_points.push_back( PointType(s/2) );
  seed_point_locator->set_input( "hole_points", hole_points );

  seed_point_locator->run();

  typedef viennamesh::result_of::point_container<PointType>::type PointContainerType;
  viennamesh::result_of::parameter_handle<PointContainerType>::type point_container = seed_point_locator->get_output<PointContainerType>( "default" );
  if (point_container)
  {
    std::cout << "Number of extracted seed points: " << point_container->get().size() << std::endl;
    for (PointContainerType::iterator it = point_container->get().begin(); it != point_container->get().end(); ++it)
      std::cout << "  " << *it << std::endl;
  }
}
