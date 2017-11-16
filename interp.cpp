// $Id: interp.cpp,v 1.2 2015-07-16 16:57:30-07 - - $

#include <memory>
#include <string>
#include <vector>
using namespace std;

#include <GL/freeglut.h>

#include "debug.h"
#include "interp.h"
#include "shape.h"
#include "util.h"

unordered_map<string,interpreter::interpreterfn>
interpreter::interp_map {
   {"define" , &interpreter::do_define },
   {"draw"   , &interpreter::do_draw   },
   {"border" , &interpreter::set_border},
   {"moveby" , &interpreter::set_moveby},
};

unordered_map<string,interpreter::factoryfn>
interpreter::factory_map {
   {"text"          , &interpreter::make_text          },
   {"ellipse"       , &interpreter::make_ellipse       },
   {"circle"        , &interpreter::make_circle        },
   {"polygon"       , &interpreter::make_polygon       },
   {"rectangle"     , &interpreter::make_rectangle     },
   {"square"        , &interpreter::make_square        },
   {"diamond"       , &interpreter::make_diamond       },
   {"triangle"      , &interpreter::make_triangle      },
   {"right_triangle", &interpreter::make_right_triangle},
   {"isosceles"     , &interpreter::make_isosceles     },
   {"equilateral"   , &interpreter::make_equilateral   },
};

//added the font maps from shape.cpp to make creating text easier
static unordered_map<void*,string> fontname {
  {GLUT_BITMAP_8_BY_13       , "Fixed-8x13"    },
  {GLUT_BITMAP_9_BY_15       , "Fixed-9x15"    },
  {GLUT_BITMAP_HELVETICA_10  , "Helvetica-10"  },
  {GLUT_BITMAP_HELVETICA_12  , "Helvetica-12"  },
  {GLUT_BITMAP_HELVETICA_18  , "Helvetica-18"  },
  {GLUT_BITMAP_TIMES_ROMAN_10, "Times-Roman-10"},
  {GLUT_BITMAP_TIMES_ROMAN_24, "Times-Roman-24"},
};

static unordered_map<string,void*> fontcode {
  {"Fixed-8x13"    , GLUT_BITMAP_8_BY_13       },
  {"Fixed-9x15"    , GLUT_BITMAP_9_BY_15       },
  {"Helvetica-10"  , GLUT_BITMAP_HELVETICA_10  },
  {"Helvetica-12"  , GLUT_BITMAP_HELVETICA_12  },
  {"Helvetica-18"  , GLUT_BITMAP_HELVETICA_18  },
  {"Times-Roman-10", GLUT_BITMAP_TIMES_ROMAN_10},
  {"Times-Roman-24", GLUT_BITMAP_TIMES_ROMAN_24},
};

interpreter::shape_map interpreter::objmap;

interpreter::~interpreter() {
   for (const auto& itor: objmap) {
      cout << "objmap[" << itor.first << "] = "
           << *itor.second << endl;
   }
}

void interpreter::syntax_error(param begin, param end, int no){
   if(end-begin!=no){
      no+=2;
      throw runtime_error("syntax error: incorrect no of params");
   }
}

void interpreter::interpret (const parameters& params) {
   DEBUGF ('i', params);
   param begin = params.cbegin();
   string command = *begin;
   auto itor = interp_map.find (command);
   if (itor == interp_map.end()) throw runtime_error ("syntax error");
   interpreterfn func = itor->second;
   func (++begin, params.cend());
}

void interpreter::do_define (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   string name = *begin;
   objmap.emplace (name, make_shape (++begin, end));
}

void interpreter::do_draw (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   syntax_error(begin, end, 4);
   string name = begin[1];
   shape_map::const_iterator itor = objmap.find (name);
   if (itor == objmap.end()) {
      throw runtime_error (name + ": no such shape");
   }
   rgbcolor color {begin[0]};
   vertex where {from_string<GLfloat> (begin[2]),
                 from_string<GLfloat> (begin[3])};
   /*
 * remove this line
 * This will cause the shape to be draw even though there is no 
 * window
 * 
 * Instead, create an object that holds the coordinate, color and 
 * shape. Store that object in a vecotor objects (in graphics.cpp)
 * object {pshape, center, color}, then add this to objects (a 
 * vector of objects in the window class) So that the program 
 * remebers to draw it when window::display() is called
 */
   window::push_back(object(itor->second,where, color));
}

