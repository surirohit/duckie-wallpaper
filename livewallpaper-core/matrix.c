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
 * Copyright (C) 2013-2016 Maximilian Schnarr <Maximilian.Schnarr@googlemail.com>
 *
 */

/* TODO: Create some kind of unit tests for livewallpaper-core and test the matrix implementation */

/**
 * SECTION: matrix
 * @Short_description: A 4x4 matrix class
 *
 * #LwMatrix provides some basic matrix functionality.
 */

#include <math.h>
#include <livewallpaper/core.h>

struct _LwMatrixPrivate
{
	gfloat *data;
	GSList *stack;
};

/**
 * LwMatrix:
 *
 * This structure represents a 4x4 matrix.
 *
 * Since: 0.5
 */

G_DEFINE_TYPE(LwMatrix, lw_matrix, G_TYPE_OBJECT)

gfloat identity[] = {1, 0, 0, 0,
                     0, 1, 0, 0,
                     0, 0, 1, 0,
                     0, 0, 0, 1};

/**
 * lw_matrix_new:
 *
 * Creates a new matrix.
 *
 * Returns: A new 4x4 identity matrix
 *
 * Since: 0.5
 */
LwMatrix*
lw_matrix_new()
{
	return g_object_new(LW_TYPE_MATRIX, NULL);
}

/**
 * lw_matrix_get_elements:
 * @self: A #LwMatrix
 *
 * Returns: (array fixed-size=16) (transfer none): The elements of the matrix as array
 *
 * Since: 0.5
 */
gfloat*
lw_matrix_get_elements(LwMatrix *self)
{
	return self->priv->data;
}

static void
lw_matrix_multiply_real(gfloat *a, gfloat *b)
{
	gfloat *t = g_slice_copy(sizeof(identity), a);

	/*
	 * The following matrix multiplication code was generated by these loops:
	 *     int i, j, r, c;
	 *     for(i = 0; i < 16; i++)
	 *     {
	 *         r = i / 4;
	 *         c = i % 4;
	 *         printf("a[%2d] =", i);
	 *         for(j = 0; j < 4; j++)
	 *         {
	 *             printf(" t[%2d] * b[%2d]", 4 * r + j, c + 4 * j);
	 *             if(j != 3) printf(" +");
	 *         }
	 *         printf(";\n");
	 *     }
	 */

	a[ 0] = t[ 0] * b[ 0] + t[ 1] * b[ 4] + t[ 2] * b[ 8] + t[ 3] * b[12];
	a[ 1] = t[ 0] * b[ 1] + t[ 1] * b[ 5] + t[ 2] * b[ 9] + t[ 3] * b[13];
	a[ 2] = t[ 0] * b[ 2] + t[ 1] * b[ 6] + t[ 2] * b[10] + t[ 3] * b[14];
	a[ 3] = t[ 0] * b[ 3] + t[ 1] * b[ 7] + t[ 2] * b[11] + t[ 3] * b[15];
	a[ 4] = t[ 4] * b[ 0] + t[ 5] * b[ 4] + t[ 6] * b[ 8] + t[ 7] * b[12];
	a[ 5] = t[ 4] * b[ 1] + t[ 5] * b[ 5] + t[ 6] * b[ 9] + t[ 7] * b[13];
	a[ 6] = t[ 4] * b[ 2] + t[ 5] * b[ 6] + t[ 6] * b[10] + t[ 7] * b[14];
	a[ 7] = t[ 4] * b[ 3] + t[ 5] * b[ 7] + t[ 6] * b[11] + t[ 7] * b[15];
	a[ 8] = t[ 8] * b[ 0] + t[ 9] * b[ 4] + t[10] * b[ 8] + t[11] * b[12];
	a[ 9] = t[ 8] * b[ 1] + t[ 9] * b[ 5] + t[10] * b[ 9] + t[11] * b[13];
	a[10] = t[ 8] * b[ 2] + t[ 9] * b[ 6] + t[10] * b[10] + t[11] * b[14];
	a[11] = t[ 8] * b[ 3] + t[ 9] * b[ 7] + t[10] * b[11] + t[11] * b[15];
	a[12] = t[12] * b[ 0] + t[13] * b[ 4] + t[14] * b[ 8] + t[15] * b[12];
	a[13] = t[12] * b[ 1] + t[13] * b[ 5] + t[14] * b[ 9] + t[15] * b[13];
	a[14] = t[12] * b[ 2] + t[13] * b[ 6] + t[14] * b[10] + t[15] * b[14];
	a[15] = t[12] * b[ 3] + t[13] * b[ 7] + t[14] * b[11] + t[15] * b[15];

	g_slice_free1(sizeof(identity), t);
}

#if 0
/* This function is used for debugging */
void
lw_matrix_print(LwMatrix *self)
{
	gfloat *m = self->priv->data;
	int i;

	for(i = 0; i < 16; i++)
	{
		printf("%.2f ", m[i]);
		if(i % 4 == 3) printf("\n");
	}
}
#endif

