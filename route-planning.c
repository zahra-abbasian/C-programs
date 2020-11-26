/*******************************************************************************
 Grid Base Route Planning in a 2D cell grids including blocks
*******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <limits.h>

// various separators used in input/output
#define SEP0 '$'
#define SEP1 "==STAGE %d=======================================\n"
#define SEP2 "------------------------------------------------\n"
#define SEP3 "================================================\n"

// ASCII codes used in grid visualizations
#define CELL_CODE_INITIAL   73  // 'I' - initial cell
#define CELL_CODE_GOAL      71  // 'G' - goal cell
#define CELL_CODE_BLOCK     35  // '#' - block cell
#define CELL_CODE_EMPTY     32  // ' ' - empty cell
#define CELL_CODE_VISITED   42  // '*' - visited cell

// route statuses
#define ROUTE_STATUS_UNKNOWN    0 // Unknown route status
#define ROUTE_INVALID_INITIAL   1 // Initial cell in the route is wrong!
#define ROUTE_INVALID_GOAL      2 // Goal cell in the route is wrong!
#define ROUTE_INVALID_MOVE      3 // There is an illegal move in this route!
#define ROUTE_INVALID_BLOCK     4 // There is a block on this route!
#define ROUTE_VALID             5 // The route is valid!

// grid reset modes
#define RESET_FULL                  0 // full reset
#define RESET_VISITED               1 // reset visited cells only
#define RESET_VISITED_AND_BLOCKS    2 // reset visited and blocked cells only

#define MAX_CELLS_PER_LINE          5 // max number of cells to print per line

/* type definitions ----------------------------------------------------------*/

// grid cell content
typedef struct {
    char code;              // ASCII character for visualization of the cell
    unsigned int counter;   // counter value used to discover a route
} cell_info_t;

// grid cell
typedef struct {
    short row;              // row of the cell
    short col;              // column of the cell
    unsigned int counter;   // counter value associated with this cell
} cell_t;

// state in a route
typedef struct state state_t;
struct state {
    cell_t *cell;           // pointer to a cell
    state_t *next;          // pointer to the next state in the route
};

// route (implemented as a linked list)
typedef struct {
    state_t *head;  // pointer to the node in the head of the linked list
    state_t *tail;  // pointer to the node in the tail of the linked list
} route_t;

// grid
typedef cell_info_t **grid_t;

/* function prototypes -------------------------------------------------------*/
grid_t read_grid(short,short,unsigned int*,cell_t*,cell_t*);
void reset_grid(grid_t,short,short,int);
int read_cell(short,short,cell_t*);
int read_blocks(grid_t,short,short);
route_t* read_route(short,short);
void print_route(route_t*);
int get_route_status(grid_t,short,short,route_t*);
int check_move(short,short,cell_t*,cell_t*);
void print_route_status(int);
void free_grid(grid_t,short,short);
void print_grid(grid_t,short,short);
void add_route_to_grid(grid_t,short,short,route_t*);
cell_t* create_cell(short,short,int);
int handle_route_repair(grid_t,short,short,route_t*,int,
                            route_t* (*repair)(grid_t,short,short,route_t*));
route_t* get_first_broken_fragment(grid_t,int,int,route_t*);
route_t* construct_repair(grid_t,int,int,route_t*,route_t*);
cell_t* create_next_cell(grid_t,int,int,int,int,route_t*,int);
state_t* found_repair(route_t*,cell_t*);
route_t* stage1repair(grid_t,short,short,route_t*);
route_t* stage2repair(grid_t,short,short,route_t*);
route_t* make_empty_list(void);
route_t* insert_at_head(route_t*, cell_t*);
route_t* insert_at_tail(route_t*, cell_t*);
void free_list(route_t* R);

