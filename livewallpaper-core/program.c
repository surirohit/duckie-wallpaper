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
 * SECTION: program
 * @Short_description: OpenGL program wrapper
 *
 * #LwProgram is an easy way to use GLSL shaders in a LiveWallpaper plugin.
 *
 * <note>
 *   <para>
 *     LwProgram is not finished yet. It is possible that parts of this type
 *     will be changed in a future version of LiveWallpaper.
 *   </para>
 * </note>
 *
 * <example>
 *   <title>Using LwProgram</title>
 *   <programlisting>
 * GError *error = NULL;
 * LwProgram *prog = g_object_new(LW_TYPE_PROGRAM, NULL);
 *
 * lw_program_create_and_attach_shader(prog, "/path/to/shader.glsl", GL_VERTEX_SHADER, &error);
 * if(error != NULL)
 * {
 *     g_warning("Could not load vertex shader: %s", error->message);
 *     g_error_free(error);
 *     return;
 * }
 *
 * lw_program_link(prog);
 *
 * ...
 *
 * lw_prog_enable(prog);
 *
 * // The program is enabled for all following drawing operations
 * ...
 *
 * lw_prog_disable(prog);</programlisting>
 * </example>
 *
 * The noise plugin makes use of the #LwProgram object. Take a look at the source code of that
 * plugin to see a full working example for #LwProgram.
 */

#include <string.h>
#include <livewallpaper/core.h>

struct _LwProgramPrivate
{
	guint name;

	GArray *tex_units;
};

/**
 * LwGLSLType:
 * @LW_GLSL_TYPE_FLOAT: Represents the GLSL type float
 * @LW_GLSL_TYPE_VEC2: Represents the GLSL type vec2
 * @LW_GLSL_TYPE_VEC3: Represents the GLSL type vec3
 * @LW_GLSL_TYPE_VEC4: Represents the GLSL type vec4
 * @LW_GLSL_TYPE_INT: Represents the GLSL type int
 * @LW_GLSL_TYPE_IVEC2: Represents the GLSL type ivec2
 * @LW_GLSL_TYPE_IVEC3: Represents the GLSL type ivec3
 * @LW_GLSL_TYPE_IVEC4: Represents the GLSL type ivec4
 * @LW_GLSL_TYPE_BOOL: Represents the GLSL type bool
 * @LW_GLSL_TYPE_BVEC2: Represents the GLSL type bvec2
 * @LW_GLSL_TYPE_BVEC3: Represents the GLSL type bvec3
 * @LW_GLSL_TYPE_BVEC4: Represents the GLSL type bvec4
 *
 * Since: 0.5
 */

static void
array_clear_func(gpointer pointer)
{
	g_free(*(gchar**)pointer);
}

/**
 * LwProgram:
 *
 * A structure for easier OpenGL program handling.
 *
 * Since: 0.4
 */

G_DEFINE_TYPE(LwProgram, lw_program, G_TYPE_OBJECT)

/**
 * lw_program_get_name:
 * @self: A #LwProgram
 *
 * Returns: The program name returned by <ulink url="http://www.opengl.org/sdk/docs/man/xhtml/glCreateProgram.xml">glCreateProgram</ulink>
 *
 * Since: 0.4
 */
guint
lw_program_get_name(LwProgram *self)
{
	return self->priv->name;
}

/**
 * lw_program_attach_shader:
 * @self: A #LwProgram
 * @shader: A #LwShader
 *
 * Attaches the shader to the program. This function uses <ulink url="http://www.opengl.org/sdk/docs/man/xhtml/glAttachShader.xml">glAttachShader</ulink>
 * to attach the shader. The #LwShader can be freed after attaching it to a #LwProgram.
 *
 * Since: 0.4
 */
void
lw_program_attach_shader(LwProgram *self, LwShader *shader)
{
	LW_OPENGL_1_4_HELPER(glAttachShader, glAttachObjectARB, (self->priv->name, lw_shader_get_name(shader)));
}

