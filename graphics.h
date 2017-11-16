// $Id: graphics.h,v 1.1 2015-07-16 16:47:51-07 - - $

#ifndef __GRAPHICS_H__
#define __GRAPHICS_H__

#include <memory>
#include <vector>
using namespace std;

#include <GL/freeglut.h>

#include "rgbcolor.h"
#include "shape.h"

class object {
   private://public://private:
      shared_ptr<shape> pshape;
      vertex center;
      rgbcolor color;
   public:
      // Default copiers, movers, dtor all OK.
      object() {}
      object(shared_ptr<shape> p,vertex cen,rgbcolor col) 
            {pshape=p;center=cen;color=col;}
      void draw() { pshape->draw (GL_POLYGON,center, color); }
      void outline(rgbcolor outline,size_t thick){
         //draw the outline of a shape
         glLineWidth(GLfloat(thick)); 
         pshape->draw(GL_LINE_LOOP, center, outline);
      }
      void move (GLfloat delta_x, GLfloat delta_y) {
         center.xpos += delta_x;
         center.ypos += delta_y;
      }
      vertex getCenter(){return center;}
};

class mouse {
      friend class window;
   private:
      int xpos {0};
      int ypos {0};
      int entered {GLUT_LEFT};
      int left_state {GLUT_UP};
      int middle_state {GLUT_UP};
      int right_state {GLUT_UP};
   private:
      void set (int x, int y) { xpos = x; ypos = y; }
      void state (int button, int state);
      void draw();
};

class window {
      friend class mouse;
   private:
      static int width;         // in pixels
      static int height;        // in pixels
      static vector<object> objects;
      static size_t selected_obj;
      static mouse mus;
      static int movement;// in pixels, how much an object is moved by
      static rgbcolor outlineColor;//color of border on selected_obj
      static size_t thickness;//in pixels of above border 
   private:
      static void close();
      static void entry (int mouse_entered);
      static void display();
      static void reshape (int width, int height);
      static void keyboard (GLubyte key, int, int);
      static void special (int key, int, int);
      static void motion (int x, int y);
      static void passivemotion (int x, int y);
      static void mousefn (int button, int state, int x, int y);
      static void move_selected_object(int x, int y)
                //move the selected_obj
                {objects.at(selected_obj).move(x*movement,y*movement);}
      static void select_object(size_t index)
                        //change the selected object pointed at by 
                        //selected_obj
                        {if(index<objects.size()){selected_obj=index;}}
   public:
      static void push_back (const object& obj) {
                  objects.push_back (obj); }
      static void setwidth (int width_) { width = width_; }
      static void setheight (int height_) { height = height_; }
      //change the distance a shape covers when moved
      static void set_movement(int move_) {movement=move_;}
      //change the color and size of the border around the selected 
      //object
      static void set_border(rgbcolor outline_,int thickness_) 
                         {outlineColor=outline_;thickness=thickness_;}
      static int getWidth(){return width;}
      //static int getHeight()(return height;)
      static void main();
};

#endif