/* where it all happens ------------------------------------------------------*/
int
main(int argc, char *argv[]) {
    // STAGE 0
    printf(SEP1,0);
    // read and print grid dimensions, check if grid has at least two cells
    // grid can have between 1 and 32,767 rows/columns
    short rows, cols;
    if (scanf("%hdx%hd\n",&rows,&cols)!=2 || rows<1 || cols<1 || rows*cols<2) {
        fprintf(stderr,"Wrong grid dimensions!\n");
        return EXIT_FAILURE;
    }
    printf("The grid has %d rows and %d columns.\n",rows,cols);

    // read grid from stdin
    grid_t G; unsigned int nblocks; cell_t init, goal;
    if ((G=read_grid(rows,cols,&nblocks,&init,&goal))==NULL) {
        fprintf(stderr,"Error when reading grid!\n");
        return EXIT_FAILURE;
    }

    // print grid statistics
    printf("The grid has %d block(s).\n",nblocks);
    printf("The initial cell in the grid is [%d,%d].\n",init.row,init.col);
    printf("The goal cell in the grid is [%d,%d].\n",goal.row,goal.col);

    // read separator control character
    char c;
    if (scanf("%c\n",&c)!=1 || c!=SEP0) {
        fprintf(stderr,"Error when reading input!\n");
        return EXIT_FAILURE;
    }

    // read route from stdin
    route_t *R=read_route(rows,cols);

    // print route
    printf("The proposed route in the grid is:\n");
    print_route(R);

    // check and print route status
    int status = get_route_status(G,rows,cols,R);
    print_route_status(status);

    // STAGE 1
    printf(SEP1,1);
    status=handle_route_repair(G,rows,cols,R,status,stage1repair);

    if (status!=ROUTE_INVALID_BLOCK && status!=ROUTE_VALID) {
        printf(SEP3);
        return EXIT_FAILURE;
    }

    // STAGE 2
    if (scanf("%c\n",&c)==1 && c==SEP0) {
        printf(SEP1,2);
        do {
            reset_grid(G,rows,cols,RESET_VISITED_AND_BLOCKS);
            read_blocks(G,rows,cols);
            status=handle_route_repair(G,rows,cols,R,ROUTE_STATUS_UNKNOWN,
                                       stage2repair);
            printf(SEP3);
        } while (scanf("%c\n",&c)==1 && c==SEP0);   // forgive me, Alistair :)
    }
    else printf(SEP3);

    free_grid(G,rows,cols);
    free_list(R); R=NULL;
    return EXIT_SUCCESS;        // we are done, vitayu!!! algorithms are fun!!!
}

// handle route repair (including printing to stdout)
int
handle_route_repair(grid_t G, short rows, short cols, route_t *R, int status,
        route_t* (*repair)(grid_t,short,short,route_t*)) {
    if (status==ROUTE_STATUS_UNKNOWN) status=get_route_status(G,rows,cols,R);
    reset_grid(G,rows,cols,RESET_VISITED);      // reset visited cells
    add_route_to_grid(G,rows,cols,R);           // mark visited cells
    print_grid(G,rows,cols);                    // print original grid
    if (status==ROUTE_INVALID_BLOCK) {          // route is blocked
        printf(SEP2);                           // attempt repair
        route_t *new_fragment = (*repair)(G,rows,cols,R); // repair route
        reset_grid(G,rows,cols,RESET_VISITED);            // reset visited cells
        add_route_to_grid(G,rows,cols,R);                 // mark visited cells
        print_grid(G,rows,cols);                          // print grid
        printf(SEP2);                           // print separator

        // print route and its status to stdout
        if (!new_fragment) printf("The route cannot be repaired!\n");
        else {
            print_route(R);
            status=get_route_status(G,rows,cols,R);
            print_route_status(status);
        }
        free(new_fragment); new_fragment=NULL;
    }

    return status;  // return route status
}

