#include <bits/stdint-uintn.h>
#include <cstddef>
#include <initializer_list>
#include <ostream>
#include <vector>
#include <iostream>
#include <optional>
#include <string>
#include <termios.h>
#include <unistd.h>
#include <array>

///////////////////////////////////////////////////////////////////////
//                    ***   Optimus prime   ***                      //
//                  ──────────▄▄▄▄▄▄▄▄▄──────────                    //
//                  ───────▄█████████████▄───────                    //
//                  ▐███▌─█████████████████─▐███▌                    //
//                  ─████▄─▀███▄─────▄███▀─▄████─                    //
//                  ─▐█████▄─▀███▄─▄███▀─▄█████▌─                    //
//                  ──██▄▀███▄─▀█████▀─▄███▀▄██──                    //
//                  ──▐█▀█▄▀███─▄─▀─▄─███▀▄█▀█▌──                    //
//                  ───██▄▀█▄██─██▄██─██▄█▀▄██───                    //
//                  ────▀██▄▀██─█████─██▀▄██▀────                    //
//                  ───▄──▀████─█████─████▀──▄───                    //
//                  ───██────────███────────██───                    //
//                  ───██▄────▄█─███─█▄────▄██───                    //
//                  ───████─▄███─███─███▄─████───                    //
//                  ───████─████─███─████─████───                    //
//                  ───████─████─███─████─████───                    //
//                  ───████─████▄▄▄▄▄████─████───                    //
//                  ───▀███─█████████████─███▀───                    //
//                  ─────▀█─███─▄▄▄▄▄─███─█▀─────                    //
//                  ────────▀█▌▐█████▌▐█▀────────                    //
//                  ───────────███████───────────                    //
///////////////////////////////////////////////////////////////////////
//                           blablabla ....

/*
 *
 *                    10-24-2021 Poivret Julien
 *                    *******GNU license*******
 *                 ( A try for a clean graph api )
 *              for applications AND servers interfaces,
 *               whidly inspired from Rust Termion lib.
 *                 "first program ever for me in C++"
 *
 */

//Design notice///////////////////////////////////////////////////////////////////////////////////////////////////////
//   
//        This code is a personal tool mainly developed for my personal use in a way that it's usefull to me
//        Don't have too much expectations on users hanling exception or so. it's not designed for that.
//        ***                          will be improved along the way.                                    ***
//      
//        if you have any questions: -> poivretjulien@gmail.com
//        
//Quick help:
//       
//           First init the class,
//           Add a frame with Add_Frame method  
//           Then add stuff in frame relative coordinates mode...
//           have fun !
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// unicode references ;
const char* CURSOR_HIDE = "\33[?25l";
const char* CURSOR_SHOW = "\33[?25h";
const char* LIGHT_SOLID_LINE = "\u2500";
const char* LIGHT_SOLID_VERTICAL = "\u2502";
const char* LIGHT_SOLID_CROSS = "\u253c";
const char* LIGHT_SOLID_INVERS_T = "\u2534";
const char* LIGHT_SOLID_T = "\u252c";
const char* LIGHT_SOLID_HT_LEFT = "\u251c";
const char* LIGHT_SOLID_HT_RIGHT = "\u2524";
const char* LIGHT_SOLID_CORN_N_EST = "\u2510";
const char* LIGHT_SOLID_CORN_N_WEST = "\u250c";
const char* LIGHT_SOLID_CORN_S_WEST = "\u2514";
const char* LIGHT_SOLID_CORN_S_EST = "\u2518";
const char* LIGHT_SOLID_DOTED_HORIZ_LINE = "\u2574";
const char* LIGHT_SOLID_DOTED_VERT_LINE = "\u2577";

struct point2D {
    unsigned short x;
    unsigned short y;
};

struct Domain {
    unsigned short x_length;
    unsigned short y_length;
};

struct Cell {
    struct point2D anchor_pt;
    struct Domain  cell_domain;
};

