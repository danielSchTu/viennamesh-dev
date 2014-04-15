#ifdef VIENNAMESH_WITH_TRIANGLE

#include "viennamesh/algorithm/triangle/triangle_mesh.hpp"
#include "viennamesh/algorithm/triangle/triangle_mesh_generator.hpp"
#include "viennagrid/algorithm/extract_seed_points.hpp"

namespace viennamesh
{
  namespace triangle
  {
    viennamesh::sizing_function_2d triangle_sizing_function;

    int should_triangle_be_refined(double * triorg, double * tridest, double * triapex, double)
    {
      REAL dxoa, dxda, dxod;
      REAL dyoa, dyda, dyod;
      REAL oalen, dalen, odlen;
      REAL maxlen;

      dxoa = triorg[0] - triapex[0];
      dyoa = triorg[1] - triapex[1];
      dxda = tridest[0] - triapex[0];
      dyda = tridest[1] - triapex[1];
      dxod = triorg[0] - tridest[0];
      dyod = triorg[1] - tridest[1];
      /* Find the squares of the lengths of the triangle's three edges. */
      oalen = dxoa * dxoa + dyoa * dyoa;
      dalen = dxda * dxda + dyda * dyda;
      odlen = dxod * dxod + dyod * dyod;
      /* Find the square of the length of the longest edge. */
      maxlen = (dalen > oalen) ? dalen : oalen;
      maxlen = (odlen > maxlen) ? odlen : maxlen;

      point_2d pt;
      pt[0] = (triorg[0] + tridest[0] + triapex[0]) / 3;
      pt[1] = (triorg[1] + tridest[1] + triapex[1]) / 3;

      viennagrid::static_array<double, 4> local_sizes;

      local_sizes[0] = triangle_sizing_function( point_2d(triorg[0], triorg[1]) );
      local_sizes[1] = triangle_sizing_function( point_2d(tridest[0], tridest[1]) );
      local_sizes[2] = triangle_sizing_function( point_2d(triapex[0], triapex[1]) );
      local_sizes[3] = triangle_sizing_function(pt);

      double local_size = -1;
      for (int i = 0; i < 4; ++i)
      {
        if (local_sizes[i] > 0)
        {
          if (local_size < 0)
            local_size = local_sizes[i];
          else
            local_size = std::min( local_size, local_sizes[i] );
        }
      }

      if (local_size > 0 && maxlen > local_size*local_size)
        return 1;
      else
        return 0;
    }



    void extract_seed_points( triangle::input_segmentation const & segmentation, int num_hole_points, REAL * hole_points, seed_point_2d_container & seed_points )
    {
      if (segmentation.segments.empty())
        return;

      LoggingStack stack( string("Extracting seed points from segments") );

      info(5) << "Extracting seed points from segments" << std::endl;

      string options = "zpQ";

      int highest_segment_id = -1;
      for (seed_point_2d_container::iterator spit = seed_points.begin(); spit != seed_points.end(); ++spit)
        highest_segment_id = std::max( highest_segment_id, spit->second );
      ++highest_segment_id;

      for ( std::list<triangle::input_mesh>::const_iterator sit = segmentation.segments.begin(); sit != segmentation.segments.end(); ++sit)
      {
        LoggingStack stack( string("Segment ") + lexical_cast<string>(highest_segment_id) );

        triangulateio tmp = sit->triangle_mesh;
        triangle::output_mesh tmp_mesh;

        if (hole_points)
        {
          tmp.numberofholes = num_hole_points;
          tmp.holelist = hole_points;
        }

        char * buffer = new char[options.length()];
        std::strcpy(buffer, options.c_str());


        triangulate( buffer, &tmp, &tmp_mesh.triangle_mesh, NULL);

        viennagrid::triangular_2d_mesh viennagrid_mesh;
        viennamesh::triangle::convert( tmp_mesh, viennagrid_mesh );



        std::vector<point_2d> local_seed_points;
        viennagrid::extract_seed_points( viennagrid_mesh, local_seed_points );
        for (unsigned int i = 0; i < local_seed_points.size(); ++i)
        {
          info(5) << "Found seed point: " << local_seed_points[i] << std::endl;
          seed_points.push_back( std::make_pair(local_seed_points[i], highest_segment_id) );
        }
        highest_segment_id++;

        tmp.holelist = NULL;
      }
    }




