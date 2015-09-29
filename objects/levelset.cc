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

#include <glm/gtc/type_ptr.hpp>
#include <openvdb/tools/VolumeToMesh.h>

#include "util/util_opengl.h"
#include "util/utils.h"

#include "levelset.h"

LevelSet::LevelSet()
    : m_buffer_data(std::unique_ptr<GPUBuffer>(new GPUBuffer()))
    , m_bbox(nullptr)
    , m_topology(nullptr)
    , m_min(glm::vec3(0.0f))
    , m_max(glm::vec3(0.0f))
    , m_size(glm::vec3(0.0f))
    , m_inv_size(glm::vec3(0.0f))
    , m_draw_bbox(false)
    , m_draw_topology(false)
{}

LevelSet::LevelSet(openvdb::FloatGrid::Ptr &grid)
    : LevelSet()
{
	using namespace openvdb;
	using namespace openvdb::math;

	CoordBBox bbox = grid->evalActiveVoxelBoundingBox();

	BBoxd ws_bbox = grid->transform().indexToWorld(bbox);
	Vec3f min = ws_bbox.min();
	Vec3f max = ws_bbox.max();

	m_min = convertOpenVDBVec(min);
	m_max = convertOpenVDBVec(max);

	m_size = (m_max - m_min);
	m_inv_size = 1.0f / m_size;

	m_bbox = std::unique_ptr<Cube>(new Cube(m_min, m_max));
	m_topology = std::unique_ptr<TreeTopology>(new TreeTopology(grid));

	loadShader();
	generate_mesh(grid);
}

void LevelSet::loadShader()
{
	m_program.loadFromFile(GL_VERTEX_SHADER, "shader/object.vert");
	m_program.loadFromFile(GL_FRAGMENT_SHADER, "shader/object.frag");
	m_program.createAndLinkProgram();

	m_program.enable();
	{
		m_program.addAttribute("vertex");
		m_program.addAttribute("normal");
		m_program.addUniform("MVP");
		m_program.addUniform("N");
	}
	m_program.disable();
}

void LevelSet::render(const glm::mat4 &MVP, const glm::mat3 &N)
{
	glEnable(GL_DEPTH_TEST);

	if (m_program.isValid()) {
		m_program.enable();
		m_buffer_data->bind();

		glUniformMatrix4fv(m_program("MVP"), 1, GL_FALSE, glm::value_ptr(MVP));
		glUniformMatrix3fv(m_program("N"), 1, GL_FALSE, glm::value_ptr(N));
		glDrawElements(GL_QUADS, m_elements, GL_UNSIGNED_INT, nullptr);

		m_buffer_data->unbind();
		m_program.disable();
	}

	glDisable(GL_DEPTH_TEST);
}

void LevelSet::generate_mesh(openvdb::FloatGrid::ConstPtr grid)
{
	Timer(__func__);

	using namespace openvdb;
	using openvdb::Index64;

	openvdb::tools::VolumeToMesh mesher(0.0);
	mesher(*grid);

	/* Copy points and generate point normals. */
	std::vector<GLfloat> points(mesher.pointListSize() * 3);
	std::vector<GLfloat> normals(mesher.pointListSize() * 3);

	for (Index64 n = 0, i = 0, N = mesher.pointListSize(); n < N; ++n) {
		const openvdb::Vec3s& p = mesher.pointList()[n];
		points[i++] = p[0];
		points[i++] = p[1];
		points[i++] = p[2];
	}

	/* Copy primitives */
	openvdb::tools::PolygonPoolList& polygonPoolList = mesher.polygonPoolList();
	Index64 numQuads = 0;
	for (Index64 n = 0, N = mesher.polygonPoolListSize(); n < N; ++n) {
		numQuads += polygonPoolList[n].numQuads();
	}

	std::vector<GLuint> indices;
	indices.reserve(numQuads * 4);
	openvdb::Vec3d normal, e1, e2;

	for (Index64 n = 0, N = mesher.polygonPoolListSize(); n < N; ++n) {
		const openvdb::tools::PolygonPool& polygons = polygonPoolList[n];

		for (Index64 i = 0, I = polygons.numQuads(); i < I; ++i) {
			const openvdb::Vec4I& quad = polygons.quad(i);
			indices.push_back(quad[0]);
			indices.push_back(quad[1]);
			indices.push_back(quad[2]);
			indices.push_back(quad[3]);

			e1 = mesher.pointList()[quad[1]];
			e1 -= mesher.pointList()[quad[0]];
			e2 = mesher.pointList()[quad[2]];
			e2 -= mesher.pointList()[quad[1]];
			normal = e1.cross(e2);

			const double length = normal.length();
			if (length > 1.0e-7) normal *= (1.0 / length);

			for (int v = 0; v < 4; ++v) {
				normals[quad[v] * 3]     = static_cast<GLfloat>(-normal[0]);
				normals[quad[v] * 3 + 1] = static_cast<GLfloat>(-normal[1]);
				normals[quad[v] * 3 + 2] = static_cast<GLfloat>(-normal[2]);
			}
		}
	}

	m_buffer_data->bind();
	m_buffer_data->generateVertexBuffer(&points[0], points.size() * sizeof(GLfloat));
	m_buffer_data->generateIndexBuffer(&indices[0], indices.size() * sizeof(GLuint));
	m_buffer_data->attribPointer(m_program["vertex"], 3);
	m_buffer_data->generateNormalBuffer(&normals[0], normals.size() * sizeof(GLfloat));
	m_buffer_data->attribPointer(m_program["normal"], 3);
	m_buffer_data->unbind();

	gl_check_errors();

	m_elements = indices.size();
}

void LevelSet::toggleBBoxDrawing()
{
	m_draw_bbox = !m_draw_bbox;
}

void LevelSet::toggleTopologyDrawing()
{
	m_draw_topology = !m_draw_topology;
}