//**********************************************************************************
// A data structure with all Cells informations for updating the grid cell enclosed
//   data without recomputing the whole borders intersections and others corners...
//Im adding a grid cell id for a better interactivity, in that way you can in the same 
//   loop update the text content of two grids cell without redrawing the grid cell 
//   of both data structure.
//**********************************************************************************
struct Grid_Cell {
    bool visibility_Border;
    size_t id;
    struct point2D origin_Pt;
    unsigned short max_x;
    unsigned short max_y;
    std::vector<std::vector<Cell>> Cells_parameters_grid;
};


/// !!! for now size_t allow the max possible range aviable whatever the machine is.
/// don't want to limit by software design the program ability... (hardware is there for that).
/// vector is NOT the best memory and performance choice but for small, low frame rate and low resolution
/// of an given classic terminal server, it's sim obviosly to be the best fit...it's nice and robust at my sens.
/// bool is nice and user explicit too, even if it's crunch one byte, rather than one bit ...
/// (but we are not in micro controller and mostly disigning an user api) ;) bitwise op is bad for that...


struct Frame {
    // Grid 2d points origin.
    struct point2D origin_Pt;
    // boundary limit x and y.
    unsigned int length_x;
    unsigned int length_y;
    // Id of the frame
    size_t id;
    // deprecated Boundary box of the frame. (will be removed soon)
    std::vector<std::vector<point2D>> Boundary_box_points_list;
    // deprecated to...
    // the 4 corners of the frame.
    std::vector<point2D*> cross_Pt;
    // Boundary visibility
    bool visibility_Border;
    std::optional<std::vector<Grid_Cell>> gridcell_List;
};

class TermDraw {
public:
    std::optional<std::vector<Frame>> frame_List;
    std::optional<std::vector<Grid_Cell>> grid_cell_List;
    // Main constructor.
    TermDraw()
    {
        std::cout<<CURSOR_HIDE;
        frame_List = std::nullopt;
        grid_cell_List = std::nullopt;
    }

    ///  Draw a basic Horizontal line.
    ///  @param point origin (x,y) + line length
    void Draw_Line_Horiz_basic(unsigned int x,unsigned int y,unsigned int LineLength)
    {
        for(unsigned short int i = 0; i<=LineLength; i++) {
            std::cout<<"\33["<<y<<";"<<x+i<<"H"<<LIGHT_SOLID_LINE;
        }
    }

    ///  Draw a basic Vertical line.
    ///  @param point origin (x,y) + line length
    void Draw_line_Verti_basic(unsigned int x,unsigned int y,unsigned int LineLength)
    {
        for(unsigned short int i = 0; i<=LineLength; i++) {
            std::cout<<"\33["<<y+i<<";"<<x<<"H"<<LIGHT_SOLID_VERTICAL;
        }
    }


