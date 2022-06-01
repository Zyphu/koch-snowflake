// Koch Snowflake 
// By Daniel Cumming

// ABOUT
// -------------------------------------------------------------------------------------------------
// The following program computes and generates a koch snowflake or anti-snowflake of a given size.
// It asks the user for the general size of the image, number of iterations to perform and the type.

// REQUIREMENTS
// - OpenGl
// - FreeGlut

#include <GL/glut.h>
#include <iostream>
#include <cmath>
#include <vector>

using namespace std;

int WINDOW_SIZE = 600; 
const int VIEWPORT_SIZE = 5;

struct coord { // Helpful data structure for points in xy-plane
    GLfloat x, y;
};

// Global Variables
int koch_itr = 10;
bool koch_anti = false;
coord user_start_point{-4.25,-2.5};
coord user_end_point{4.25,-2.5};

int get_uint(const string& greeting, const string& sorry){ // Reads a positive int from cin with given prompts
    cout << greeting;
    int i = 0;
    while(true){
        if(cin>>i && i > 0){
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // discards all remaining input that is not a number
            return i;
        } 
        cin.clear(); // clears error state
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // discards all input
        cout << sorry;
    }
}

float get_float(const string& greeting, const string& sorry){ // Reads a float from cin with given prompts
    cout << greeting;
    float f = 0;
    while(true){
        if(cin>>f){
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // discards all remaining input that is not a number
            return f;
        } 
        cout << sorry;
        cin.clear(); // clears error state
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // discards all input
    }
}

coord get_coord(const string& greeting){ // Obtains point coordinates from user
    cout << greeting;
    float x = get_float("Enter the x-coordinate: ", "Invalid input, enter a number: ");
    float y = get_float("Enter the y-coordinate: ", "Invalid input, enter a number: ");
    return coord{x,y};
}

coord get_delta(coord start_point, coord end_point){ // Returns difference between points
    return coord {end_point.x - start_point.x, end_point.y - start_point.y};
}

coord get_triangle(coord start_point, coord end_point){ // Returns point to complete an equilateral triangle in a counterclockwise trip
    coord delta = get_delta(start_point, end_point);
    float x = start_point.x + delta.x*cosf(M_PI/3) - delta.y*sinf(M_PI/3); // Counterclockwise Rotation Matrix
    float y = start_point.y + delta.x*sinf(M_PI/3) + delta.y*cosf(M_PI/3);
    return coord{x,y};
}

void set_colour (coord point, bool anti){
    float r,g,b;
    r = abs(point.x - VIEWPORT_SIZE) / (2*VIEWPORT_SIZE);
    g = abs(point.y + VIEWPORT_SIZE) / (2*VIEWPORT_SIZE);   
    b = abs(point.x + VIEWPORT_SIZE) / (2*VIEWPORT_SIZE); 
    glColor3f(r,g,b);
}

void welcome(){ // Prompts user for necessary information
    cout << "Welcome, together we will create a Koch Snowflake fractal. But we need your help first.\n";
    WINDOW_SIZE = get_uint("Enter the pixel lenght of your snowflake: ", "Invalid input, please enter a positive integer: ");

    koch_itr = get_uint("Enter the number of iterations you wish to perform: ", "Invalid input, please enter a positive integer: ");
    
    int koch_type = get_uint("1) Snowflake\n2) Anti-Snowflake\nSelect Koch Type (1 or 2): ", "Invalid input, please enter a positive integer: ");
    koch_anti = (koch_type > 1) ? true : false;
}

void koch_iteration(coord start_point, coord end_point, int itr, bool anti){ // Computes and draws the lines of a koch snowflake for a given iteration and type
    coord delta = get_delta(start_point, end_point);
    float delta_length = sqrt(pow(delta.x,2) + pow(delta.y,2));
    if(itr > 0 && delta_length >= (float) VIEWPORT_SIZE/WINDOW_SIZE){  // limits compute to valid iteration and visible portions  
        coord triangle_start {start_point.x + delta.x/3, start_point.y + delta.y/3};
        coord triangle_end {start_point.x + delta.x*2/3, start_point.y + delta.y*2/3};
        coord triangle_altitude = (anti) ? get_triangle(triangle_start, triangle_end) : get_triangle(triangle_end, triangle_start); // computes triangle based on snowflake type
        koch_iteration(start_point, triangle_start, itr-1, anti);       // iterates on line segments
        koch_iteration(triangle_start, triangle_altitude, itr-1, anti);
        koch_iteration(triangle_altitude, triangle_end, itr-1, anti);
        koch_iteration(triangle_end, end_point, itr-1, anti);
    } 
    else { // Draw lines
        glBegin(GL_LINES);
            set_colour(start_point,anti); glVertex2f(start_point.x, start_point.y);
            set_colour(end_point, anti);  glVertex2f(end_point.x, end_point.y);            
        glEnd();
    }
}

void koch_snowflake(){ //Draws a koch snowflake
    if (koch_itr > 0){
        cout << "\n Computing snowflake ... ";
        coord triangle_altitude = get_triangle(user_start_point, user_end_point);
        koch_iteration(user_start_point,  user_end_point,    koch_itr-1, false);
        koch_iteration(user_end_point,    triangle_altitude, koch_itr-1, false);
        koch_iteration(triangle_altitude, user_start_point,  koch_itr-1, false);
        cout << "Done\n";
    }        
}

void koch_antisnowflake(){ // Draws a koch-antisnowflake
    if (koch_itr > 0){
        cout << "\n Computing anti-snowflake ... ";
        coord triangle_altitude = get_triangle(user_start_point, user_end_point);
        koch_iteration(user_start_point,  user_end_point,    koch_itr-1, true);
        koch_iteration(user_end_point,    triangle_altitude, koch_itr-1, true);
        koch_iteration(triangle_altitude, user_start_point,  koch_itr-1, true);
        cout << "Done\n";
    }
}

void init(){ // Initializes viewport
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-VIEWPORT_SIZE, VIEWPORT_SIZE, -VIEWPORT_SIZE, VIEWPORT_SIZE); // setup view
}

void display(){ // Draws snowflakes
    glClear(GL_COLOR_BUFFER_BIT);
    if (koch_anti)  koch_antisnowflake();
    else            koch_snowflake();
    glFlush();
}

int main (int argc, char ** argv){
    welcome();

    glutInit(&argc, argv);
    glutInitWindowSize(WINDOW_SIZE,WINDOW_SIZE);  // setup window size
    glutInitDisplayMode(GLUT_RGBA | GLUT_SINGLE); 
    glutCreateWindow("Koch Snowflake");

    glutDisplayFunc(display);
    init();
    glutMainLoop();
}





