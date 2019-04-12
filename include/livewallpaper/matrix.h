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

#ifndef _LW_MATRIX_H_
#define _LW_MATRIX_H_

G_BEGIN_DECLS

#define LW_TYPE_MATRIX            (lw_matrix_get_type())
#define LW_MATRIX(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), LW_TYPE_MATRIX, LwMatrix))
#define LW_IS_MATRIX(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), LW_TYPE_MATRIX))
#define LW_MATRIX_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), LW_TYPE_MATRIX, LwMatrixClass))
#define LW_IS_MATRIX_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), LW_TYPE_MATRIX))
#define LW_MATRIX_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), LW_TYPE_MATRIX, LwMatrixClass))

typedef struct _LwMatrix LwMatrix;
typedef struct _LwMatrixClass LwMatrixClass;

typedef struct _LwMatrixPrivate LwMatrixPrivate;

struct _LwMatrix
{
	/*< private >*/
	GObject parent_instance;

	LwMatrixPrivate *priv;
};

struct _LwMatrixClass
{
	/*< private >*/
	GObjectClass parent_class;
};

GType lw_matrix_get_type(void);

LwMatrix *lw_matrix_new();

gfloat *lw_matrix_get_elements(LwMatrix *self);

void lw_matrix_ortho(LwMatrix *self,
                     gfloat left, gfloat right,
                     gfloat bottom, gfloat top,
                     gfloat nearVal, gfloat farVal);
void lw_matrix_frustum(LwMatrix *self,
                     gfloat left, gfloat right,
                     gfloat bottom, gfloat top,
                     gfloat nearVal, gfloat farVal);

void lw_matrix_translate(LwMatrix *self, gfloat x, gfloat y, gfloat z);
void lw_matrix_rotate(LwMatrix *self, gfloat angle, gfloat x, gfloat y, gfloat z);
void lw_matrix_scale(LwMatrix *self, gfloat x, gfloat y, gfloat z);

void lw_matrix_multiply(LwMatrix *self, LwMatrix *other);

void lw_matrix_push(LwMatrix *self);
void lw_matrix_pop(LwMatrix *self);

G_END_DECLS

#endif /* _LW_MATRIX_H_ */