  ///     *** Display the data input in the grid at the right place with the max cell allowed capacity ***
  /// id -> a sise_t grid cell identification number... first grid cell created in the frame id=0 second id=1 and so on...
  ///       in that way you can update several grid cell contents in a loop at a given time.
  /// data_input -> vector of 'n' vectors
  /// paging -> size_t, the input vectors is segmented in the right amount of what the grid cells can display
  ///                           ( incressing the number scroll up or down the input data display. )
  /// display_need -> simply give the total column number of the data structure.
  ///                            ( ...that the 'n' vector needed for input... )
    void FeedGridCell(size_t id,std::optional<std::vector<std::vector<const char*>>> data_input,size_t paging,bool display_need)
    {
        if(this->grid_cell_List.has_value()) {
            struct Grid_Cell& tmp = this->grid_cell_List.value()[id];
            if(display_need) {
                // Erase the first line.
                int ct = 0;
                while (ct<tmp.max_x-1) {
                    std::cout<<"\33["<<tmp.origin_Pt.y+1<<";"<<tmp.origin_Pt.x+1+ct<<"H "<<std::endl;
                    ct++;
                }
                // Then write the output.
                std::cout<<"\33["<<tmp.origin_Pt.y+1<<";"<<tmp.origin_Pt.x+1<<"HInput needed: a vec of "<<
                         tmp.Cells_parameters_grid.size()<<" vec(s)"<<std::endl;
            }
            else if (data_input->size() == tmp.Cells_parameters_grid.size()) {
                // Keep the paging value in the right range...
                if(paging<0) {
                    paging = 0;
                }
                if(paging > (data_input.value()[0].size()/tmp.Cells_parameters_grid[0].size())) {
                    paging = data_input.value()[0].size()/tmp.Cells_parameters_grid[0].size();
                }
                // Then display the data in the right amount.
                size_t offset = tmp.Cells_parameters_grid[0].size() * paging;
                size_t y= 0;
                size_t stop_y = tmp.Cells_parameters_grid[0].size();
                size_t mem_pointer = 0;
                size_t x_cursor = 0;
                size_t ln_jump =1;
                uint8_t END_flag=0;
                while(y<stop_y) {
                    for(size_t x=0; x<tmp.Cells_parameters_grid.size(); x++) {
                        struct Cell cell = tmp.Cells_parameters_grid[x][y];
                        if((y+offset) >= data_input.value()[0].size()) {
                            break;
                        }
                        const char* cell_data = data_input.value()[x][y+offset];
                        // Just fill the the Grid cell with the input text.
                        for(size_t buff_y=0; buff_y<cell.cell_domain.y_length; buff_y++) {
                            for(size_t buff_x=0; buff_x<cell.cell_domain.x_length; buff_x++) {
                                //--->"point One" catch entry condition
                                if(mem_pointer) {
                                    x_cursor=(mem_pointer*buff_y)+(ln_jump);
                                    mem_pointer=0;
                                    ln_jump++;
                                }
                                if((uint8_t)*(cell_data+buff_x+x_cursor) != (uint8_t)'\0' && !END_flag) {
                                    std::cout<<"\33["<<cell.anchor_pt.y+buff_y<<";"<<cell.anchor_pt.x+buff_x<<"H"
                                             <<*(cell_data+buff_x+x_cursor)<<std::endl;
                                }
                                else if(END_flag) {
                                    // avoid to rewrite END_flag to 1... just read it ...
                                    std::cout<<"\33["<<cell.anchor_pt.y+buff_y<<";"<<cell.anchor_pt.x+buff_x<<"H "<<std::endl;
                                }
                                else {
                                    // if a "\0" is reached then write " " instead until the grid cell is copletely filled.
                                    END_flag = 1;
                                    std::cout<<"\33["<<cell.anchor_pt.y+buff_y<<";"<<cell.anchor_pt.x+buff_x<<"H "<<std::endl;
                                }
                                // Set exit condition catched by "point One".
                                if(buff_x==(size_t) cell.cell_domain.x_length-1 && cell.cell_domain.y_length>1) {
                                    mem_pointer = buff_x;
                                }
                                if(buff_y == (size_t) cell.cell_domain.y_length-1 &&
                                        buff_x == (size_t) cell.cell_domain.x_length-1) {
                                    x_cursor = 0;
                                    mem_pointer = 0;
                                    END_flag = 0;
                                }
                            }
                        }
                    }
                    y++;
                }
            }
            else {
                std::cout<<"\33[1;1Herror on Grid cell input data !"<<std::endl;
            }
        }
    }

