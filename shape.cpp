// $Id: shape.cpp,v 1.1 2015-07-16 16:47:51-07 - - $

#include <cmath>
#include <typeinfo>
#include <unordered_map>
using namespace std;

#include "shape.h"
#include "util.h"

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

ostream& operator<< (ostream& out, const vertex& where) {
   out << "(" << where.xpos << "," << where.ypos << ")";
   return out;
}

shape::shape() {
   DEBUGF ('c', this);
}

text::text (void* glut_bitmap_font_, const string& textdata):
      glut_bitmap_font(glut_bitmap_font_), textdata(textdata) {
   DEBUGF ('c', this);
}

ellipse::ellipse (GLfloat width, GLfloat height):
dimension ({width, height}) {
   DEBUGF ('c', this);
}

circle::circle (GLfloat diameter): ellipse (diameter, diameter) {
   DEBUGF ('c', this);
}


polygon::polygon (const vertex_list& vertices): vertices(vertices) {
   DEBUGF ('c', this);
}

rectangle::rectangle (GLfloat width, GLfloat height):
            //populate polygon with vertices that correspond to a 
            //rectangle, based on width and height
            polygon({vertex{0,0},
                     vertex{width,0},
                     vertex{width,height},
                     vertex{0,height}}) {
   DEBUGF ('c', this << "(" << width << "," << height << ")");
}

square::square (GLfloat width): rectangle (width, width) {
   DEBUGF ('c', this);
}

diamond::diamond(GLfloat width,GLfloat height)://see rectangle ctor
          polygon({vertex{-1*width/2,0},
                   vertex{0,-1*height/2},
                   vertex{width/2,0},
                   vertex{0,height/2}}){
}

//a triangle is just a polygon with 3 vertices
triangle::triangle(const vertex_list& vertices):polygon(vertices){
}

right_triangle::right_triangle(GLfloat width):
   //basically a square missing a vertex
   triangle({vertex{0,0},vertex{width,0},vertex{width,width}}){
}

isosceles::isosceles(GLfloat width,GLfloat height):
        //create 3 vertices based on width and height
        triangle({vertex{0,0},vertex{width,0},vertex{width/2,height}}){
}

equilateral::equilateral(GLfloat equal):isosceles(equal,equal){
   //isosceles but all sides are equal
}

void text::draw (int kind, const vertex& center, 
                 const rgbcolor& color) const {
   //make text writable
   auto text=reinterpret_cast<const GLubyte*>(textdata.c_str());
   if(kind==GL_LINE_LOOP){//draw rectangle around text, since text 
                          //doesn't have a outline the way polygons do
      GLfloat x=center.xpos;//upper left
      GLfloat y=center.ypos;//corner
      GLfloat l=glutBitmapLength(glut_bitmap_font,text);//extends left
      GLfloat h=glutBitmapHeight(glut_bitmap_font);//extends right
      //create a rectangle as a polygon with 4 vertices
      polygon p ({vertex{x,y},//upper left corner
                  vertex{x,y-h},//lower left
                  vertex{x+l,y-h},//lower right
                  vertex{x+l,y}});//upper right
      p.draw(kind,vertex{x+l/2,y},color);//draw outline of rectangle
   }
   //write text in window
   else{
      glColor3ub(color.red,color.green,color.blue);//set colot
      float xpos=center.xpos;//find starting position of text
      float ypos=center.ypos;//upper corner of drawn object, NOT CENTER
      glRasterPos2f(xpos,ypos);
      glutBitmapString(glut_bitmap_font,text);//actually draw the text
      glutSwapBuffers();
   }
   DEBUGF ('d', this << "(" << center << "," << color << ")");
}

void ellipse::draw (int kind, const vertex& center, 
                    const rgbcolor& color) const {
   DEBUGF ('d', this << "(" << center << "," << color << ")");
   glBegin(kind);//begin drawing
   glEnable(GL_LINE_SMOOTH);
   glColor3ub(color.red,color.green,color.blue);//set color
   const float delta=2*M_PI/32;//constant for creating points on edge 
                               //of ellipse
                               //smaller constant = smoother edge
   float width=dimension.xpos;
   float height=dimension.ypos;//distance of edges from center
   /*if(kind==GL_LINE_LOOP)
        width+scale; height+scale*/
   for(float theta=0;theta<2*M_PI; theta+=delta){
      float xpos=width*cos(theta)+center.xpos;//point is x horizontal 
                                              //from center
      float ypos=height*sin(theta)+center.ypos;//point is y vertical
                                               //from center
      glVertex2f(xpos,ypos);//create point
   }
   glEnd();//finish drawing
}

void polygon::draw (int kind, const vertex& center, 
                    const rgbcolor& color) const {
   glBegin(kind);
   glEnable(GL_LINE_SMOOTH);
   glColor3ub(color.red,color.green,color.blue);//set color
   //adjust all the vertices in the polygon for the center given by 
   //user input
   float avg_width=0;
   float avg_height=0;
   for(auto& v:vertices){
      avg_width+=v.xpos;
      avg_height+=v.ypos;
   }
   avg_width/=vertices.size();
   avg_height/=vertices.size();
   //total all x coordinates and divide for average
   //same for y coordinates
   vertex_list vs;//store adjusted vertices in new vector
   for(auto& v:vertices){
      vs.push_back(vertex{v.xpos-avg_width,v.ypos-avg_height});
      //adjust every vertex by the average coordinate
   }
   for(auto& vertice:vs){//draw polygon
      glVertex2f(vertice.xpos+center.xpos,vertice.ypos+center.ypos);
   }
   glEnd();
   DEBUGF ('d', this << "(" << center << "," << color << ")");
}

void shape::show (ostream& out) const {
   out << this << "->" << demangle (*this) << ": ";
}

void text::show (ostream& out) const {
   shape::show (out);
   out << glut_bitmap_font << "(" << fontname[glut_bitmap_font]
       << ") \"" << textdata << "\"";
}

void ellipse::show (ostream& out) const {
   shape::show (out);
   out << "{" << dimension << "}";
}

void polygon::show (ostream& out) const {
   shape::show (out);
   out << "{" << vertices << "}";
}

ostream& operator<< (ostream& out, const shape& obj) {
   obj.show (out);
   return out;
}

