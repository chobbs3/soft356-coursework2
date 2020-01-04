#pragma once

void init(void);

void loadTexture(GLuint& texture, std::string texturepath);

bool loadOBJ(
	const char* path,
	std::vector < glm::vec3 >& out_vertices,
	std::vector < glm::vec2 >& out_uvs,
	std::vector < glm::vec3 >& out_normals
);