    //#pragma GCC push_options
    //#pragma GCC optimize ("O0")
    //#pragma GCC pop_options
    /*
     * -Add a parameterized grid cell with automatic cells boundary and coordinates feed points.
     *                           (a Frame must have been created before.)
     * -Grid cell origin points: (unsigned short x,unsigned short y)
     *                           "Frame relative" x=0,y=0 -> grid cell at corner up Left.
     * -x_slices initialized_list<unsigned short> like : {1,3,6}
     * -y_slices initialized_list<unsigned short> like : {1,3,6}
     *
     */
    void Add_Grid_Cell(unsigned short x, unsigned short y, const std::initializer_list<unsigned short>x_slices,
                       const std::initializer_list<unsigned short>y_slices)
    {
        if(this->frame_List.has_value()) {
            // Base config settings.
            struct Grid_Cell tmp;

            // Make the current Frame absolute.
            auto& frame_origin = this->frame_List->back().origin_Pt;
            tmp.origin_Pt.x = frame_origin.x + x;
            tmp.origin_Pt.y = frame_origin.y + y;


            // The Domains are build there from "user" slices inputs
            //   -a version vector based will be made for dynamic creation from code.
            //   "non constant based"
            //   -this vertion is user freindly few inputs parameters for driving the whole system.

            ////////////////////////////////////////////////////////////////////////////////////////
            // There the users slices are managed with the right heap allocation
            // for building the necessary basement of the algorythm
            // (only 4 or 8Mbytes on stack) there we belong to the machine ram memory...
            // in that way we can safelly handel all kind of users inputs
            // dynamiclly on the fly... sim to be the right way of doing that...
            ////////////////////////////////////////////////////////////////////////////////////////

            // There the users slices are managed with the right heap allocation
            size_t length = x_slices.size()-1;
            std::vector<std::array<unsigned short,2>> DomainResult_x;
            DomainResult_x.reserve(length);
            std::array<unsigned short, 2>base_block;
            unsigned short ct = 0;
            std::vector<unsigned short> x_intervals;
            x_intervals.reserve(length);
            while(ct<length) {
                base_block[0] = *(x_slices.begin()+ct);
                base_block[1] = *(x_slices.begin()+ct+1);
                ct++;
                x_intervals.emplace_back(base_block[1]-base_block[0]-1);
                DomainResult_x.emplace_back(base_block);
            }
            std::vector<std::array<unsigned short,2>> DomainResult_y;
            ct = 0;
            length = y_slices.size()-1;
            std::vector<unsigned short> y_intervals;
            y_intervals.reserve(length);
            DomainResult_y.reserve(length);
            while(ct<length) {
                base_block[0] = *(y_slices.begin()+ct);
                base_block[1] = *(y_slices.begin()+(ct+1));
                ct++;
                y_intervals.emplace_back(base_block[1]-base_block[0]-1);
                DomainResult_y.emplace_back(base_block);
            }
            ///////////////////////////////////////////////////////////////////////////////////////

            // Compute the cells parameters (min, max)
            // For corners and cross lines identification.
            unsigned short x_max = 0,x_min = 0,y_max = 0,y_min = 0;
            // Find for x...
            for(unsigned short i = 0; i<DomainResult_x.size(); i++) {
                if (x_max <= DomainResult_x[i][0]) {
                    x_max = DomainResult_x[i][0];
                }
                if (x_min >= DomainResult_x[i][0]) {
                    x_min = DomainResult_x[i][0];
                }
            }
            if (x_max <= DomainResult_x[DomainResult_x.size()-1][1]) {
                x_max = DomainResult_x[DomainResult_x.size()-1][1];
            }
            if (x_min >= DomainResult_x[DomainResult_x.size()-1][1]) {
                x_min = DomainResult_x[DomainResult_x.size()-1][1];
            }
            // Find for y...
            for(unsigned short i = 0; i<DomainResult_y.size(); i++) {
                if (y_max <= DomainResult_y[i][0]) {
                    y_max = DomainResult_y[i][0];
                }
                if (y_min >= DomainResult_y[i][0]) {
                    y_min = DomainResult_y[i][0];
                }
            }
            if (y_max <= DomainResult_y[DomainResult_y.size()-1][1]) {
                y_max = DomainResult_y[DomainResult_y.size()-1][1];
            }
            if (y_min >= DomainResult_y[DomainResult_y.size()-1][1]) {
                y_min = DomainResult_y[DomainResult_y.size()-1][1];
            }

            //  Data structure Summary:
            //    - x_slices -> flatten list of slices
            //    - y_slices -> flatten list of slices
            //    - DomainResult_x -> domain list on x.
            //    - DomainResult_y -> domain list on y.
            //    - max min for x and y -> corner of the grids computation helper.

            tmp.max_x = x_max;
            tmp.max_y = y_max;

            /// First we are going to draw the grid cell lines verticals and horizontal.
            for(const unsigned short& x:x_slices) {
                unsigned short ct = 0;
                while(ct<(y_max-y_min)) {
                    std::cout<<"\33["<<(tmp.origin_Pt.y)+ct<<";"<<tmp.origin_Pt.x+x<<"H"
                             <<LIGHT_SOLID_VERTICAL<<std::endl;
                    ct++;
                }
            }
            for(const unsigned short& y:y_slices) {
                unsigned short ct = 0;
                while(ct<(x_max-x_min)) {
                    std::cout<<"\33["<<tmp.origin_Pt.y+y<<";"<<
                             tmp.origin_Pt.x+ct<<"H"<<LIGHT_SOLID_LINE<<std::endl;
                    ct++;
                }
            }

            // Secondly start drawing the intersections.
            //  ... simply "cross" or "right kind of corner"
            //  at slice points intersections.
            for(const unsigned short& t_y:y_slices) {
                for(const unsigned short& t_x:x_slices) {
                    /// ( then identification ).
                    if(t_x==x_min && t_y==y_min) {
                        std::cout<<"\33["<<t_y+tmp.origin_Pt.y<<";"
                                 <<t_x+tmp.origin_Pt.x<<"H"<<LIGHT_SOLID_CORN_N_WEST<<std::endl;
                    }
                    else if(t_x==x_max && t_y == y_min) {
                        std::cout<<"\33["<<t_y+tmp.origin_Pt.y<<";"
                                 <<t_x+tmp.origin_Pt.x<<"H"<<LIGHT_SOLID_CORN_N_EST<<std::endl;
                    }
                    else if( t_x==x_max && t_y!=y_max) {
                        std::cout<<"\33["<<t_y+tmp.origin_Pt.y<<";"
                                 <<t_x+tmp.origin_Pt.x<<"H"<<LIGHT_SOLID_HT_RIGHT<<std::endl;
                    }
                    else if( t_x==x_min && t_y!=y_max) {
                        std::cout<<"\33["<<t_y+tmp.origin_Pt.y<<";"
                                 <<t_x+tmp.origin_Pt.x<<"H"<<LIGHT_SOLID_HT_LEFT<<std::endl;
                    }
                    else if( t_x==x_min && t_y==y_max) {
                        std::cout<<"\33["<<t_y+tmp.origin_Pt.y<<";"
                                 <<t_x+tmp.origin_Pt.x<<"H"<<LIGHT_SOLID_CORN_S_WEST<<std::endl;
                    }
                    else if( t_x==x_max && t_y==y_max) {
                        std::cout<<"\33["<<t_y+tmp.origin_Pt.y<<";"
                                 <<t_x+tmp.origin_Pt.x<<"H"<<LIGHT_SOLID_CORN_S_EST<<std::endl;
                    }
                    else if( (t_x!=x_max && t_x) && (t_y!=y_max && t_y)) {
                        std::cout<<"\33["<<t_y+tmp.origin_Pt.y<<";"
                                 <<t_x+tmp.origin_Pt.x<<"H"<<LIGHT_SOLID_CROSS<<std::endl;
                    }
                    else if( (t_x!=x_min && t_x!=x_max) && t_y==y_min) {
                        std::cout<<"\33["<<t_y+tmp.origin_Pt.y<<";"
                                 <<t_x+tmp.origin_Pt.x<<"H"<<LIGHT_SOLID_T<<std::endl;
                    }
                    else if( (t_x!=x_min && t_x!=x_max) && t_y==y_max) {
                        std::cout<<"\33["<<t_y+tmp.origin_Pt.y<<";"
                                 <<t_x+tmp.origin_Pt.x<<"H"<<LIGHT_SOLID_INVERS_T<<std::endl;
                    }
                    else {
                        std::cout<<"\33["<<t_y+tmp.origin_Pt.y<<";"
                                 <<t_x+tmp.origin_Pt.x<<"H"<<LIGHT_SOLID_LINE<<std::endl;
                    }
                }
            }

            // make a grid of row & column
            //    x1 x2 x3 x4 x5
            // y1  x  x  x  x  x
            // y2  x  x  x  x  x
            // y3  x  x  x  x  x
            // y4  x  x  x  x  x
            // y5  x  x  x  x  x

            /// Now we have the grid Cell we need the whole set of cells anchor points
            /// and cells intervals on x an y... so:
            /// we make the 2D matrix_points of the anchor points like that -> anchor_pt[x][y]
            /// for easly indexing the grid (api convinien).

            // We build the Data structure with a nested loop.
            tmp.Cells_parameters_grid.reserve(x_slices.size()-1);
            std::vector<Cell> cell_column;
            cell_column.reserve(y_slices.size()-1);
            for(unsigned short x = 0; (unsigned short)x<x_slices.size()-1; x++) {
                for(unsigned short y = 0; (unsigned short)y<y_slices.size()-1; y++) {
                    struct Cell cell_block;
                    cell_block.anchor_pt.x = *(x_slices.begin()+x)+1+tmp.origin_Pt.x;
                    cell_block.anchor_pt.y = *(y_slices.begin()+y)+1+tmp.origin_Pt.y;
                    cell_block.cell_domain.x_length = x_intervals[x];
                    cell_block.cell_domain.y_length = y_intervals[y];
                    cell_column.emplace_back(cell_block);
                }
                tmp.Cells_parameters_grid.emplace_back(cell_column);
                cell_column.clear();
            }

            /// Internal debug option show the data structure aspect. (will be removed later.)
            bool bake_mode = false;
            if(bake_mode) {
                unsigned short shift_y = 0;
                unsigned short shift_x = 0;
                for(int x=0; x<(int)tmp.Cells_parameters_grid.size(); x++) {
                    for(int y=0; y<(int)tmp.Cells_parameters_grid[0].size(); y++) {
                        std::cout<<"\33["<<1+shift_y<<";"<<1+shift_x<<"H x:"<<
                                 tmp.Cells_parameters_grid[x][y].anchor_pt.x<<std::endl;
                        std::cout<<"\33["<<2+shift_y<<";"<<1+shift_x<<"H y:"<<
                                 tmp.Cells_parameters_grid[x][y].anchor_pt.y<<std::endl;
                        std::cout<<"\33["<<1+shift_y<<";"<<6+shift_x<<"H Len(x):"<<
                                 tmp.Cells_parameters_grid[x][y].cell_domain.x_length<<std::endl;
                        std::cout<<"\33["<<2+shift_y<<";"<<6+shift_x<<"H Len(y):"<<
                                 tmp.Cells_parameters_grid[x][y].cell_domain.y_length<<std::endl;
                        shift_y+=3;
                    }
                    shift_y = 0;
                    shift_x+=15;
                }
            }

            /// Finaly manage the optional for a nice data structure...
            if(this->grid_cell_List.has_value()) {
	        // increment the GridCell id.
	        tmp.id = this->grid_cell_List->size() - 1;
                // if there are allready some Grid_Cell
                // then emplace_back (put on the last allready allocated memorry)
                this->grid_cell_List.value().emplace_back(tmp);
            }
            else {
                // if no value then make a first one
                std::vector<Grid_Cell> stack_grid;
		// First grid cell -> id = 0.
		tmp.id = 0;
                stack_grid.emplace_back(tmp);
                this->grid_cell_List = stack_grid;
            }
        }
    }