/**
 * lw_matrix_ortho:
 * @self: A #LwMatrix
 * @left: Coordinate of the left vertical clipping plane
 * @right: Coordinate of the right vertical clipping plane
 * @bottom: Coordinate of the bottom horizontal clipping plane
 * @top: Coordinate of the top horizontal clipping plane
 * @nearVal: Distance to the nearer depth clipping plane. A negative value means that the plane is to be behind the viewer.
 * @farVal: Distance to the farther depth clipping plane. A negative value means that the plane is to be behind the viewer.
 *
 * Multiplies the current matrix by an orthographic matrix. This tranformation produces a
 * parallel projection.
 *
 * This function is a replacement for the OpenGL function
 * <ulink url="http://www.opengl.org/sdk/docs/man2/xhtml/glOrtho.xml">glOrtho</ulink>.
 *
 * Since: 0.5
 */
void
lw_matrix_ortho(LwMatrix *self,
                gfloat left, gfloat right,
                gfloat bottom, gfloat top,
                gfloat nearVal, gfloat farVal)
{
	gfloat *o;

	g_return_if_fail(left != right);
	g_return_if_fail(bottom != top);
	g_return_if_fail(nearVal != farVal);
	g_return_if_fail(nearVal >= 0 && farVal >= 0);

	o = g_slice_alloc0(sizeof(identity));

	o[ 0] = 2.0f / (right - left);
	o[ 3] = - (right + left) / (right - left);
	o[ 5] = 2.0f / (top - bottom);
	o[ 7] = - (top + bottom) / (top - bottom);
	o[10] = - 2.0f / (farVal - nearVal);
	o[11] = - (farVal + nearVal) / (farVal - nearVal);
	o[15] = 1.0f;

	lw_matrix_multiply_real(self->priv->data, o);
	g_slice_free1(sizeof(identity), o);
}

/**
 * lw_matrix_frustum:
 * @self: A #LwMatrix
 * @left: Coordinate of the left vertical clipping plane
 * @right: Coordinate of the right vertical clipping plane
 * @bottom: Coordinate of the bottom horizontal clipping plane
 * @top: Coordinate of the top horizontal clipping plane
 * @nearVal: Distance to the nearer depth clipping plane. @nearVal must be positive
 * @farVal: Distance to the farther depth clipping plane. @farVal must be positive
 *
 * Multiplies the current matrix by a perspective matrix. This transformation produces a
 * perspective projection.
 *
 * This function is a replacement for the OpenGL function
 * <ulink url="http://www.opengl.org/sdk/docs/man2/xhtml/glFrustum.xml">glFrustum</ulink>.
 *
 * Since: 0.5
 */
void
lw_matrix_frustum(LwMatrix *self,
                  gfloat left, gfloat right,
                  gfloat bottom, gfloat top,
                  gfloat nearVal, gfloat farVal)
{
	gfloat *f;

	g_return_if_fail(left != right);
	g_return_if_fail(bottom != top);
	g_return_if_fail(nearVal != farVal);

	f = g_slice_alloc0(sizeof(identity));

	f[ 0] = 2.0f * nearVal / (right - left);
	f[ 2] = (right + left) / (right - left);
	f[ 5] = 2.0f * nearVal / (top - bottom);
	f[ 6] = (top + bottom) / (top - bottom);
	f[10] = - (farVal + nearVal) / (farVal - nearVal);
	f[11] = - 2.0f * farVal * nearVal / (farVal - nearVal);
	f[14] = - 1.0f;

	lw_matrix_multiply_real(self->priv->data, f);
	g_slice_free1(sizeof(identity), f);
}

/**
 * lw_matrix_translate:
 * @self: A #LwMatrix
 * @x: Translation in x direction
 * @y: Translation in y direction
 * @z: Translation in z direction
 *
 * Multiplies the current matrix by a translation matrix. The tranlation matrix produces a
 * translation by (@x, @y, @z).
 *
 * This function is a replacement for the OpenGL function
 * <ulink url="http://www.opengl.org/sdk/docs/man2/xhtml/glTranslate.xml">glTranslate</ulink>.
 *
 * Since: 0.5
 */
void
lw_matrix_translate(LwMatrix *self, gfloat x, gfloat y, gfloat z)
{
	gfloat *m = self->priv->data;

	m[ 3] = x * m[ 0] + y * m[ 1] + z * m[ 2] + m[ 3];
	m[ 7] = x * m[ 4] + y * m[ 5] + z * m[ 6] + m[ 7];
	m[11] = x * m[ 8] + y * m[ 9] + z * m[10] + m[11];
	m[15] = x * m[12] + y * m[13] + z * m[14] + m[15];
}

/**
 * lw_matrix_rotate:
 * @self: A #LwMatrix
 * @angle: Angle of rotation in radians
 * @x: x coordinate of a vector
 * @y: y coordinate of a vector
 * @z: z coordinate of a vector
 *
 * Multiplies the current matrix by a rotation matrix. The rotation matrix produces a rotation
 * around the vector (@x, @y, @z) by @angle degrees.
 *
 * This function is a replacement for the OpenGL function
 * <ulink url="http://www.opengl.org/sdk/docs/man2/xhtml/glRotate.xml">glRotate</ulink>.
 *
 * Since: 0.5
 */
