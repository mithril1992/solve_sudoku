#include <stdio.h>
#include <stdlib.h>
#include "solver.h"

//16bit専用
unsigned int bitcount(uint16_t bits)
{
	bits = (bits & 0x5555) + (bits >> 1 & 0x5555);
	bits = (bits & 0x3333) + (bits >> 2 & 0x3333);
	bits = (bits & 0x0f0f) + (bits >> 4 & 0x0f0f);
	return (bits & 0x00ff) + (bits >> 8 & 0x00ff);
}

uint16_t grid_at(const uint16_t* grid, unsigned int x, unsigned int y)
{
	return grid[9 * x + y];
}

int is_solved(uint16_t flag)
{
	return bitcount(flag) == 1;
}

int is_on_flag(uint16_t flag, unsigned int num)
{
	if(num == 0 || num > 9)
		return 0;
	uint16_t mask = 1 << (num - 1);
	return (mask & flag) > 0;
}

uint16_t set_flag(uint16_t flag, unsigned int n)
{
	if(n == 0)
		return flag;
	uint16_t add_flag = 1 << (n - 1);
	uint16_t return_flag = flag | add_flag;
	return return_flag;
}

unsigned int base_matrix(unsigned int index)
{
	unsigned int oder = index / 3;
	return oder * 3;
}

//グリッドと行を指定して、その行にまだ入っていない数字のフラグを得る
uint16_t mask_row(const uint16_t* grid, unsigned int row)
{
	//その行で解決済みの数字
	uint16_t flag = 0;

	//行を走査
	for(unsigned int i = 0; i < 9; i++)
	{
		uint16_t item = grid_at(grid, row, i);

		//そのセルが既に数字が確定している場合
		if(bitcount(item) < 2)
			flag |= item;
	}

	//未確定な数字のフラグ
	return ~flag & 0x1FF;
}

//グリッドと列を指定して、その列にまだ入っていない数字のフラグを得る
uint16_t mask_column(const uint16_t* grid, unsigned int column)
{
	//列内で解決済みの数
	uint16_t flag = 0;

	//列を走査
	for(unsigned int i = 0; i < 9; i++)
	{
		uint16_t item = grid_at(grid, i, column);

		//そのセルが既に数字が確定している場合
		if(bitcount(item) < 2)
			flag |= item;
	}

	//未確定な数字のフラグを返す
	return ~flag & 0x1FF;
}

/*
	グリッドとセルの座標を指定して、
	そのセルが含まれるマトリクスに
 	まだ入っていない数字のフラグを得る
*/

uint16_t mask_matrix(const uint16_t* grid, unsigned int x, unsigned int y)
{
	//マトリクスの右上のセルの座標
	/*
	baseMatrix x
		| x `elem` [0..2] = 0
		| x `elem` [3..5] = 3
		| x `elem` [6..8] = 6
	*/
	unsigned int base_x = base_matrix(x);
	unsigned int base_y = base_matrix(y);

	// マトリクス内で解決済みの数
	uint16_t flag = 0;

	//セルを走査
	for(int i = 0; i < 3; i++)
	{
		for(int j = 0; j < 3; j++)
		{
			uint16_t item = grid_at(grid, base_x + i, base_y + j);

			//そのセルが既に数字が確定している場合
			if(bitcount(item) < 2)
				flag |= item;
		}
	}

	//未確定な数字のフラグを返す
	return ~flag & 0x1FF;
}

//セルに入りうる値を得る
uint16_t mask_cell(const uint16_t* grid, unsigned int x, unsigned int y)
{
	uint16_t row_flag = mask_row(grid, x);
	uint16_t column_flag = mask_column(grid, y);
	uint16_t matrix_flag = mask_matrix(grid, x, y);
	uint16_t flag = row_flag & column_flag & matrix_flag;
	return flag;
}

//初期化処理
uint16_t* init_grid(uint16_t* grid)
{
	for(unsigned int i = 0; i < 81; i++)
		if(grid[i] == 0)
			grid[i] = mask_cell(grid, i / 9, i % 9);
	return grid;
}

uint16_t* solve_grid(uint16_t* grid)
{
	// 値の変化を検出するフラグ
	unsigned int vary_flag = 0;
	//未解決のセルが存在する場合のフラグ
	int unsolved_index = -1;

	// 全セルの値が変わらなくなるまでループ
	do {
		// フラグの初期化
		vary_flag = 0;
		unsolved_index = -1;

		// グリッドを走査
		for(unsigned int i = 0; i < 81; i++)
		{
			uint16_t current_value = grid_at(grid, i / 9, i % 9);
			uint16_t new_value = current_value;

			// 未確定のセルに関して処理
			if(!is_solved(grid[i]))
			{
				new_value = mask_cell(grid, i / 9, i % 9);

				// 値の変化があればフラグを建てる
				if(current_value != new_value)
				{
					grid[i] = new_value;
					vary_flag = 1;
				}
			}

			// 未解決のセルであればフラグを建てる
			if(!is_solved(new_value))
				unsolved_index = i;
		}
	} while(vary_flag);

	// 未解決のセルが存在する場合
	if(unsolved_index >= 0)
	{
		//可能性のある数字を埋めてsolve
		uint16_t item = grid[unsolved_index];
		for(int i = 1; i < 10; i++)
		{
			if(is_on_flag(item, i))
			{
				grid[unsolved_index] = set_flag(0, i);
				break;
			}
		}

		printf("------------------\n");
		view_grid(grid);
		return solve_grid(grid);
	}

	return grid;
}

void view_grid(const uint16_t* grid)
{
	for(int i = 0; i < 81; i++)
	{
		int number = 0;
		for(int j = 0; j < 9; j++)
		{
			if((grid[i] & (1 << j)) > 0)
			{
				number = j + 1;
				break;
			}
		}

		if(number == 0)
			printf("-");
		else
			printf("%d", number);

		if(i % 9 == 8)
			printf("\n");
		else
			printf(" ");
	}
}
