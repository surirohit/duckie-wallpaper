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

#ifndef _LW_SHADER_H_
#define _LW_SHADER_H_

G_BEGIN_DECLS

#define LW_TYPE_SHADER            (lw_shader_get_type())
#define LW_SHADER(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), LW_TYPE_SHADER, LwShader))
#define LW_IS_SHADER(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), LW_TYPE_SHADER))
#define LW_SHADER_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), LW_TYPE_SHADER, LwShaderClass))
#define LW_IS_SHADER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), LW_TYPE_SHADER))
#define LW_SHADER_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), LW_TYPE_SHADER, LwShaderClass))

typedef struct _LwShader LwShader;
typedef struct _LwShaderClass LwShaderClass;

typedef struct _LwShaderPrivate LwShaderPrivate;

struct _LwShader
{
	/*< private >*/
	GObject parent_instance;

	LwShaderPrivate *priv;
};

struct _LwShaderClass
{
	/*< private >*/
	GObjectClass parent_class;
};

GType lw_shader_get_type(void);

LwShader *lw_shader_new_from_uri(const gchar *uri, guint type);
LwShader *lw_shader_new_from_string(const gchar *source, guint type);

guint lw_shader_get_name(LwShader *self);
guint lw_shader_get_shader_type(LwShader *self);

gboolean lw_shader_compile(LwShader *self);

G_END_DECLS

#endif /* _LW_SHADER_H_ */