/*
 * a function for setting the color of the border generated around 
 * selected shapes
 */
void interpreter::set_border(param begin,param end){
   syntax_error(begin, end, 2);   
   rgbcolor color(*begin++);//create color
   window::set_border(color,stoi(*begin++));//pass to window
}

/*
 * a method to change the number of pixels by which a shape moves
 */
void interpreter::set_moveby(param begin,param end){
   syntax_error(begin, end, 1);
   window::set_movement(stoi(*begin++));
}

shape_ptr interpreter::make_shape (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   string type = *begin++;
   auto itor = factory_map.find(type);
   if (itor == factory_map.end()) {
      throw runtime_error (type + ": no such shape");
   }
   factoryfn func = itor->second;
   return func (begin, end);
}

shape_ptr interpreter::make_text (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   if(!(end-begin>2)) throw runtime_error("syntax error");//we can't use
                                                       //syntax_error()
                                                       //here b/c we
                                                       //need the args
                                                       //>2
   const auto& entry=fontcode.find(*begin++);//using the font maps
                                             //(taken from shape.cpp)
                                             //find the font that
                                             //corresponds to user 
                                             //input
   void* font=entry->second;
   string txt="";
   while(begin!=end)//compound the remaining params into a string
      txt+=*begin++ + " ";
   txt=txt.substr(0,txt.size()-1);//chop off the last space
   return make_shared<text> (font, txt);//pass to ctor
}

shape_ptr interpreter::make_ellipse (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   syntax_error(begin,end,2);
   GLfloat width=from_string<GLfloat>(*begin++);//convert first param
                                                //to float and advance
                                                //itor
   return make_shared<ellipse> (width,from_string<GLfloat>(*begin++));
}

shape_ptr interpreter::make_circle (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   syntax_error(begin,end,1);
   //only need to pass one param
   return make_shared<circle> (from_string<GLfloat>(*begin++));
}

//pair off params from begin to end into vertices, which are stored in 
//a vector and returned
vertex_list interpreter::make_vertices(param begin,param end){
   vertex_list vertices;
   while(begin!=end){
      GLfloat x=from_string<GLfloat>(*begin++);
      GLfloat y=from_string<GLfloat>(*begin++);
      vertices.push_back(vertex{x,y});
   }
   return vertices;
}

shape_ptr interpreter::make_polygon (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   if(!(end-begin>2)&&((end-begin)%2==0))//if we don't have an even no
                                         //of params, or at least 2
                                         //params, then we can't make
                                         //a vertex list
      throw runtime_error("syntax error");
   vertex_list vertices=make_vertices(begin,end);//just make the params
                                                 //into a vertex list
   return make_shared<polygon> (vertices);//a construct
}

/*
 * All of the remaining constructors follow the same format of
 * the above constructors
 *  - either creating a vertex_list and passing that to the constructor 
 * (like polygon)
 *  - passing a width and a height (like ellipse)
 *  - passing only a width (from which a height will be derived) 
 * (like circle)
 */

shape_ptr interpreter::make_rectangle (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   syntax_error(begin,end,2);
   GLfloat width=from_string<GLfloat>(*begin++);
   return make_shared<rectangle>(width,from_string<GLfloat>(*begin++));
}

shape_ptr interpreter::make_square (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   syntax_error(begin,end,1);
   return make_shared<square> (from_string<GLfloat>(*begin++));
}

shape_ptr interpreter::make_diamond(param begin,param end){
   syntax_error(begin,end,2);
   GLfloat width=from_string<GLfloat>(*begin++);
   return make_shared<diamond>(width,from_string<GLfloat>(*begin++));
}

shape_ptr interpreter::make_triangle(param begin,param end){
   syntax_error(begin,end,6);
   vertex_list vertices=make_vertices(begin,end);
   return make_shared<triangle>(vertices);
}

shape_ptr interpreter::make_right_triangle(param begin,param end){
   syntax_error(begin,end,1);
   return make_shared<right_triangle>(from_string<GLfloat>(*begin++));
}

shape_ptr interpreter::make_isosceles(param begin,param end){
   syntax_error(begin,end,2);
   GLfloat width=from_string<GLfloat>(*begin++);
   return make_shared<isosceles>(width,from_string<GLfloat>(*begin++));
}

shape_ptr interpreter::make_equilateral(param begin,param end){
   syntax_error(begin,end,1);
   return make_shared<equilateral>(from_string<GLfloat>(*begin++));
}

