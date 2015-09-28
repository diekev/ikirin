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
#include <glm/glm.hpp>

#define DWREAL_IS_DOUBLE 0
#include <openvdb/openvdb.h>

#include <vector>

const int MAX_SLICES = 512;

class Cube;
class GPUBuffer;
class GPUTexture;
class TreeTopology;

class Volume {
	GPUBuffer *m_buffer_data;
	GPUTexture *m_volume_texture, *m_transfer_texture, *m_index_texture;
	GPUShader m_shader;

	Cube *m_bbox;
	TreeTopology *m_topology;

	glm::vec3 m_min, m_max;
	glm::vec3 m_size, m_inv_size;

	int m_num_slices;
	std::vector<glm::vec3> m_texture_slices;

	int m_axis;
	float m_scale; // scale of the values contained in the grid (1 / (max - min))
	bool m_use_lut, m_draw_bbox, m_draw_topology;

	void loadTransferFunction();
	void loadVolumeShader();

public:
	Volume();
	Volume(openvdb::FloatGrid::Ptr &grid);
	~Volume();

	void slice(const glm::vec3 &view_dir);
	void render(const glm::vec3 &dir, const glm::mat4 &MVP);
	void changeNumSlicesBy(int x);

	void toggleUseLUT();
	void toggleBBoxDrawing();
	void toggleTopologyDrawing();
};