void
lw_matrix_rotate(LwMatrix *self, gfloat angle, gfloat x, gfloat y, gfloat z)
{
	gfloat *r = g_slice_alloc0(sizeof(identity));
	gfloat c = cos(angle), s = sin(angle), t = 1.0f - c;

	/* normalize vector */
	gfloat magnitude = sqrt(x*x + y*y + z*z);
	x = x / magnitude;
	y = y / magnitude;
	z = z / magnitude;

	r[ 0] = x*x*t + c  ; r[ 1] = x*y*t - z*s; r[ 2] = x*z*t + y*s;
	r[ 4] = y*x*t + z*s; r[ 5] = y*y*t + c  ; r[ 6] = y*z*t - x*s;
	r[ 8] = z*x*t - y*s; r[ 9] = z*y*t + x*s; r[10] = z*z*t + c  ;
	r[15] = 1.0f;

	lw_matrix_multiply_real(self->priv->data, r);
	g_slice_free1(sizeof(identity), r);
}

/**
 * lw_matrix_scale:
 * @self: A #LwMatrix
 * @x: Scale factor along the x axis
 * @y: Scale factor along the y axis
 * @z: Scale factor along the z axis
 *
 * Multiplies the current matrix by a scaling matrix. The scaling matrix produces a nonuniform
 * scaling along the x, y and z axes.
 *
 * This function is a replacement for the OpenGL function
 * <ulink url="http://www.opengl.org/sdk/docs/man2/xhtml/glScale.xml">glScale</ulink>.
 *
 * Since: 0.5
 */
void
lw_matrix_scale(LwMatrix *self, gfloat x, gfloat y, gfloat z)
{
	gfloat *m = self->priv->data;

	m[ 0] = x * m[ 0]; m[ 1] = y * m[ 1]; m[ 2] = z * m[ 2];
	m[ 4] = x * m[ 4]; m[ 5] = y * m[ 5]; m[ 6] = z * m[ 6];
	m[ 8] = x * m[ 8]; m[ 9] = y * m[ 9]; m[10] = z * m[10];
	m[12] = x * m[12]; m[13] = y * m[13]; m[14] = z * m[14];
}

/**
 * lw_matrix_multiply:
 * @self: A #LwMatrix
 * @other: Another #LwMatrix
 *
 * Multiplies @self with @other and stores the result in @self.
 *
 * Since: 0.5
 */
void
lw_matrix_multiply(LwMatrix *self, LwMatrix *other)
{
	lw_matrix_multiply_real(self->priv->data, other->priv->data);
}

/**
 * lw_matrix_push:
 * @self: A #LwMatrix
 *
 * Pushes the data of the current matrix on top of the stack.
 *
 * This function is a replacement for the OpenGL function
 * <ulink url="http://www.opengl.org/sdk/docs/man2/xhtml/glPushMatrix.xml">glPushMatrix</ulink>.
 *
 * Since: 0.5
 */
void
lw_matrix_push(LwMatrix *self)
{
	self->priv->stack = g_slist_prepend(self->priv->stack,
	                                    g_slice_copy(sizeof(identity),
	                                                 self->priv->data));
}

/**
 * lw_matrix_pop:
 * @self: A #LwMatrix
 *
 * Replaces the data of the current matrix with the data on top of the stack.
 *
 * This function is a replacement for the OpenGL function
 * <ulink url="http://www.opengl.org/sdk/docs/man2/xhtml/glPopMatrix.xml">glPopMatrix</ulink>.
 *
 * Since: 0.5
 */
void
lw_matrix_pop(LwMatrix *self)
{
	if(self->priv->stack == NULL)
	{
		g_warning("lw_matrix_pop(): The stack is empty. Make sure to call lw_matrix_push() first.");
		return;
	}

	/* Free current matrix */
	g_slice_free1(sizeof(identity), self->priv->data);

	/* Get matrix on top of the stack */
	self->priv->data = self->priv->stack->data;

	/* Free first node of the stack */
	self->priv->stack = g_slist_delete_link(self->priv->stack,
	                                        self->priv->stack);
}

static void
lw_matrix_init(LwMatrix *self)
{
	self->priv = G_TYPE_INSTANCE_GET_PRIVATE(self, LW_TYPE_MATRIX,
	                                         LwMatrixPrivate);

	self->priv->data = g_slice_copy(sizeof(identity), &identity);
}

static void
lw_matrix_data_free(gpointer data)
{
	g_slice_free1(sizeof(identity), data);
}

static void
lw_matrix_finalize(GObject *object)
{
	LwMatrix *self = LW_MATRIX(object);

	g_slice_free1(sizeof(identity), self->priv->data);
	g_slist_free_full(self->priv->stack, lw_matrix_data_free);

	/* Chain up to the parent class */
	G_OBJECT_CLASS(lw_matrix_parent_class)->finalize(object);
}

static void
lw_matrix_class_init(LwMatrixClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

	gobject_class->finalize = lw_matrix_finalize;

	g_type_class_add_private(klass, sizeof(LwMatrixPrivate));
}