    void clear_last_Frame()
    {
        if(this->frame_List.has_value()) {
            struct Frame& tmp = this->frame_List->back();
            for(unsigned short x=0; x<tmp.length_x; x++) {
                for(unsigned short y=0; y<tmp.length_y; y++) {
                    std::cout<<"\33["<<y+tmp.origin_Pt.y<<";"<<x+tmp.origin_Pt.x<<"H ";
                }
            }
        }
    }

    /**
     *
     * A frame let the user to divide the terminal in Frames boxs and allow by this way a relative
     * x & y coordinates from the Frame origin point (corner up left of the targeted id Frame box)
     *
     */
    void Add_Frame(unsigned short origin_x,unsigned short origin_y,
                   unsigned short length_x, unsigned short length_y,bool visibility,bool clear)
    {
        struct Frame tmp;
        tmp.origin_Pt.x = origin_x;
	tmp.origin_Pt.y = origin_y;
	tmp.length_x = length_x;
	tmp.length_y = length_y;
	// Trace the two verticals.
	if(clear){
	  for(int y =0;y<length_y;y++){
	    for(int x =0;x<length_x;x++){
	      std::cout<<"\33["<<origin_y+y<<";"<<origin_x+x<<"H ";
	    }
	  }
	}
	if(visibility) {
	  unsigned short ct = 0;
	  while(ct <= (length_x-1)) {
	    for(unsigned short i=0; i<=(length_y-1); i++) {
	      std::cout<<"\33["<<origin_y+i<<";"<<origin_x+ct<<"H"<<LIGHT_SOLID_VERTICAL;
	    }
	    ct += (length_x-1);
	  }
	  ct = 0;
	  // Trace the two horizotals.
	  while(ct<=(length_y-1)) {
	    for(unsigned short i=0; i<=(length_x-1); i++) {
	      // mind the frame corners.
	      if(i==0 && ct ==0) {
		std::cout<<"\33["<<origin_y+ct<<";"<<origin_x+i<<"H"<<LIGHT_SOLID_CORN_N_WEST;
	      }
	      else if(i==(length_x-1) && ct==0) {
		std::cout<<"\33["<<origin_y+ct<<";"<<origin_x+i<<"H"<<LIGHT_SOLID_CORN_N_EST;
	      }
	      else if(i==0 && ct==(length_y-1)) {
		std::cout<<"\33["<<origin_y+ct<<";"<<origin_x+i<<"H"<<LIGHT_SOLID_CORN_S_WEST;
	      }
	      else if(i==(length_x-1) && ct==(length_y-1)) {
		std::cout<<"\33["<<origin_y+ct<<";"<<origin_x+i<<"H"<<LIGHT_SOLID_CORN_S_EST;
	      }
	      else {
		std::cout<<"\33["<<origin_y+ct<<";"<<origin_x+i<<"H"<<LIGHT_SOLID_LINE;
	      }
	    }
	    ct += (length_y-1);
	  }
	}
	if(this->frame_List.has_value()) {
	  // add a Frame.
	  tmp.id = this->frame_List.value().back().id++;
	  frame_List.value().push_back(tmp);
	}
	else {
	  // create a Frame.
	  tmp.id = 0;
	  std::vector<Frame> ListFrame;
	  ListFrame.push_back(tmp);
	  this->frame_List = ListFrame;
	}
    }