/**
 * lw_program_link:
 * @self: A #LwProgram
 *
 * Links the program using <ulink url="http://www.opengl.org/sdk/docs/man/xhtml/glLinkProgram.xml">glLinkProgram</ulink>.
 * If an error occurs, this function returns %FALSE and prints a warning.
 *
 * Returns: %TRUE on success, %FALSE if an error occured
 *
 * Since: 0.4
 */
gboolean
lw_program_link(LwProgram *self)
{
	int status;

	LW_OPENGL_1_4_HELPER(glLinkProgram, glLinkProgramARB, (self->priv->name));

	/* Reset tex units */
	g_array_free(self->priv->tex_units, TRUE);
	self->priv->tex_units = g_array_new(FALSE, FALSE, sizeof(gchar*));
	g_array_set_clear_func(self->priv->tex_units, array_clear_func);

	/* Handle errors */
	LW_OPENGL_1_4_HELPER(glGetProgramiv, glGetObjectParameterivARB, (self->priv->name, GL_LINK_STATUS, &status));
	if(status == GL_FALSE)
	{
		int log_length;
		gchar *log_buffer;

		LW_OPENGL_1_4_HELPER(glGetProgramiv, glGetObjectParameterivARB, (self->priv->name, GL_INFO_LOG_LENGTH, &log_length));
		log_buffer = g_malloc(log_length * sizeof(gchar));

		LW_OPENGL_1_4_HELPER(glGetProgramInfoLog, glGetInfoLogARB, (self->priv->name, log_length, NULL, log_buffer));

		g_warning("%s", log_buffer);

		g_free(log_buffer);
		return FALSE;
	}

	return TRUE;
}

/**
 * lw_program_create_and_attach_shader:
 * @self: A #LwProgram
 * @path: The file containing the shader's source code
 * @type: GL_VERTEX_SHADER or GL_FRAGMENT_SHADER
 *
 * This function creates, compiles and attaches a shader to the program.
 * It is easier to use this function instead of creating, compiling and
 * attaching the shader by yourself.
 *
 * Returns: %TRUE on success, %FALSE otherwise
 *
 * Since: 0.4
 */
gboolean
lw_program_create_and_attach_shader(LwProgram *self, const gchar *path, guint type)
{
	LwShader *shader = lw_shader_new_from_uri(path, type);

	if(shader != NULL)
	{
		lw_shader_compile(shader);
		lw_program_attach_shader(self, shader);
		g_object_unref(shader);

		return TRUE;
	}

	return FALSE;
}

/**
 * lw_program_create_and_attach_shader_from_resource:
 * @self: A #LwProgram
 * @path: The file containing the shader's source code
 * @type: GL_VERTEX_SHADER or GL_FRAGMENT_SHADER
 *
 * This function creates, compiles and attaches a shader to the program.
 * It is easier to use this function instead of creating, compiling and
 * attaching the shader by yourself.
 *
 * Returns: %TRUE on success, %FALSE otherwise
 *
 * Since: 0.5
 */
gboolean
lw_program_create_and_attach_shader_from_resource (LwProgram *self, const gchar *path, guint type)
{
	LwShader *shader = lw_shader_new_from_uri (path, type);

	if(shader != NULL)
	{
		lw_shader_compile(shader);
		lw_program_attach_shader(self, shader);
		g_object_unref(shader);

		return TRUE;
	}

	return FALSE;
}

/**
 * lw_program_get_attrib_location:
 * @self: A #LwProgram
 * @name: Name of an attribute variable
 *
 * Calls <ulink url="http://www.opengl.org/sdk/docs/man/xhtml/glGetAttribLocation.xml">glGetAttribLocation</ulink>
 * to get the location of an attribute variable.
 *
 * Returns: The location of an attribute variable or -1 if attribute is not found
 *
 * Since: 0.4
 */
gint
lw_program_get_attrib_location(LwProgram *self, const gchar *name)
{
	gint location = LW_OPENGL_1_4_HELPER(glGetAttribLocation, glGetAttribLocationARB, (self->priv->name, name));
	if(location == -1) g_warning("lw_program_get_attrib_location(): "
	                             "Could not find attribute '%s'", name);
	return location;
}

