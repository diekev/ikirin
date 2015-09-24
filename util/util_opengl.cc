/*
 * ***** BEGIN GPL LICENSE BLOCK *****
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * The Original Code is Copyright (C) 2015 Kévin Dietrich.
 * All rights reserved.
 *
 * ***** END GPL LICENSE BLOCK *****
 */

#include <GL/glew.h>
#include <iostream>

#include "util_opengl.h"
#include "utils.h"

void gl_check_errors()
{
	GLenum error = glGetError();

	if (error == GL_NO_ERROR) {
		return;
	}

	switch (error) {
		case GL_INVALID_ENUM:
			std::cerr << "GL Invalid Enum Error\n";
			break;
		case GL_INVALID_VALUE:
			std::cerr << "GL Invalid Value Error\n";
			break;
		case GL_INVALID_OPERATION:
			std::cerr << "GL Invalid Operation Error\n";
			break;
		case GL_OUT_OF_MEMORY:
			std::cerr << "GL Invalid Out of Memory Error\n";
			break;
	}
}

void texture_bind(const GLenum target, const GLuint texture_id, const GLint num)
{
	glActiveTexture(GL_TEXTURE0 + num);
	glBindTexture(target, texture_id);
}

void texture_unbind(const GLenum target, const GLint num)
{
	glActiveTexture(GL_TEXTURE0 + num);
	glBindTexture(target, 0);
}

void create_texture_1D(GLuint &texture_id, const int size, GLfloat *data)
{
	glGenTextures(1, &texture_id);
	glBindTexture(GL_TEXTURE_1D, texture_id);

	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB, size, 0, GL_RGB, GL_FLOAT, data);
}

void create_texture_3D(GLuint &texture_id, const int size[3], const int channels, GLfloat *data)
{
	Timer(__func__);

	GLenum type = GL_FLOAT;
	GLenum format, internalformat;

	if (channels == 3) {
		format = GL_RGB;
		internalformat = GL_RGB;
	}
	else {
		format = GL_RED;
		internalformat = GL_RED;
	}

	glGenTextures(1, &texture_id);
	glBindTexture(GL_TEXTURE_3D, texture_id);

	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAX_LEVEL, 4);

	glTexImage3D(GL_TEXTURE_3D, 0, internalformat, size[0], size[1], size[2], 0, format, type, data);

	glGenerateMipmap(GL_TEXTURE_3D);
}

VBOData *create_vertex_buffers(GLuint attribute, const GLfloat *vertices, size_t vsize,
                           const GLuint *indices, size_t isize)
{
	VBOData *data = new VBOData;

	GLenum draw_type = GL_STATIC_DRAW;

	// TODO: feels a bit hackish
	if ((vertices == nullptr) && (indices == nullptr)) {
		draw_type = GL_DYNAMIC_DRAW;
	}

	glGenVertexArrays(1, &data->vao);
	glGenBuffers(1, &data->vbo);
	glGenBuffers(1, &data->index_vbo);

	glBindVertexArray(data->vao);

	glBindBuffer(GL_ARRAY_BUFFER, data->vbo);
	glBufferData(GL_ARRAY_BUFFER, vsize, vertices, draw_type);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, data->index_vbo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, isize, indices, draw_type);

	glEnableVertexAttribArray(attribute);
	glVertexAttribPointer(attribute, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	glBindVertexArray(0);
	gl_check_errors();

	return data;
}

void delete_vertex_buffers(VBOData *data)
{
	glDeleteVertexArrays(1, &data->vao);
	glDeleteBuffers(1, &data->index_vbo);
	glDeleteBuffers(1, &data->vbo);

	delete data;
}
