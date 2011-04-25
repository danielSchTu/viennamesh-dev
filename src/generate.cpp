/* =============================================================================
   Copyright (c) 2011, Institute for Microelectronics, TU Wien
   http://www.iue.tuwien.ac.at
                             -----------------
                 ViennaMesh - The Vienna Mesh Generation Framework
                             -----------------

   authors:    Josef Weinbub                         weinbub@iue.tuwien.ac.at


   license:    see file LICENSE in the base directory
============================================================================= */

#include <iostream>
#include <vector>

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

#include "viennautils/config.hpp"
#include "viennautils/convert.hpp"
#include "viennautils/contio.hpp"
#include "viennautils/io.hpp"
#include "viennautils/file.hpp"

#include "viennagrid/domain.hpp"
#include "viennagrid/io/vtk_writer.hpp"
#include "viennagrid/io/vtk_reader.hpp"
#include "viennagrid/io/bnd_reader.hpp"

#include "vgmodeler/hull_generation.hpp"

#include "viennamesh/generator.hpp"
#include "viennamesh/wrapper.hpp"
#include "viennamesh/transfer/viennagrid.hpp"


namespace viennamesh {

namespace key {
std::string geometry = "geometry";
} // end namespace key

namespace query {

struct input
{
   template<typename ConfigT>
   static std::string type(ConfigT const& config)
   {
      return config.query("/input/type/key/text()");
   }
};


} // end namespace query
} // end namespace viennamesh



int main(int argc, char *argv[])
{
   if(argc != 4)
   {
      std::cerr << "## Error::Parameter - usage: " << argv[0] << " inputfile outputfile configfile" << std::endl;
      std::cerr << "## shutting down .." << std::endl;
      return -1;
   }
   
   std::string inputfile(argv[1]);
   std::string outputfile(argv[2]);
   
//   std::string::size_type pos = inputfile.rfind(".")+1;
//   std::string input_extension = inputfile.substr(pos, inputfile.size());
//   pos = outputfile.rfind(".")+1;   
//   std::string output_extension = outputfile.substr(pos, outputfile.size());

   std::string input_extension  = viennautils::file_extension(inputfile);
   std::string output_extension = viennautils::file_extension(outputfile);

   typedef viennautils::config<viennautils::tag::xml>::type    config_type;
   config_type config;
   std::ifstream configstream(argv[3]);
   config.read(configstream);
   configstream.close();

   if( input_extension == "vtu" )
   {
      typedef viennagrid::domain<viennagrid::config::line_2d>                  domain_type;
      domain_type domain;
      
      viennagrid::io::importVTK(domain, inputfile);
   }
   else
   if(input_extension == "bnd")
   {
      if(viennamesh::query::input::type(config) == viennamesh::key::geometry)
      {
         std::cout << "bnd geometry meshing .." << std::endl;
         
         typedef viennagrid::domain<viennagrid::config::triangular_3d>                  domain_type;
         domain_type domain;
         
         viennagrid::io::importBND(domain, inputfile);
         
//         viennamesh::bnd_reader bnd;
//         bnd.bnd2surface(inputfile);
         
         
         
         /*
         
         typedef  mesh_kernel
         
         typedef viennamesh::result_of::mesh_generator<viennamesh::tag::delink, bnd_wrapper_type>::type   mesh_generator_type;
         mesh_generator_type mesher(data_in);      
         
         mesher();         

         typedef viennagrid::domain<viennagrid::config::triangular_3d> domain_out_type;
         domain_out_type domain_out;      
         
         typedef viennamesh::transfer<viennamesh::tag::viennagrid>      transfer_type;
         transfer_type  transfer;
         transfer(mesher, domain_out);      
         
         viennagrid::io::Vtk_writer<domain_out_type> my_vtk_writer;
         my_vtk_writer.writeDomain(domain_out, outputfile);      
         */
      }
   }
   else
   if(input_extension == "hin")
   {
      viennautils::io::hin_reader my_hin_reader;
      my_hin_reader(inputfile);
   }
   else
   if(input_extension == "gau32")
   {
      typedef gsse::detail_topology::unstructured<2>                                unstructured_topology_32t; 
      typedef gsse::get_domain<unstructured_topology_32t, double, double,3>::type   domain_32t;
      domain_32t domain;
      domain.read_file(inputfile, false);
      
      typedef viennamesh::wrapper<viennamesh::tag::gsse01, domain_32t>     gsse01_wrapper_type;
      gsse01_wrapper_type data_in(domain);      
      
      typedef viennamesh::result_of::mesh_generator<viennamesh::tag::vgmodeler, gsse01_wrapper_type>::type   mesh_generator_type;
      mesh_generator_type mesher(data_in);      

      mesher( boost::fusion::make_map<viennamesh::tag::criteria, viennamesh::tag::size>(viennamesh::tag::conforming_delaunay(), 1.0) );         

      typedef viennagrid::domain<viennagrid::config::tetrahedral_3d> domain_out_type;
      domain_out_type domain_out;      
      
      typedef viennamesh::transfer<viennamesh::tag::viennagrid>      transfer_type;
      transfer_type  transfer;
      transfer(mesher, domain_out);      
      
      viennagrid::io::Vtk_writer<domain_out_type> my_vtk_writer;
      my_vtk_writer.writeDomain(domain_out, outputfile);      
   }
   else
   if(input_extension == "gts")
   {
      typedef viennagrid::domain<viennagrid::config::line_2d>                  domain_type;
      domain_type domain;
      
      viennautils::io::gts_reader my_gts_reader;
      my_gts_reader(domain, inputfile);      
      
      typedef viennamesh::wrapper<viennamesh::tag::viennagrid, domain_type>     vgrid_wrapper_type;
      vgrid_wrapper_type data_in(domain);      
      
      typedef viennamesh::result_of::mesh_generator<viennamesh::tag::triangle, vgrid_wrapper_type>::type   mesh_generator_type;
      mesh_generator_type mesher(data_in);      

      mesher( boost::fusion::make_map<viennamesh::tag::criteria, viennamesh::tag::size>(viennamesh::tag::conforming_delaunay(), 1.0) );         

      typedef viennagrid::domain<viennagrid::config::triangular_2d> domain_out_type;
      domain_out_type domain_out;      
      
      typedef viennamesh::transfer<viennamesh::tag::viennagrid>      transfer_type;
      transfer_type  transfer;
      transfer(mesher, domain_out);      
      
      viennagrid::io::Vtk_writer<domain_out_type> my_vtk_writer;
      my_vtk_writer.writeDomain(domain_out, outputfile);
   }
   else
   {
      std::cerr << "## input file format not supported: " << input_extension << std::endl;
      std::cerr << "## shutting down .. " << std::endl;
      return -1;   
   }

   return 0;
}



