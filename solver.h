#ifndef SUDOKU_SOLVER_H
#define SUDOKU_SOLVER_H
#include <stdint.h>

void view_grid(const uint16_t* grid);
uint16_t* solve_grid(uint16_t* grid);
uint16_t* init_grid(uint16_t* grid);
int is_on_flag(uint16_t flag, unsigned int num);
int is_solved(uint16_t flag);
//nビット目にフラグを建てる
uint16_t set_flag(uint16_t flag, unsigned int n);

#endif