// Route repair according to Stage 1 of the specification (first segment)
route_t
*stage1repair(grid_t G, short rows, short cols, route_t *R) {
    // get first broken segment of route R
    route_t *fragment     = get_first_broken_fragment(G,rows,cols,R);

    // construct repair of the broken fragment
    route_t *new_fragment = construct_repair(G,rows,cols,R,fragment);

    // if repair was constructed, implement it
    if (new_fragment) {
        state_t *curr = fragment->head->next;
        while (curr!=fragment->tail) { // free broken fragment
            state_t *prev = curr;
            curr = curr->next;
            free(prev);
        }
        fragment->head->next = new_fragment->head->next;
        free(fragment); fragment=NULL;
    }

    return new_fragment; // repaired fragment of the route
}

// Route repair according to Stage 2 of the specification (all segments)
route_t
*stage2repair(grid_t G, short rows, short cols, route_t *R) {
    route_t *new_fragment=NULL;
    while (get_route_status(G,rows,cols,R)==ROUTE_INVALID_BLOCK) {
        // get first broken segment of route R
        route_t *fragment = get_first_broken_fragment(G,rows,cols,R);

        // construct repair of the broken fragment
        new_fragment = construct_repair(G,rows,cols,R,fragment);
        if (new_fragment==NULL) {
            if (fragment) { free(fragment); fragment=NULL; }
            break; // repair is not possible
        }
        state_t *curr = fragment->head->next;
        while (curr!=fragment->tail) { // free broken fragment
            state_t *prev = curr;
            curr = curr->next;
            free(prev);
        }
        // if repair was constructed, implement it
        fragment->head->next = new_fragment->head->next;
        free(fragment); fragment=NULL;
    }

    return new_fragment; // repaired fragment of the route
}

// read grid from stdin
grid_t
read_grid(short rows, short cols, unsigned int *n, cell_t *init, cell_t *goal) {
    if (!read_cell(rows,cols,init)) return NULL; // read initial cell position
    if (!read_cell(rows,cols,goal)) return NULL; // read goal cell position

    // allocate memory for the grid
    grid_t G=malloc(rows*sizeof(cell_info_t*));
    assert(G);
    short i;
    for (i=0; i<rows; i++) {
        G[i]=malloc(cols*sizeof(cell_info_t));
        assert(G[i]);
    }

    reset_grid(G,rows,cols,RESET_FULL); // reset grid
    (*n)=read_blocks(G,rows,cols);      // read block positions

    // set initial and goal cells
    G[init->row][init->col].code=CELL_CODE_INITIAL;
    G[goal->row][goal->col].code=CELL_CODE_GOAL;

    return G;
}

// reset grid
void
reset_grid(grid_t G, short rows, short cols, int mode) {
    short r, c;
    for (r=0; r<rows; r++) {                            // iterate over rows
        for (c=0; c<cols; c++) {                        // iterate over columns
            if (mode==RESET_VISITED) {                  // reset visited cells
                if (G[r][c].code==CELL_CODE_VISITED) 
                    G[r][c].code=CELL_CODE_EMPTY;
            }
            else if (mode==RESET_VISITED_AND_BLOCKS) {  // reset visited cells 
                if (G[r][c].code==CELL_CODE_VISITED ||  // and blocks
                        G[r][c].code==CELL_CODE_BLOCK)
                G[r][c].code=CELL_CODE_EMPTY;
            }
            else G[r][c].code=CELL_CODE_EMPTY;

            G[r][c].counter=UINT_MAX; // reset counter values
        }
    }
}

// read cell from stdin and check if it is within the grid boundaries
int
read_cell(short rows, short cols, cell_t* cell) {
    if (scanf("[%hd,%hd]\n",&cell->row,&cell->col)!=2 ||
    cell->row<0 || cell->col<0 || cell->row>=rows || cell->col>=cols) return 0;

    return 1;
}

// read blocks from stdin and set them in the grid
int
read_blocks(grid_t G, short rows, short cols) {
    cell_t cell;
    int result=0;
    while (read_cell(rows,cols,&cell)) { // read next block cell from stdin
        G[cell.row][cell.col].code=CELL_CODE_BLOCK;
        result++;
    }

    return result;
}