    mesh_generator::mesh_generator() :
      input_mesh(*this, "mesh"),
      input_seed_points(*this, "seed_points"),
      input_hole_points(*this, "hole_points"),
      sizing_function(*this, "sizing_function"),
      cell_size(*this, "cell_size"),
      min_angle(*this, "min_angle"),
      delaunay(*this, "delaunay", true),
      algorithm_type(*this, "algorithm_type"),
      extract_segment_seed_points(*this, "extract_segment_seed_points", true),
      option_string(*this, "option_string"),
      output_mesh(*this, "mesh") {}

    string mesh_generator::name() const { return "Triangle 1.6 mesher"; }


    bool mesh_generator::run_impl()
    {
      typedef triangle::output_mesh OutputMeshType;
      output_parameter_proxy<OutputMeshType> omp(output_mesh);

      std::ostringstream options;

      if (option_string.valid())
        options << option_string();
      else
        options << "zp";

      if (min_angle.valid())
        options << "q" << min_angle() / M_PI * 180.0;

      if (cell_size.valid())
        options << "a" << cell_size();

      if (delaunay())
        options << "D";

      if (algorithm_type.valid())
      {
        if (algorithm_type() == "incremental_delaunay")
          options << "i";
        else if (algorithm_type() == "sweepline")
          options << "F";
        else if (algorithm_type() == "devide_and_conquer")
        {}
        else
        {
          warning(5) << "Algorithm not recognized: '" << algorithm_type() << "' supported algorithms:" << std::endl;
          warning(5) << "  'incremental_delaunay'" << std::endl;
          warning(5) << "  'sweepline'" << std::endl;
          warning(5) << "  'devide_and_conquer'" << std::endl;
        }
      }


      triangulateio tmp = input_mesh().mesh.triangle_mesh;

      REAL * tmp_regionlist = NULL;
      REAL * tmp_holelist = NULL;


      seed_point_2d_container seed_points;

      if (input_seed_points.valid() && !input_seed_points().empty())
      {
        info(5) << "Found input seed points" << std::endl;
        seed_points = input_seed_points();
      }

      point_2d_container hole_points;

      if (input_hole_points.valid() && !input_hole_points().empty())
      {
        info(5) << "Found hole points" << std::endl;
        hole_points = input_hole_points();
      }

      if (!hole_points.empty())
      {
        tmp_holelist = (REAL*)malloc( 2*sizeof(REAL)*(tmp.numberofholes+hole_points.size()) );
        memcpy( tmp_holelist, tmp.holelist, 2*sizeof(REAL)*tmp.numberofholes );

        for (std::size_t i = 0; i < hole_points.size(); ++i)
        {
          tmp_holelist[2*(tmp.numberofholes+i)+0] = hole_points[i][0];
          tmp_holelist[2*(tmp.numberofholes+i)+1] = hole_points[i][1];
        }

        tmp.numberofholes += hole_points.size();
        tmp.holelist = tmp_holelist;
      }


      if (extract_segment_seed_points())
      {
        extract_seed_points( input_mesh().segmentation, tmp.numberofholes, tmp.holelist, seed_points );
      }

      if (!seed_points.empty())
      {
        tmp_regionlist = (REAL*)malloc( 4*sizeof(REAL)*(tmp.numberofregions+seed_points.size()) );
        memcpy( tmp_regionlist, tmp.regionlist, 4*sizeof(REAL)*tmp.numberofregions );

        for (unsigned int i = 0; i < seed_points.size(); ++i)
        {
          tmp_regionlist[4*(i+tmp.numberofregions)+0] = seed_points[i].first[0];
          tmp_regionlist[4*(i+tmp.numberofregions)+1] = seed_points[i].first[1];
          tmp_regionlist[4*(i+tmp.numberofregions)+2] = REAL(seed_points[i].second);
          tmp_regionlist[4*(i+tmp.numberofregions)+3] = 0;
        }

        tmp.numberofregions += seed_points.size();
        tmp.regionlist = tmp_regionlist;

        options << "A";
      }

      if (sizing_function.valid())
      {
        triangle_sizing_function = sizing_function();
        options << "u";
        ::should_triangle_be_refined = should_triangle_be_refined;
      }


      char * buffer = new char[options.str().length()];
      std::strcpy(buffer, options.str().c_str());

      std_capture().start();
      triangulate( buffer, &tmp, &omp().triangle_mesh, NULL);
      std_capture().start();

      delete[] buffer;
      if (!seed_points.empty())
        free(tmp_regionlist);
      if (!hole_points.empty())
        free(tmp_holelist);

      return true;
    }



  }
}

#endif
