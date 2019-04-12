/*
 *
 * LiveWallpaper
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Copyright (C) 2012-2016 Maximilian Schnarr <Maximilian.Schnarr@googlemail.com>
 *
 *
 * All simplex noise functions were taken from Stefan Gustavson. You can find
 * the original code here:
 *     http://webstaff.itn.liu.se/~stegu/simplexnoise/SimplexNoise.java
 * The original code was placed in the public domain, so all lw_simplex_noise_*
 * functions are also placed in the public domain.
 *
 * You can find a good description of the simplex noise algorithm here:
 *     http://webstaff.itn.liu.se/~stegu/simplexnoise/simplexnoise.pdf
 */

/**
 * SECTION: noise
 * @Short_description: Functions generating noise
 * @Title: Noise
 *
 * These functions may help you to add some randomness to your live wallpapers.
 */

#include <math.h>
#include <livewallpaper/core.h>

/* Permutation and gradient table */
#define PERM_SIZE 256

static int perm[2 * PERM_SIZE];
static float grad[PERM_SIZE][2];

/* Skewing and unskewing factors */
#define F2 0.36602540f
#define G2 0.21132486f

/**
 * lw_noise_init:
 *
 * Initializes internal tables used by the simplex noise algorithm. Make sure to
 * call this before using the lw_simplex_noise_2f() function. The tables will only
 * be generated once so calling this multiple times has no effect.
 *
 * Since: 0.4
 */
void
lw_noise_init()
{
	static gboolean is_initialized = FALSE;

	if(!is_initialized)
	{
		int i;

		/* Create permutation table */
		for(i = 0; i < PERM_SIZE; i++)
		{
			perm[i] = i;
		}

		/* Make the permutation table random */
		for(i = 0; i < PERM_SIZE; i++)
		{
			int tmp = perm[i];
			int j = rand() % PERM_SIZE;
			perm[i] = perm[j];
			perm[j] = tmp;
		}

		/* Double the permutation table to remove the need for index wrapping */
		for(i = PERM_SIZE; i < 2 * PERM_SIZE; i++)
		{
			perm[i] = perm[i - PERM_SIZE];
		}

		/* Generate random gradient table */
		for(i = 0; i < PERM_SIZE; i++)
		{
			grad[i][0] = rand2f(-1.0f, 1.0f);
			grad[i][1] = rand2f(-1.0f, 1.0f);
		}

		is_initialized = TRUE;
	}
}

/**
 * lw_simplex_noise_2f:
 * @x: A x coordinate
 * @y: A y coordinate
 *
 * Calculates a noise value at the position x, y using the simplex noise algorithm.
 *
 * <note>
 *   <para>This function cannot handle negative coordinates, so make sure that @x >= 0 and @y >= 0.</para>
 * </note>
 *
 * Returns: A noise value in the interval [-1;1]
 *
 * Since: 0.4
 */
float
lw_simplex_noise_2f(float x, float y)
{
	float t;

	/* Noise contributions from the three corners */
	float n0, n1, n2;

	/* The distances from the cell origin in (x,y) unskewed coords */
	float x0, y0, x1, y1, x2, y2;

	/* x and y skewed to (i,j) coords */
	int i, j;

	/* Offsets for second (middle) corner of simplex in (i,j) coords */
	int i1, j1;

	/* Permutation/Gradient table indices */
	int pi, pj, gi0, gi1, gi2;

	/* Skew the input space to determine which simplex cell we're in */
	t = (x + y) * F2;
	i = floor(x + t);
	j = floor(y + t);

	/* Unskew the cell origin back to (x, y) space */
	t = (i + j) * G2;
	/* X0 = i - t; Y0 = j - t; */

	x0 = x - i + t; /* x - X0 */
	y0 = y - j + t; /* y - Y0 */

	/* For the 2D case, the simplex shape is an equilateral triangle.
	 * Determine which simplex we are in. */
	if(x0 > y0)
	{
		/* lower triangle, XY order: (0,0)->(1,0)->(1,1) */
		i1 = 1;
		j1 = 0;
	}
	else
	{
		/* upper triangle, YX order: (0,0)->(0,1)->(1,1) */
		i1 = 0;
		j1 = 1;
	}

	/* A step of (1,0) in (i,j) means a step of (1-G2,-G2) in (x,y) and
	 * a step of (0,1) in (i,j) means a step of (-G2,1-G2) in (x,y). */
	x1 = x0 - i1 + G2;
	y1 = y0 - j1 + G2;
	x2 = x0 - 1.0 + 2.0 * G2;
	y2 = y0 - 1.0 + 2.0 * G2;

	/* Work out the hashed gradient indices of the three simplex corners */
	pi = i % PERM_SIZE;
	pj = j % PERM_SIZE;
	gi0 = perm[pi + perm[pj]];
	gi1 = perm[pi + i1 + perm[pj + j1]];
	gi2 = perm[pi + 1  + perm[pj + 1 ]];

	/* Calculate the contribution from the three corners */
	t = 0.5 - x0*x0 - y0*y0;
	if(t < 0) n0 = 0.0f;
	else
	{
		t *= t;
		n0 = t * t * (grad[gi0][0] * x0 + grad[gi0][1] * y0);
	}

	t = 0.5 - x1*x1 - y1*y1;
	if(t < 0) n1 = 0.0f;
	else
	{
		t *= t;
		n1 = t * t * (grad[gi1][0] * x1 + grad[gi1][1] * y1);
	}

	t = 0.5 - x2*x2 - y2*y2;
	if(t < 0) n2 = 0.0f;
	else
	{
		t *= t;
		n2 = t * t * (grad[gi2][0] * x2 + grad[gi2][1] * y2);
	}

	/* Add contributions from each corner to get the final noise value.
	 * The result is scaled to return values in the interval [-1;1]. */
	return 70.0f * (n0 + n1 + n2);
}