    /**
     *
     * A frame let the user to divide the terminal in Frames boxs and allow by this way a relative
     * x & y coordinates from the Frame origin point (corner up left of the targeted id Frame box)
     *                                       deprecated ?
     */
    void Add_Frame_old(unsigned int origin_x,unsigned int origin_y,unsigned int length_x,
	unsigned int length_y,bool visibility)
    {
      struct Frame tmp;
      tmp.origin_Pt.x = origin_x;
      tmp.origin_Pt.y = origin_y;
      tmp.length_x = length_x;
      tmp.length_y = length_y;
      struct point2D pt2d;
      unsigned int ct = 0;
      // Trace the two verticals.
      while(ct<=(length_x-1)) {
	std::vector<point2D> row;
	for(unsigned int i=0; i<=(length_y-1); i++) {
	  pt2d.x = origin_x + ct;
	  pt2d.y = origin_y + i;
	  row.push_back(pt2d);
	  if(visibility) {
	    std::cout<<"\33["<<pt2d.y<<";"<<pt2d.x<<"H"<<LIGHT_SOLID_VERTICAL;
	  }
	}
	ct += (length_x-1);
	tmp.Boundary_box_points_list.push_back(row);
	row.clear();
      }
      ct = 0;
      // Trace the two horizotals.
      while(ct<=(length_y-1)) {
	std::vector<point2D> row;
	for(unsigned int i=0; i<=(length_x-1); i++) {
	  pt2d.x = origin_x + i;
	  pt2d.y = origin_y + ct;
	  row.push_back(pt2d);
	  if(i==0||i==(length_x-1)) {
	    tmp.cross_Pt.push_back(&row.back()); //mind the life time.
	  }
	  if(visibility) {
	    if(i==0 && ct ==0) {
	      std::cout<<"\33["<<pt2d.y<<";"<<pt2d.x<<"H"<<LIGHT_SOLID_CORN_N_WEST;
	    }
	    else if(i==(length_x-1) && ct==0) {
	      std::cout<<"\33["<<pt2d.y<<";"<<pt2d.x<<"H"<<LIGHT_SOLID_CORN_N_EST;
	    }
	    else if(i==0 && ct==(length_y-1)) {
	      std::cout<<"\33["<<pt2d.y<<";"<<pt2d.x<<"H"<<LIGHT_SOLID_CORN_S_WEST;
	    }
	    else if(i==(length_x-1) && ct==(length_y-1)) {
	      std::cout<<"\33["<<pt2d.y<<";"<<pt2d.x<<"H"<<LIGHT_SOLID_CORN_S_EST;
	    }
	    else {
	      std::cout<<"\33["<<pt2d.y<<";"<<pt2d.x<<"H"<<LIGHT_SOLID_LINE;
	    }
	  }
	}
	ct += (length_y-1);
	tmp.Boundary_box_points_list.push_back(row);
	row.clear();
      }
      if(this->frame_List.has_value()) {
	// add a Frame.
	tmp.id = this->frame_List.value().back().id++;
	frame_List.value().push_back(tmp);
      }
      else {
	// create a Frame.
	tmp.id = 0;
	std::vector<Frame> ListFrame;
	ListFrame.push_back(tmp);
	this->frame_List = ListFrame;
      }
    }