// read route from stdin
route_t
*read_route(short rows, short cols) {
    int r, c;
    route_t* R = make_empty_list();
    while (scanf("[%d,%d]",&r,&c)==2) {
        cell_t* cell = create_cell(r,c,0);
        R = insert_at_tail(R,cell);
        scanf("->"); scanf("\n");
    }

    return R;
}

// print route to stdout
void
print_route(route_t* R) {
    assert(R!=NULL);
    int i=0; state_t *curr=R->head;
    while (curr) {
        i++;
        printf("[%d,%d]",curr->cell->row,curr->cell->col);
        curr=curr->next;
        if (curr) {
            printf("->");
            if (i%MAX_CELLS_PER_LINE==0) printf("\n");
        }
    }
    printf(".\n");
}

// compute route status
int
get_route_status(grid_t G, short rows, short cols, route_t* R) {
    assert(G!=NULL && R!=NULL);
    // initial checks of the initial cell and goal cell of the route
    if (R->tail==NULL) return ROUTE_INVALID_INITIAL;
    if (G[R->tail->cell->row][R->tail->cell->col].code!=CELL_CODE_GOAL)
        return ROUTE_INVALID_GOAL;
    if (G[R->head->cell->row][R->head->cell->col].code!=CELL_CODE_INITIAL)
        return ROUTE_INVALID_INITIAL;

    // check if route R is valid and there is a block on route R
    int blocked = 0;
    state_t *prev = R->head;
    state_t *curr = prev->next;
    if (curr==R->tail && !check_move(rows,cols,prev->cell,curr->cell))
        return ROUTE_INVALID_MOVE;
    else {
        while (curr!=R->tail) {
            if (!check_move(rows,cols,prev->cell,curr->cell))
                return ROUTE_INVALID_MOVE;

            if (G[curr->cell->row][curr->cell->col].code==CELL_CODE_BLOCK)
                blocked = 1;

            prev = curr;
            curr = curr->next;
        }
    }

    if (blocked) return ROUTE_INVALID_BLOCK;

    return ROUTE_VALID; // yes, route R is valid!
}

// check if a move between two cells of a grid is valid
int 
check_move(short rows, short cols, cell_t *from, cell_t *to) {
    if (from->row<0 || from->col<0 || to->row<0 || to->col<0 || 
        from->row>=rows || from->col>=cols || to->row>=rows || 
        to->col>=cols) return 0;

    if ((abs(from->row-to->row)+abs(from->col-to->col))!=1) return 0;

    return 1;
}

// print route status to stdout
void
print_route_status(int status) {
    if (status==ROUTE_INVALID_INITIAL) 
        printf("Initial cell in the route is wrong!\n");
    else if (status==ROUTE_INVALID_GOAL) 
        printf("Goal cell in the route is wrong!\n");
    else if (status==ROUTE_INVALID_MOVE) 
        printf("There is an illegal move in this route!\n");
    else if (status==ROUTE_INVALID_BLOCK) 
        printf("There is a block on this route!\n");
    else if (status==ROUTE_VALID) 
        printf("The route is valid!\n");
    else return;
}

// free grid
void
free_grid(grid_t G, short rows, short cols) {
    int i;
    for (i=0; i<rows; i++) free(G[i]);

    free(G);
    G=NULL;
}

// print grid to stdout
void
print_grid(grid_t G, short rows, short cols) {
    int i, j;
    printf(" ");
    for (j=0; j<cols; j++) printf("%d", j%10);
    printf("\n");

    for (i=0; i<rows; i++) {
        printf("%d", (i)%10);
        for (j=0; j<cols; j++) {
            putchar(G[i][j].code);
        }
        printf("\n");
    }
}

// allocate memory to store information about a grid cell
cell_t
*create_cell(short r, short c, int counter) {
    cell_t* cell;
    cell = (cell_t*)malloc(sizeof(*cell));
    assert(cell);
    cell->row = r;
    cell->col = c;
    cell->counter = counter;
    return cell;
}

