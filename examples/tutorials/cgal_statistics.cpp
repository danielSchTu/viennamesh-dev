#include "viennameshpp/core.hpp"
#include <boost/algorithm/string.hpp>  
#include <fstream>
#include <iostream>

#define Debug true


#define help_text "This Progam makes a CSV File which that has statistic information about a cgal reduces mesh from 1% to 99%.\n\
input parameters:\n\
\t -input_mesh, -im                     [mesh location              : default ./statistic/exp_conf.vtu ]\n\
\n\
\t -volume_weight, -vw                  [volume weight factor       : default 0.1 ]\n\
\t -boundary_weight, -bw                [boundary weight factor     : default 0.5 ]\n\
\t -shape_weight, -sw                   [shape weight factor        : default 0.4 ]\n\
\t -feature_angle, -fa                  [sets the feature angle     : default not used ]\n\
\t -placement_policy, -pp               [sets the placement policy  : default lindstrom-turk ]\n\
\t -cost_policy, -cp                    [sets the cost policy       : default edgelength ]\n\
\n\
\t -alpha, -a                           [sets alpha                 : default 0.18 ]\n\
\t -beta, -b                            [stes beta                  : default 8.82 ]\n\
\t -gamma, -g                           [stes gamma                 : default 0.88 ]\n\
\t -delta, -d                           [sets delta                 : default 0.7  ]\n\
\n\
\t -enable_output_mesh, -eom            [true:false should outputmeshes be produced? ]\n\
\t -output_file, -of                    [overrides the outputfile location/name a .csv / .vtu  ending is added ]\n\
\n\
\t -ratio_step, -rstep                  [ratio step length ]\n\
\t -ratio_start, -rstart                [ratio start counter ]\n\
\t -ratio_stop, -rstop                  [ratio stop counter ]\n\
\t -help                                if help is one of the arguments nothing is produced\n\
"

/*
main objective is to test all possible stop-ratios of a given mesh or default a default mesh

can get inputs in the for: argument value argument value ...

the following line had to be added in make_statistic 
after line 244: set_output( "number_of_cells", viennagrid_numeric(statistic.count()));
*/