    /// Add a text in a Frame relative to the Frame origin point,
    /// (for that of course a frame have to be created before...)
    void Frame_Add_Text(unsigned short x,unsigned short y,const char * str,size_t id)
    {
      if (this->frame_List.has_value()) {
	struct point2D* origin_Pt = &(this->frame_List.value()[id].origin_Pt);
	std::cout<<"\33["<<origin_Pt->y + y<<";"<<origin_Pt->x + x<<"H"<<str;
      }
    }

    ///  Draw a Frame based Horizontal line.
    ///  point origin (x,y) + line length + Frame id
    void Frame_Draw_Line_Horiz_basic(unsigned short  x,unsigned short y,
	unsigned short  LineLength,size_t id)
    {
      if(this->frame_List.has_value()) {
	for(unsigned short i = 0; i<=LineLength; i++) {
	  std::cout<<"\33["<<y+this->frame_List.value()[id].origin_Pt.y<<";"<<x +
	    this->frame_List.value()[id].origin_Pt.x + i<<"H"<<LIGHT_SOLID_LINE;
	}
      }
    }

    ///  Draw a Frame based Vertical line.
    ///  point origin (x,y) + line length + Frame id
    void Frame_Draw_Line_Verti_basic(unsigned short x,unsigned short y,
	unsigned short LineLength,size_t id)
    {
      if(this->frame_List.has_value()) {
	for(unsigned short i = 0; i<=LineLength; i++) {
	  std::cout<<"\33["<<y+this->frame_List.value()[id].origin_Pt.y + i<<";"<<x +
	    this->frame_List.value()[id].origin_Pt.x <<"H"<<LIGHT_SOLID_VERTICAL;
	}
      }
    }