// add route R to grid G
void 
add_route_to_grid(grid_t G, short rows, short cols, route_t* R) {
    assert(G!=NULL && R!=NULL && R->head!=NULL);
    state_t *curr = R->head->next;
    while (curr) {
        if (curr!=R->tail && curr->cell->row>=0 && curr->cell->col>=0 &&
            curr->cell->row<rows && curr->cell->col<cols &&
            G[curr->cell->row][curr->cell->col].code==CELL_CODE_EMPTY)
                G[curr->cell->row][curr->cell->col].code=CELL_CODE_VISITED;

        curr = curr->next;
    }
}

// get first broken segment of route R
route_t
*get_first_broken_fragment(grid_t G, int rows, int cols, route_t* R) {
    assert(G!=NULL && R!=NULL);
    route_t *result = make_empty_list();
    state_t *prev = R->head;
    state_t *curr = prev->next;
    int flag = 0;
    while (curr) {
        if (G[curr->cell->row][curr->cell->col].code==CELL_CODE_BLOCK && !flag){
            result->head = prev;
            flag=1;
        }
        if (flag && G[curr->cell->row][curr->cell->col].code!=CELL_CODE_BLOCK) {
            result->tail = curr;
            return result;
        }
        prev = curr;
        curr = curr->next;
    }
    
    free_list(result); result=NULL;
    return NULL; // no broken segments in R
}

// construct repair of broken segment F in route R
route_t
*construct_repair(grid_t G, int rows, int cols, route_t* R, route_t* F) {
    assert(G!=NULL);
    // initialize a queue with the cell at the start of segment F 
    route_t* main_list = make_empty_list();
    cell_t* addr = create_cell(F->head->cell->row,F->head->cell->col,0);
    main_list = insert_at_tail(main_list, addr);

    // process queue
    state_t *curr = main_list->head;
    cell_t *nc;
    state_t *done = NULL;
    while (curr) {
        // add cell above current cell
        if ((nc=create_next_cell(G,rows,cols,curr->cell->row-1,curr->cell->col,
                                  main_list,curr->cell->counter))!=NULL) { 
            main_list = insert_at_tail(main_list,nc);
            if ((done=found_repair(F,nc))!=NULL) break; // repair found
        }
        // add cell below current cell
        if ((nc=create_next_cell(G,rows,cols,curr->cell->row+1,curr->cell->col,
                                  main_list,curr->cell->counter))!=NULL) { 
            main_list = insert_at_tail(main_list, nc);
            if ((done=found_repair(F,nc))!=NULL) break; // repair found
        }
        // add cell to the left from current cell
        if ((nc=create_next_cell(G,rows,cols,curr->cell->row,curr->cell->col-1,
                                  main_list,curr->cell->counter))!=NULL) {
            main_list = insert_at_tail(main_list, nc);
            if ((done=found_repair(F,nc))!=NULL) break; // repair found
        }
        // add cell to the right from current cell
        if ((nc=create_next_cell(G,rows,cols,curr->cell->row,curr->cell->col+1,
                                  main_list,curr->cell->counter))!=NULL) {
            main_list = insert_at_tail(main_list, nc);
            if ((done=found_repair(F,nc))!=NULL) break; // repair found
        }

        curr = curr->next;
    }

    if (!done) {
        free_list(main_list); main_list=NULL;
        return NULL; // repair was not found
    }

    curr = main_list->head;
    while (curr) {
        G[curr->cell->row][curr->cell->col].counter = curr->cell->counter;
        curr = curr->next;
    }

    // construct broken segment repair
    route_t* result = make_empty_list();
    cell_t* cell = create_cell(main_list->tail->cell->row,
                                main_list->tail->cell->col,0);
    result = insert_at_head(result,cell);

    while (G[cell->row][cell->col].counter>0) {
        int r=cell->row;
        int c=cell->col;
        int count = G[r][c].counter;

        if (r-1>=0 && G[r-1][c].code!=CELL_CODE_BLOCK &&
                G[r-1][c].counter==count-1) {
            cell = create_cell(r-1,c,0);
            result = insert_at_head(result,cell);
        }
        else if (r+1<rows && G[r+1][c].code!=CELL_CODE_BLOCK &&
                G[r+1][c].counter==count-1) {
            cell = create_cell(r+1,c,0);
            result = insert_at_head(result,cell);
        }
        else if (c-1>=0 && G[r][c-1].code!=CELL_CODE_BLOCK &&
                G[r][c-1].counter==count-1) {
            cell = create_cell(r,c-1,0);
            result = insert_at_head(result,cell);
        }
        else if (c+1<cols && G[r][c+1].code!=CELL_CODE_BLOCK &&
                G[r][c+1].counter==count-1) {
            cell = create_cell(r,c+1,0);
            result = insert_at_head(result,cell);
        }
    }

    state_t *prev = result->head;
    curr = prev->next;
    while (curr->next) {
        prev=curr;
        curr=curr->next;
    }

    prev->next = done;
    free_list(main_list); main_list=NULL;
    return result;
}