int main(int argc, char **argv)
{
#if Debug == true
    std::cout << "start of the programm\n";
#endif

    /*Default values*/
    /*input handling strings*/
    std::vector<std::string>            input_strings;
    std::vector<std::string>            input_values;

    /*output files*/
    std::string meshPath("./statistic/exp_conf.vtu");
    std::string output_CSV_file;
    std::string output_mesh_file;
    bool override_outputfile = false;

    
    /*cgal simplyfy mesh values*/
    std::string cost_policy             = "edgelength";
    std::string placement_policy        = "lindstrom-turk";
    double lindstrom_volume_weight      = 0.1;
    double lindstrom_boundary_weight    = 0.5; 
    double lindstrom_shape_weight       = 0.4; 
    double feature_angle                     ;
    bool   use_lindstrom_volume_weight  = true;
    bool   use_lindstrom_boundary_weight= true;
    bool   use_lindstrom_shape_weight   = true;
    bool   use_feature_angle            = false;

    /* Default Statistic Values */
    double alpha                        = 0.18;
    double beta                         = 8.82;
    double gamma                        = 0.88;
    double delta                        = 0.7;
    std::string metric_type             = "radius_ratio";

    /* Set up the variables for the data handling*/     // muss noch schöner geschrieben werden
    viennamesh::algorithm_handle        coarser;
    viennamesh::algorithm_handle        stats;
    viennamesh::algorithm_handle        mesh_writer;
    viennamesh::context_handle          context;
    bool enable_output_mesh             = true;

        
    /*zu berechnende*/
    viennagrid_numeric  count                                   = 0,
                        Triangle_shape_quality                  = 0,
                        Geometric_distance_to_the_original_mesh = 0,
                        Dierences_in_curvature                  = 0,
                        Surface_area_deviation                  = 0;

    
    double ratio                        = 0;
    double ratio_step                   = 0.01;
    double ratio_start                  = 0.01;
    double ratio_stop                   = 0.99;

    bool status=true;

/*debug text*/
#if Debug == true
    std::cout << "finished initializing variabes\n";
#endif


    /*Get possible extra input*/
    /* 0 is the name of the program */
    for(int count=1;count<argc;count+=2)
    {
        std::string tmp(argv[count]);
        boost::algorithm::to_lower(tmp);
        input_strings.push_back(tmp);
    }
    for(int count=2;count<argc;count+=2)
    {
        input_values.push_back(std::string(argv[count]));
    }
#if Debug == true
    for(int i=0;i<input_strings.size();i++)
        std::cout << "type: |" << input_strings [i] << "|\n" ;
#endif

    for(int count=0;count<input_strings.size();count++)
    {
        /*input mesh*/
        if(input_strings[count].compare("-input_mesh")==0 || input_strings[count].compare("-im")==0)
        {
            meshPath=input_values[count];
        }
        /*cgal algorythm values*/
        if(input_strings[count].compare("-volume_weight")==0 || input_strings[count].compare("-vw")==0)
        {
            lindstrom_volume_weight=std::stod(input_values[count]);
            use_lindstrom_volume_weight  = true;
        }
        if(input_strings[count].compare("-boundary_weight")==0 || input_strings[count].compare("-bw")==0)
        {
            lindstrom_boundary_weight=std::stod(input_values[count]);
            use_lindstrom_boundary_weight= true;
        }
        if(input_strings[count].compare("-shape_weight")==0 || input_strings[count].compare("-sw")==0)
        {
            lindstrom_shape_weight=std::stod(input_values[count]);
            use_lindstrom_shape_weight   = true;
        }
        if(input_strings[count].compare("-feature_angle")==0 || input_strings[count].compare("-fa")==0)
        {
            feature_angle=std::stod(input_values[count]);
            use_feature_angle            = true;
        }
        if(input_strings[count].compare("-cost_policy")==0 || input_strings[count].compare("-cp")==0)
        {
            cost_policy=input_values[count];
        }
        if(input_strings[count].compare("-placement_policy")==0 || input_strings[count].compare("-pp")==0)
        {
            placement_policy=input_values[count];
        }
        /* sets Statistic values */
        if(input_strings[count].compare("-alpha")==0 || input_strings[count].compare("-a")==0)
        {
            alpha=std::stod(input_values[count]);
        }
        if(input_strings[count].compare("-beta")==0 || input_strings[count].compare("-b")==0)
        {
            beta=std::stod(input_values[count]);
        }
        if(input_strings[count].compare("-gamma")==0 || input_strings[count].compare("-g")==0)
        {
            gamma=std::stod(input_values[count]);
        }
        if(input_strings[count].compare("-delta")==0 || input_strings[count].compare("-d")==0)
        {
            delta=std::stod(input_values[count]);
        }
        /* output mesh */
        if(input_strings[count].compare("-enable_output_mesh")==0 || input_strings[count].compare("-eom")==0)
        {
            enable_output_mesh=(input_values[count].compare("true")==0)? true:false;
        }
        /*output file*/
        if(input_strings[count].compare("-output_file")==0 || input_strings[count].compare("-of")==0)
        {
            output_CSV_file=input_values[count];
            output_mesh_file=input_values[count];
            override_outputfile=true;
            std::cout << "override of output accepted\n";
        }
        /*reduction ratio */
        if(input_strings[count].compare("-ratio_step")==0 || input_strings[count].compare("-rstep")==0)
        {
            ratio_step=std::stod(input_values[count]);
        }
        if(input_strings[count].compare("-ratio_start")==0 || input_strings[count].compare("-rstart")==0)
        {
            double tmp=std::stod(input_values[count]);
            ratio_start=tmp>0? tmp:0.01;
        }
        if(input_strings[count].compare("-ratio_stop")==0 || input_strings[count].compare("-rstop")==0)
        {
            double tmp=std::stod(input_values[count]);
            ratio_stop=tmp<1? tmp:0.99;
        }


        /*help text*/
        if(input_strings[count].compare("-help")==0)
        {
            std::cout << help_text ;
            exit(0);
        }
    }


#if Debug == true
    std::cout << "finished reading all input variables\n";
#endif

   
    /* Read mesh*/
    viennamesh::algorithm_handle mesh_reader = context.make_algorithm("mesh_reader");
    mesh_reader.set_input( "filename", meshPath );
    status=mesh_reader.run();
    //rauch noch schönere error behandlung
    if(!status)
    {
        std::cout << "Error at reading file";
        exit(1);
    }

#if Debug == true
    std::cout << "finished loading output file\n";
#endif


    
    /*statistic to get the original count*/
    stats = context.make_algorithm("make_statistic");
    stats.set_input("mesh",  mesh_reader.get_output("mesh"));
    /*default Values*/
    stats.set_input("alpha",        alpha);
    stats.set_input("beta",         beta);
    stats.set_input("gamma",        gamma);
    stats.set_input("delta",        delta);
    stats.set_input("metric_type",  metric_type);
    stats.run();
       
    viennagrid_numeric orig_count;
    orig_count = stats.get_output<viennagrid_numeric>("number_of_cells")();
    


     /*output File*/
    if(override_outputfile==false)
    {
        output_CSV_file=meshPath.substr(0,meshPath.find_last_of('.')) + "_statisic";   
        std::cout << output_CSV_file;                       //sollte noch zu info geäändert werden
        output_mesh_file=meshPath.substr(0,meshPath.find_last_of('.'));
    }
    std::ofstream output_file;
    output_file.open(output_CSV_file + ".csv");

    /* set the first line to know what what is */
    output_file << "Input Mesh,Elementzahl,Stop Ratio,Output ElementZahl,Metrik 1(T),Metrik 2(H),Metrik 3(C),Metrik(A)\n"; 


    
    

    for(ratio=ratio_start;ratio<=ratio_stop;ratio+=ratio_step)
    {
        /* Set up the algorithm*/
        coarser = context.make_algorithm("cgal_mesh_simplification");
        coarser.set_default_source(mesh_reader);
        coarser.set_input("stop_predicate", "ratio");
        coarser.set_input("ratio",           ratio);

        coarser.set_input("cost_policy",         cost_policy );
        coarser.set_input("placement_policy",    placement_policy );

        if(use_lindstrom_volume_weight)     coarser.set_input("lindstrom_volume_weight",    lindstrom_volume_weight);    
        if(use_lindstrom_boundary_weight)   coarser.set_input("lindstrom_boundary_weight",  lindstrom_boundary_weight);  
        if(use_lindstrom_shape_weight)      coarser.set_input("lindstrom_shape_weight",     lindstrom_shape_weight);     
        if(use_feature_angle)               coarser.set_input("feature_angle",              feature_angle);

        coarser.run();

        /*Make the statistic*/
        stats = context.make_algorithm("make_statistic");
        /*get both meshes*/
        stats.set_input("original_mesh",mesh_reader.get_output("mesh"));
        stats.set_input("mesh",         coarser.get_output("mesh"));
        /*default Values*/
        stats.set_input("alpha",        alpha);
        stats.set_input("beta",         beta);
        stats.set_input("gamma",        gamma);
        stats.set_input("delta",        delta);
        stats.set_input("metric_type",  metric_type);

        stats.run(); 
        /*write statistic*/
        count                                   = stats.get_output<viennagrid_numeric>("number_of_cells")();
        Triangle_shape_quality                  = stats.get_output<viennagrid_numeric>("triangle_shape")();
        Geometric_distance_to_the_original_mesh = stats.get_output<viennagrid_numeric>("minimum_distance_rms")();
        Dierences_in_curvature                  = stats.get_output<viennagrid_numeric>("mean_curvature_difference")();
        Surface_area_deviation                  = stats.get_output<viennagrid_numeric>("area_deviation")();
       
       output_file << meshPath << "," << orig_count << "," << ratio << "," << count << "," << Triangle_shape_quality << "," 
            << Geometric_distance_to_the_original_mesh << "," << Dierences_in_curvature << "," << Surface_area_deviation << "\n";

        if(enable_output_mesh)
        {
            /*make output mesh file*/
            mesh_writer = context.make_algorithm("mesh_writer");
            mesh_writer.set_default_source(coarser);
            mesh_writer.set_input( "filename", output_mesh_file + "_ratio_" +  std::to_string(ratio) + ".vtu");
            mesh_writer.run();
        }

    }
    std::cout << "finished now only closing csv file \n";
    
    output_file.close();
    
    
    return 1;
}