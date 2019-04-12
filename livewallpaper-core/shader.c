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

/**
 * SECTION: shader
 * @Short_description: OpenGL shader wrapper
 *
 * #LwShader makes it easy to load and create OpenGL shaders. You can avoid using this structure
 * if you use the lw_program_create_and_attach_shader() function.
 *
 * <note>
 *   <para>
 *     LwShader is not finished yet. It is possible that parts of this type
 *     will be changed in a future version of LiveWallpaper.
 *   </para>
 * </note>
 *
 * <example>
 *   <title>Using LwShader</title>
 *   <programlisting>
 * GError *error = NULL;
 * LwProgram *prog = g_object_new(LW_TYPE_PROGRAM, NULL);
 *
 * LwShader *shader = lw_shader_new_from_uri("/path/to/shader.glsl", GL_FRAGMENT_SHADER, &error);
 * if(error != NULL)
 * {
 *     g_warning("Could not load fragment shader: %s", error->message);
 *     g_error_free(error);
 *     return;
 * }
 * else
 * {
 *     lw_shader_compile(shader);
 *     lw_program_attach_shader(prog, shader);
 * }
 * g_object_unref(shader);
 *
 * lw_program_link(prog);
 *
 * ...</programlisting>
 * </example>
 */

#include <livewallpaper/core.h>
#include <string.h>

struct _LwShaderPrivate
{
	guint name;
	guint type;
};

/**
 * LwShader:
 *
 * A structure for easier OpenGL shader handling.
 *
 * Since: 0.4
 */

G_DEFINE_TYPE(LwShader, lw_shader, G_TYPE_OBJECT)

/**
 * lw_shader_new_from_uri:
 * @uri: The uri of the file containing the shader's source code
 * @type: GL_VERTEX_SHADER or GL_FRAGMENT_SHADER
 *
 * Creates a new shader and stores the content of the file as source code in
 * in the shader. This function will return %NULL if it cannot get the source
 * code from the specified file.
 * The uri has to begin with the protocol, by instance file:// or resource://
 *
 * Returns: A new #LwShader or %NULL if an error occured. You should use
 *          g_object_unref() to free the #LwShader.
 *
 * Since: 0.5
 */
LwShader*
lw_shader_new_from_uri(const gchar *uri, guint type)
{
    GError *error = NULL;
	LwShader *shader;
	gchar *source;

    GFile *f = g_file_new_for_uri (uri);

	g_file_load_contents (f, NULL, &source, NULL, NULL, &error);
    if (error != NULL)
    {
	    g_critical("Could not load shader: %s", error->message);
	    g_error_free(error);
        return NULL;
    }
    
	shader = lw_shader_new_from_string(source, type);
	g_free(source);

	return shader;
}

/**
 * lw_shader_new_from_string:
 * @source: The source code of the shader
 * @type: GL_VERTEX_SHADER or GL_FRAGMENT_SHADER
 *
 * Creates a new shader and stores the source code in the shader.
 *
 * Returns: A new #LwShader. You should use g_object_unref() to free the #LwShader.
 *
 * Since: 0.4
 */
LwShader*
lw_shader_new_from_string(const gchar *source, guint type)
{
	LwShader *shader = g_object_new(LW_TYPE_SHADER, NULL);
	int source_length = strlen(source);

	shader->priv->name = LW_OPENGL_1_4_HELPER(glCreateShader, glCreateShaderObjectARB, (type));
	shader->priv->type = type;

	LW_OPENGL_1_4_HELPER(glShaderSource, glShaderSourceARB, (shader->priv->name, 1, &source, &source_length));

	return shader;
}

/**
 * lw_shader_get_name:
 * @self: A #LwShader
 *
 * Returns: The shader name returned by <ulink url="http://www.opengl.org/sdk/docs/man/xhtml/glCreateShader.xml">glCreateShader</ulink>
 *
 * Since: 0.4
 */
guint
lw_shader_get_name(LwShader *self)
{
	return self->priv->name;
}

/**
 * lw_shader_get_shader_type:
 * @self: A #LwShader
 *
 * Returns: The type of the shader, either GL_VERTEX_SHADER or GL_FRAGMENT_SHADER
 *
 * Since: 0.4
 */
guint
lw_shader_get_shader_type(LwShader *self)
{
	return self->priv->type;
}

/**
 * lw_shader_compile:
 * @self: A #LwShader
 *
 * Compiles the source code of the shader. This function prints a warning
 * if the compilation of the shader fails.
 *
 * Returns: %TRUE on success, %FALSE if the compilation failed
 *
 * Since: 0.4
 */
gboolean
lw_shader_compile(LwShader *self)
{
	int status;

	LW_OPENGL_1_4_HELPER(glCompileShader, glCompileShaderARB, (self->priv->name));

	/* Handle errors */
	LW_OPENGL_1_4_HELPER(glGetShaderiv, glGetObjectParameterivARB, (self->priv->name, GL_COMPILE_STATUS, &status));
	if(status == GL_FALSE)
	{
		int log_length;
		gchar *log_buffer;

		LW_OPENGL_1_4_HELPER(glGetShaderiv, glGetObjectParameterivARB, (self->priv->name, GL_INFO_LOG_LENGTH, &log_length));
		log_buffer = g_malloc(log_length * sizeof(gchar));

		LW_OPENGL_1_4_HELPER(glGetShaderInfoLog, glGetInfoLogARB, (self->priv->name, log_length, NULL, log_buffer));

		g_warning("%s", log_buffer);

		g_free(log_buffer);
		return FALSE;
	}

	return TRUE;
}

static void
lw_shader_init(LwShader *self)
{
	self->priv = G_TYPE_INSTANCE_GET_PRIVATE(self, LW_TYPE_SHADER,
	                                         LwShaderPrivate);

	self->priv->name = 0;
}

static void
lw_shader_finalize(GObject *object)
{
	LwShader *self = LW_SHADER(object);

	if(self->priv->name)
		LW_OPENGL_1_4_HELPER(glDeleteShader, glDeleteObjectARB, (self->priv->name));

	/* Chain up to the parent class */
	G_OBJECT_CLASS(lw_shader_parent_class)->finalize(object);
}

static void
lw_shader_class_init(LwShaderClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

	gobject_class->finalize = lw_shader_finalize;

	g_type_class_add_private(klass, sizeof(LwShaderPrivate));
}