// check if a repair has been found
state_t
*found_repair(route_t* frag, cell_t* addr) {
    state_t *curr = frag->tail;
    while (curr) {
        if (curr->cell->row==addr->row && curr->cell->col==addr->col)
            return curr; // repair found

        curr = curr->next;
    }
    return NULL; // repair not found
}

// construct next cell to insert into the queue for constructing route repair
cell_t
*create_next_cell(grid_t G, int rows, int cols, int r, int c, 
                    route_t* R, int counter) {
    // perform initial checks
    assert(G!=NULL && R!=NULL);
    if (r<0 || c<0 || r>rows-1 || c>cols-1) return NULL;
    if (G[r][c].code==CELL_CODE_BLOCK) return NULL;

    state_t *curr = R->head;
    while(curr) {
        if (curr->cell->row==r && curr->cell->col==c &&
                curr->cell->counter<=counter+1) return NULL;
        curr = curr->next;
    }

    return create_cell(r,c,counter+1); // cell to insert into the repair queue
}

// Adapted version of the make_empty_list function by Alistair Moffat:
// https://people.eng.unimelb.edu.au/ammoffat/ppsaa/c/listops.c
// Data type and variable names changed
route_t
*make_empty_list(void) {
    route_t *R;
    R = (route_t*)malloc(sizeof(*R));
    assert(R!=NULL);
    R->head = R->tail = NULL;
    return R;
}

// Adapted version of the insert_at_head function by Alistair Moffat:
// https://people.eng.unimelb.edu.au/ammoffat/ppsaa/c/listops.c
// Data type and variable names changed
route_t
*insert_at_head(route_t* R, cell_t* addr) {
    state_t *new;
    new = (state_t*)malloc(sizeof(*new));
    assert(R!=NULL && new!=NULL);
    new->cell = addr;
    new->next = R->head;
    R->head = new;
    if (R->tail==NULL) { /* this is the first insertion into the route */
        R->tail = new;
    }
    return R;
}

// Adapted version of the insert_at_foot function by Alistair Moffat:
// https://people.eng.unimelb.edu.au/ammoffat/ppsaa/c/listops.c
// Data type and variable names changed
route_t 
*insert_at_tail(route_t* R, cell_t* addr) {
    state_t *new;
    new = (state_t*)malloc(sizeof(*new));
    assert(R!=NULL && new!=NULL);
    new->cell = addr;
    new->next = NULL;
    if (R->tail==NULL) { /* this is the first insertion into the route */
        R->head = R->tail = new; 
    } else {
        R->tail->next = new;
        R->tail = new;
    }
    return R;
}

// Adapted version of the free_list function by Alistair Moffat:
// https://people.eng.unimelb.edu.au/ammoffat/ppsaa/c/listops.c
// Data type and variable names changed
void
free_list(route_t* R) {
    state_t *curr, *prev;
    assert(R!=NULL);
    curr = R->head;
    while (curr) {
        prev = curr;
        curr = curr->next;
        free(prev);
    }
    free(R);
}