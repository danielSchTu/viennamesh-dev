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
#include "viennamesh/interfaces.hpp"            // all meshers

#include "viennagrid/domain.hpp"
#include "viennautils/io.hpp"

#include "viennagrid/io/vtk_writer.hpp"

#include "viennagrid/domain.hpp"

template <viennagrid::dim_type dim, 
          typename SegmentT>
void print_elements(SegmentT & seg)
{
  SegmentT const & const_seg = seg;

  typedef typename viennagrid::result_of::ncell_container<SegmentT, dim>::type  ContainerT;
  typedef typename viennagrid::result_of::iterator<ContainerT>::type           ContainerTIterator;
  
  ContainerT elements = viennagrid::ncells<dim>(seg);
  for (ContainerTIterator it = elements.begin();
       it != elements.end();
       ++it)
  {
/*    //std::cout << *it << std::endl; 
    it->print_short();*/
  }
}

template<typename DomainT>
void statistics(DomainT& domain, std::ostream& ostr = std::cout)
{
   typedef typename DomainT::config_type                                         domain_config_type;
   typedef typename domain_config_type::cell_tag                                 cell_tag_type;   
   
   static const int DIMG = domain_config_type::dimension_tag::value;
   static const int DIMT = cell_tag_type::topology_level;
   
   typedef viennagrid::segment_t<domain_config_type>                                         segment_type;
   typedef typename viennagrid::result_of::ncell_container<segment_type, DIMT>::type         cell_complex_type;
   typedef typename viennagrid::result_of::iterator<cell_complex_type>::type                 cell_iterator_type;   
   typedef typename viennagrid::result_of::ncell_type<domain_config_type, DIMT>::type        cell_type;
   typedef typename viennagrid::result_of::ncell_container<cell_type, 0>::type               vertex_on_cell_container_type;
   typedef typename viennagrid::result_of::iterator<vertex_on_cell_container_type>::type     vertex_on_cell_iterator_type;   
   
   size_t segment_size = domain.segment_container()->size();
   std::cout << "## Domain Statistics ##" << std::endl;
   std::cout << "Topology Dimension: " << DIMT << std::endl;
   std::cout << "Geometry Dimension: " << DIMG << std::endl;
   std::cout << "   segment size: " << segment_size << std::endl;
   
   for(size_t si = 0; si < segment_size; si++)
   {  
      std::cout << "   segment: " << si << std::endl;
      //std::cout << "     cell size: " << domain.segment(si).size<DIMT>() << std::endl;
      cell_complex_type cell_complex = viennagrid::ncells<DIMT>(domain.segment(si));
      std::cout << "      cells:" << std::endl;
      size_t cid = 0;
      for(cell_iterator_type cit = cell_complex.begin(); cit != cell_complex.end(); cit++)
      {
         std::cout << "         id: " << cid++ << " - vertices: ";
         vertex_on_cell_container_type vertex_on_cell_cont = viennagrid::ncells<0>(*cit);
         for(vertex_on_cell_iterator_type vocit = vertex_on_cell_cont.begin();
             vocit != vertex_on_cell_cont.end(); vocit++)
         {
            std::cout << vocit->getID() << " ";
         }         
         std::cout << std::endl;
      }
   }
//   std::cout << "Cells in Segment 0: "    << domain.segment(0).size<CellTag::topology_level>() << std::endl;
//   std::cout << "Cells in Segment 1: "    << domain.segment(1).size<CellTag::topology_level>() << std::endl;   
   
}

int main(int argc, char * argv[])
{
   if(argc != 4)
   {
      std::cerr << "## Error::Parameter - usage: " << argv[0] << " inputfile.{gts} outputfile.{vtu} dimension" << std::endl;
      std::cerr << "## shutting down .." << std::endl;
      return -1;
   }
   
   std::string inputfile(argv[1]);
   std::string outputfile(argv[2]);
   int dim(atoi(argv[3]));
   
   std::string::size_type pos = inputfile.rfind(".")+1;
   std::string input_extension = inputfile.substr(pos, inputfile.size());
   pos = outputfile.rfind(".")+1;   
   std::string output_extension = outputfile.substr(pos, outputfile.size());
   
   if(input_extension == "gts")
   {
      if(dim == 2)
      {
         typedef viennagrid::domain<viennagrid::config::line_2d>     domain_in_type;
         domain_in_type domain_in;
         std::cout << "# viennamesh::reading domain .. " << std::endl;
         viennautils::io::gts_reader   gtsread;
         gtsread(domain_in, inputfile, false);
         
         statistics(domain_in);
         
//          viennagrid::io::Vtk_writer<domain_in_type> my_vtk_writer;
//          my_vtk_writer.writeDomain(domain_in, "input.vtu");
         
         typedef viennagrid::domain<viennagrid::config::triangular_2d> domain_out_type;
         domain_out_type domain_out;

         std::cout << "# viennamesh::generating mesh .. " << std::endl;
          typedef viennamesh::mesh_generator<double> mesh_generator_type;
          mesh_generator_type mesher;
          mesher(domain_in);

         std::cout << "# viennamesh::building output domain .. " << std::endl;         
         viennamesh::transfer(mesher, domain_out);
         
         if(output_extension == "vtu")
         {
            std::cout << "# viennamesh::writing vtu file .. " << std::endl;
            viennagrid::io::Vtk_writer<domain_out_type> my_vtk_writer;
            my_vtk_writer.writeDomain(domain_out, outputfile);
         }
         else
         {
            std::cerr << "## Error: output fileformat not supported: " << output_extension << std::endl;
            std::cerr << "## shutting down .." << std::endl;     
            return -1;         
         }         
         
         // idea: provide different mesh generators, with different capabilities.
         // use our fold::property approach to get a subset of generators for a given property list.
         // provide different tools to classify/orient/improve quality of a mesh (extract the vgmodeler tools) 
         //
         // acc<dim>(property_map) = 2;
         // acc<method>(property_map) = constrained_delaunay;
         // acc<size>(property_map) = ....
         // a heterogenous mesher kernel environment canbe supported, the fold approach automatically 
         // computes the mesher kernels which fits the requested task
         //
         // typedef typename viennamesh::mesh_generator<property_map>::type  mesh_generator_type;
         // mesh_generator_type mesh_generator;
         // mesh_generator(domain_in);   // a functor, the same mesher can be applied on different input domains
         
         // typedef domain_generator<mesh_generator>  domain_generator_type;
         // domain_generator_type domain_generator(mesh_generator);
         // domain_generator(domain_out);

      }
      else
      {
         std::cerr << "## Error: dimension not supported: " << dim << std::endl;
         std::cerr << "## shutting down .." << std::endl;     
         return -1;         
      }
   }
   else
   {
      std::cerr << "## Error: input fileformat not supported: " << input_extension << std::endl;
      std::cerr << "## shutting down .." << std::endl;     
      return -1;
   }
   
   return 0;
}


