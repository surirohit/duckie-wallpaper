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
 */

#ifndef _LW_PROGRAM_H_
#define _LW_PROGRAM_H_

G_BEGIN_DECLS

typedef enum
{
	LW_GLSL_TYPE_FLOAT,
	LW_GLSL_TYPE_VEC2,
	LW_GLSL_TYPE_VEC3,
	LW_GLSL_TYPE_VEC4,

	LW_GLSL_TYPE_INT,
	LW_GLSL_TYPE_IVEC2,
	LW_GLSL_TYPE_IVEC3,
	LW_GLSL_TYPE_IVEC4,

	LW_GLSL_TYPE_BOOL,
	LW_GLSL_TYPE_BVEC2,
	LW_GLSL_TYPE_BVEC3,
	LW_GLSL_TYPE_BVEC4
} LwGLSLType;

#define LW_TYPE_PROGRAM            (lw_program_get_type())
#define LW_PROGRAM(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), LW_TYPE_PROGRAM, LwProgram))
#define LW_IS_PROGRAM(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), LW_TYPE_PROGRAM))
#define LW_PROGRAM_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), LW_TYPE_PROGRAM, LwProgramClass))
#define LW_IS_PROGRAM_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), LW_TYPE_PROGRAM))
#define LW_PROGRAM_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), LW_TYPE_PROGRAM, LwProgramClass))

typedef struct _LwProgram LwProgram;
typedef struct _LwProgramClass LwProgramClass;

typedef struct _LwProgramPrivate LwProgramPrivate;

struct _LwProgram
{
	/*< private >*/
	GObject parent_instance;

	LwProgramPrivate *priv;
};

struct _LwProgramClass
{
	/*< private >*/
	GObjectClass parent_class;
};

GType lw_program_get_type(void);

guint lw_program_get_name(LwProgram *self);

void lw_program_attach_shader(LwProgram *self, LwShader *shader);
gboolean lw_program_link(LwProgram *self);

gint lw_program_get_attrib_location(LwProgram *self, const gchar *name);
gint lw_program_get_uniform_location(LwProgram *self, const gchar *name);

void lw_program_set_attribute(LwProgram *self, const gchar *name,
                              LwGLSLType type, LwBuffer *buffer);

void lw_program_set_texture(LwProgram *self, const gchar *name, LwTexture *texture);
void lw_program_set_matrix(LwProgram *self, const gchar *name, LwMatrix *matrix);

void lw_program_enable(LwProgram *self);
void lw_program_disable(LwProgram *self G_GNUC_UNUSED);

gboolean lw_program_create_and_attach_shader(LwProgram *self, const gchar *path,
                                             guint type);
gboolean lw_program_create_and_attach_shader_from_resource (LwProgram *self,
                                                            const gchar *path,
                                                            guint type);

G_END_DECLS

#endif /* _LW_PROGRAM_H_ */