    ///TODO: will be implemented at the end of the program...
    unsigned short Get_Frame_Memory()
    {
      if(this->frame_List.has_value()) {
	unsigned short TotalMemory = this->frame_List.value().capacity() * sizeof this->frame_List.value();
	for (unsigned short i=0; i<this->frame_List.value().size(); i++) {
	  ///     later.
	}
	return TotalMemory;
      }
      else {
	return 0;
      }
    }
    /// Switch the terminal in to raw mode and disable the STDIn echo.
    void intoRawMode()
    {
      struct termios raw;
      tcgetattr(STDIN_FILENO,&raw);
      raw.c_lflag &= ~ICANON;
      raw.c_lflag &= ~ECHO;
      tcsetattr(STDIN_FILENO,TCSANOW, &raw);
    }

    /// Class Destructor.
    ~TermDraw()
    {
      std::cout<<"...Debug info -> TermDraw class destructor..."<<std::endl;
      reset_terminal();
      std::cout<<CURSOR_SHOW;
    }

private:
    void reset_terminal()
    {
      struct termios raw_out;
      tcgetattr(STDIN_FILENO, &raw_out);
      raw_out.c_lflag |= ICANON;
      raw_out.c_lflag |= ECHO;
      tcsetattr(STDIN_FILENO,2, &raw_out);
    }
};