/**
 * lw_program_get_uniform_location:
 * @self: A #LwProgram
 * @name: Name of an uniform variable
 *
 * Calls <ulink url="http://www.opengl.org/sdk/docs/man/xhtml/glGetUniformLocation.xml">glGetUniformLocation</ulink>
 * to get the location of an uniform variable.
 *
 * Returns: The location of an uniform variable or -1 if uniform is not found
 *
 * Since: 0.4
 */
gint
lw_program_get_uniform_location(LwProgram *self, const gchar *name)
{
	gint location = LW_OPENGL_1_4_HELPER(glGetUniformLocation, glGetUniformLocationARB, (self->priv->name, name));
	if(location == -1) g_warning("lw_program_get_uniform_location(): "
	                             "Could not find uniform '%s'", name);
	return location;
}

static GLint
lw_glsl_type_get_size(LwGLSLType type)
{
	switch(type)
	{
		case LW_GLSL_TYPE_FLOAT:
		case LW_GLSL_TYPE_BOOL:
		case LW_GLSL_TYPE_INT:
			return 1;

		case LW_GLSL_TYPE_VEC2:
		case LW_GLSL_TYPE_BVEC2:
		case LW_GLSL_TYPE_IVEC2:
			return 2;

		case LW_GLSL_TYPE_VEC3:
		case LW_GLSL_TYPE_BVEC3:
		case LW_GLSL_TYPE_IVEC3:
			return 3;

		case LW_GLSL_TYPE_VEC4:
		case LW_GLSL_TYPE_BVEC4:
		case LW_GLSL_TYPE_IVEC4:
			return 4;

		default:
			g_critical("lw_glsls_type_get_size(): Unknown GLSLType %d", type);
			return 0;
	}
}

static GLenum
lw_glsl_type_to_gl_type(LwGLSLType type)
{
	switch(type)
	{
		case LW_GLSL_TYPE_FLOAT:
		case LW_GLSL_TYPE_VEC2:
		case LW_GLSL_TYPE_VEC3:
		case LW_GLSL_TYPE_VEC4:
			return GL_FLOAT;

		case LW_GLSL_TYPE_INT:
		case LW_GLSL_TYPE_IVEC2:
		case LW_GLSL_TYPE_IVEC3:
		case LW_GLSL_TYPE_IVEC4:
		case LW_GLSL_TYPE_BOOL:
		case LW_GLSL_TYPE_BVEC2:
		case LW_GLSL_TYPE_BVEC3:
		case LW_GLSL_TYPE_BVEC4:
			return GL_INT;

		default:
			g_critical("lw_glsl_type_to_gl_type(): Unknown GLSLType %d", type);
			return GL_INT;
	}
}

/**
 * lw_program_set_attribute:
 * @self: A #LwProgram
 * @name: The name of an attribute variable
 * @type: The #LwGLSLType which represents the attribute's type
 * @buffer: A #LwBuffer
 *
 * Binds @buffer to the attribute variable @name. Internally this function uses
 * <ulink url="http://www.opengl.org/sdk/docs/man/xhtml/glVertexAttribPointer.xml">glVertexAttribPointer</ulink>
 * to define the vertex attribute array and
 * <ulink url="http://www.opengl.org/sdk/docs/man/xhtml/glEnableVertexAttribArray.xml">glEnableVertexAttribArray</ulink>
 * to enable it.
 *
 * This function assumes that the data stored in the @buffer is tightly packed
 * (stride is 0) and there is no offset for the first component (pointer is 0, see parameters
 * of <ulink url="http://www.opengl.org/sdk/docs/man/xhtml/glVertexAttribPointer.xml">glVertexAttribPointer</ulink>).
 *
 * Since: 0.5
 */
void
lw_program_set_attribute(LwProgram *self, const gchar *name, LwGLSLType type, LwBuffer *buffer)
{
	gint location = lw_program_get_attrib_location(self, name);

	g_return_if_fail(lw_buffer_get_target(buffer) == GL_ARRAY_BUFFER);

	lw_buffer_bind(buffer);
	LW_OPENGL_1_4_HELPER(glVertexAttribPointer,
	                     glVertexAttribPointerARB,
	                     (location,
	                        lw_glsl_type_get_size(type),
	                        lw_glsl_type_to_gl_type(type),
	                        GL_FALSE, 0, 0));
	LW_OPENGL_1_4_HELPER(glEnableVertexAttribArray,
	                     glEnableVertexAttribArrayARB,
	                     (location));
}

