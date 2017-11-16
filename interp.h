// $Id: interp.h,v 1.1 2015-07-16 16:47:51-07 - - $

#ifndef __INTERP_H__
#define __INTERP_H__

#include <iostream>
#include <unordered_map>
#include <vector>
using namespace std;

#include "debug.h"
#include "graphics.h"
#include "shape.h"

class interpreter {
   public:
      using shape_map = unordered_map<string,shape_ptr>;
      using parameters = vector<string>;
      using param = parameters::const_iterator;
      using range = pair<param,param>;
      void interpret (const parameters&);
      interpreter() {}
      ~interpreter();

   private:
      interpreter (const interpreter&) = delete;
      interpreter& operator= (const interpreter&) = delete;

      using interpreterfn = void (*) (param, param);
      using factoryfn = shape_ptr (*) (param, param);

      static unordered_map<string,interpreterfn> interp_map;
      static unordered_map<string,factoryfn> factory_map;
      static shape_map objmap;

      static void do_define (param begin, param end);
      static void do_draw (param begin, param end);
      //sends input over to window to set the border color & thickness
      static void set_border(param begin, param end);
      //sends input over to window to set the movement distance
      static void set_moveby(param begin, param end);

      //helper methods
      
      //makes printing syntax errors easier, good for checking if user 
      //input incorrect number of parameters
      static void syntax_error(param begin, param end, int no);
      //converts params begin->end into pairs of vertices in a list
      static vertex_list make_vertices(param begin,param end);

      static shape_ptr make_shape (param begin, param end);
      static shape_ptr make_text (param begin, param end);
      static shape_ptr make_ellipse (param begin, param end);
      static shape_ptr make_circle (param begin, param end);
      static shape_ptr make_polygon (param begin, param end);
      static shape_ptr make_rectangle (param begin, param end);
      static shape_ptr make_square (param begin, param end);
      static shape_ptr make_line (param begin, param end);
      //self-explanatory: 
      //create new shapes
      static shape_ptr make_diamond (param begin,param end);
      static shape_ptr make_triangle (param begin,param end);
      static shape_ptr make_right_triangle (param begin,param end);
      static shape_ptr make_isosceles (param begin,param end);
      static shape_ptr make_equilateral (param begin,param end);
};

#endif