/**
 * lw_program_set_texture:
 * @self: A #LwProgram
 * @name: The name of an uniform variable of type sampler2D
 * @texture: A #LwTexture
 *
 * Binds the uniform variable @name to @texture. This function automatically binds
 * the texture to the next free texture unit and so it is able to handle multiple
 * textures for one #LwProgram.
 *
 * This functions calls lw_texture_bind_to(), so you just have to call lw_texture_unbind()
 * or lw_texture_disable() if necessary. If you do not unbind the texture after every paint,
 * make sure to unbind it in lw_wallpaper_restore_viewport().
 *
 * Since: 0.5
 */
void
lw_program_set_texture(LwProgram *self, const gchar *name, LwTexture *texture)
{
	guint i;

	for(i = 0; i < self->priv->tex_units->len; i++)
		if(strcmp(name, g_array_index(self->priv->tex_units, gchar*, i)) == 0)
			break;

	if(i == self->priv->tex_units->len)
	{
		/* Get next unused texture unit and set uniform */
		gint location = lw_program_get_uniform_location(self, name);
		gchar *name_copy = g_strdup(name);

		g_array_append_val(self->priv->tex_units, name_copy);
		LW_OPENGL_1_4_HELPER(glUniform1i, glUniform1iARB, (location, i));
	}

	lw_texture_bind_to(texture, i);
}

/**
 * lw_program_set_matrix:
 * @self: A #LwProgram
 * @name: The name of an uniform variable of type mat4
 * @matrix: A #LwMatrix
 *
 * Specifies the value of the matrix uniform variable @name.
 *
 * Since: 0.5
 */
void
lw_program_set_matrix(LwProgram *self, const gchar *name, LwMatrix *matrix)
{
	gint location = lw_program_get_uniform_location(self, name);

	LW_OPENGL_1_4_HELPER(glUniformMatrix4fv, glUniformMatrix4fvARB, (location, 1, GL_TRUE, lw_matrix_get_elements(matrix)));
}

/**
 * lw_program_enable:
 * @self: A #LwProgram
 *
 * Enables the program.
 *
 * Since: 0.4
 */
void
lw_program_enable(LwProgram *self)
{
	LW_OPENGL_1_4_HELPER(glUseProgram, glUseProgramObjectARB, (self->priv->name));
}

/**
 * lw_program_disable:
 * @self: A #LwProgram
 *
 * Disables this program.
 *
 * Since: 0.4
 */
void
lw_program_disable(LwProgram *self G_GNUC_UNUSED)
{
	LW_OPENGL_1_4_HELPER(glUseProgram, glUseProgramObjectARB, (0));
}

static void
lw_program_init(LwProgram *self)
{
	self->priv = G_TYPE_INSTANCE_GET_PRIVATE(self, LW_TYPE_PROGRAM,
	                                         LwProgramPrivate);

	self->priv->name = LW_OPENGL_1_4_HELPER(glCreateProgram, glCreateProgramObjectARB, ());
	self->priv->tex_units = g_array_new(FALSE, FALSE, sizeof(gchar*));
	g_array_set_clear_func(self->priv->tex_units, array_clear_func);
}

static void
lw_program_finalize(GObject *object)
{
	LwProgram *self = LW_PROGRAM(object);

	if(self->priv->name)
		LW_OPENGL_1_4_HELPER(glDeleteProgram, glDeleteObjectARB, (self->priv->name));

	g_array_free(self->priv->tex_units, TRUE);

	/* Chain up to the parent class */
	G_OBJECT_CLASS(lw_program_parent_class)->finalize(object);
}

static void
lw_program_class_init(LwProgramClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

	gobject_class->finalize = lw_program_finalize;

	g_type_class_add_private(klass, sizeof(LwProgramPrivate));
}

