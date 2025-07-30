#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include <vector>

#include <GL/gl3w.h>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;

class vec3
{
public:
	float x, y, z;

	vec3() {}
	vec3(float l, float m, float r)
	{
		x = l;
		y = m;
		z = r;
	}
	~vec3() {}
};

class vec2
{
public:
	float x, y;

	vec2() {}
	vec2(float l, float r)
	{
		x = l;
		y = r;
	}
	~vec2() {}
};

struct vertex
{
public:
	vec3 vc;
	vec3 tc;
	vec3 uc;

	vertex() {}
	vertex(vec3 vc_in, vec3 tc_in, vec3 uc_in)
	{
		vc = vc_in;
		tc = tc_in;
		uc = uc_in;
	}
	~vertex() {}
};
struct triangle
{
public:
	vertex verts[3];

	triangle() {}
	triangle(vertex v0, vertex v1, vertex v2)
	{
		verts[0] = v0;
		verts[1] = v1;
		verts[2] = v2;
	}
	~triangle() {}
};

class Material
{
public:
	char mtl_name[256];
	char fil_name[256];

	Material() {}
	Material(char* n, char* f)
	{
		strcpy(mtl_name, n);
		strcpy(fil_name, f);
	}
	~Material()
	{
	}
};

class Object
{
public:
	unsigned int VAO;
	unsigned int VBO;
	vector<triangle> tris;
	Material mtl;
	GLuint texture;


	Object() {}
	Object(Material m)
	{
		strcpy(mtl.fil_name, m.fil_name);
		strcpy(mtl.mtl_name, m.mtl_name);
	}
	~Object()
	{
	}
};

int mtl_parse(char* filename, vector<Material>* mtls)
{
	FILE* file = fopen(filename, "r"); // Open file
	std::vector<Material> mats;

	// File handling
	if (file == NULL) {
		fprintf(stderr, "Error: Unable to open file %s\n", filename);
		return 0; // Failure
	}


	char mtl_name[256];// File reading

	char buffer[256]; // Directory of newmtl
	char buffer2[256]; // Directory of map_Kd

	char line[256]; // Name of newmtl
	char line2[256]; // Name of map_Kd

	bool foundM = false; // Found material 1
	bool foundF = false; // Found material 2

	//Indentation handling
	while (fgets(mtl_name, 256, file)) {
		int index = 0;
		while (mtl_name[index] == ' ' || mtl_name[index] == '\t') {
			index++;
		}


		if (strncmp(mtl_name + index, "newmtl", 6) == 0) {
			strcpy(line, mtl_name + 7);
			strtok(line, "\n"); // Remove newline character if present

			int slash = 0; // Slash counter
			int length = 0; // Size of array

			for (int i = 0; filename[i] != '\0'; i++) {
				buffer[i] = filename[i]; // Copy filename character to buffer
				if (filename[i] == '/') {
					slash++; // Increment slash if found
				}
				length++;
				if (slash == 2) {
					break; // Stop after 2nd slash
				}
			}

			// Concatenate newmtl to directory
			buffer[length] = 0;
			strcat(buffer, line);

			foundM = true; // Material 1 found
		}

		if (strncmp(mtl_name + index, "map_Kd", 6) == 0) {
			strcpy(line2, mtl_name + index + 7); // Adjust index to skip leading spaces
			strtok(line2, "\n"); // Remove newline character if present

			int slash = 0;
			int length = 0;

			for (int i = 0; filename[i] != '\0'; i++) {
				buffer2[i] = filename[i];
				if (filename[i] == '/') {
					slash++;
				}
				length++;
				if (slash == 2) {
					break;
				}
			}

			// Concatenate map_Kd to directory
			buffer2[length] = 0;
			strcat(buffer2, line2);

			foundF = true; // Material 2 found
		}

		// Handle newline
		for (int i = 0; buffer[i] != '\0'; i++) {
			if (buffer[i] == '\n') {
				buffer[i] = '\0';
				break;
			}
		}
		for (int i = 0; buffer2[i] != '\0'; i++) {
			if (buffer2[i] == '\n') {
				buffer2[i] = '\0';
				break;
			}
		}

		// Push materials if both found
		if (foundM && foundF) {
			mats.push_back(Material(buffer, buffer2));

			// Reset
			foundM = false;
			foundF = false;

			memset(buffer, 0, sizeof(buffer));
			memset(buffer2, 0, sizeof(buffer2));
		}

		// Reset
		memset(line, 0, sizeof(line));
		memset(line2, 0, sizeof(line2));
	}

	fclose(file); // Close file
	*mtls = mats; // Assign mats

	//success
	return 1;
}

int obj_parse(const char* filename, vector<Object>* objs)
{
	vector<Material> materials;
	vector<vec3> vecs;
	vector<vec3> uvs;
	vector<vec3> vecns;


	FILE* file = fopen(filename, "r"); // Open file

	// File handling
	if (file == NULL) {
		fprintf(stderr, "Error: Unable to open file %s\n", filename);
		return 0; // Failure
	}

	char mtl_name[256];
	char line[256]; // Name of mtllib

	while (fgets(mtl_name, 256, file)) {
		if (strncmp(mtl_name, "mtllib", 6) == 0) {
			strcpy(line, mtl_name + 7);
			strtok(line, "\n"); // Remove newline character if present
			break;
		}
	}

	// Find correct directory as in mtl_parse
	char full_filename[256];
	int slash = 0;
	int length = 0;
	for (int i = 0; filename[i] != '\0'; i++) {
		full_filename[i] = filename[i];
		if (filename[i] == '/') {
			slash++;
		}
		length++;
		if (slash == 2) {
			break;
		}
	}
	full_filename[length] = 0;
	strcat(full_filename, line);

	// Handle newline
	for (int i = 0; full_filename[i] != '\0'; i++) {
		if (full_filename[i] == '\n') {
			full_filename[i] = '\0';
			break;
		}
	}

	mtl_parse(full_filename, &materials); // Parse materials
	fgets(mtl_name, 256, file);

	Object obj; // Create object

	while (fgets(mtl_name, 256, file)) {

		char token[7]; // Create token
		sscanf(mtl_name, "%6s", token);

		if (strcmp(token, "v") == 0) {
			struct vec3 vertex;
			sscanf(mtl_name + 2, "%f %f %f", &vertex.x, &vertex.y, &vertex.z); // Get 'v' values
			vecs.push_back(vertex); // Store 'v' values
		}

		else if (strcmp(token, "vt") == 0) {
			struct vec3 uv;
			sscanf(mtl_name + 3, "%f %f", &uv.x, &uv.y); // Get 'uv' values
			uv.z = 0;
			uv.y = 1 - uv.y; // Flip upside down
			uvs.push_back(uv); // Store 'uv' values
		}

		else if (strcmp(token, "vn") == 0) {
			struct vec3 normal;
			sscanf(mtl_name + 3, "%f %f %f", &normal.x, &normal.y, &normal.z);
			vecns.push_back(normal); // Store vertex normals
		}

		else if (strcmp(token, "usemtl") == 0) {
			char mat_name[256];
			sscanf(mtl_name + 7, "%s", mat_name); // Get name of material

			int slash = 0;
			int length = 0;

			// Find the length and name of the directory path
			for (int i = 0; materials[0].mtl_name[i] != '\0'; i++) {
				if (materials[0].mtl_name[i] == '/') {
					slash++;
				}
				length++;
				if (slash == 2) {
					break;
				}
			}

			// Search for the material with the specified name
			for (int i = 0; i < materials.size(); i++) {
				if (strcmp(mat_name, materials[i].mtl_name + length) == 0) {
					objs->push_back(obj.mtl = materials[i]); // Push the material if they are equal
				}
			}
		}

		else if (strcmp(token, "f") == 0) {
			int v1, v2, v3, t1, t2, t3, u1, u2, u3;
			sscanf(mtl_name + 2, "%d/%d/%d %d/%d/%d %d/%d/%d", &v1, &t1, &u1, &v2, &t2, &u2, &v3, &t3, &u3); // Get 'f' values


			// Create triangles using the vertex indices
			vertex vertex1(vecs[v1 - 1], uvs[t1 - 1], vecns[u1 - 1]);
			vertex vertex2(vecs[v2 - 1], uvs[t2 - 1], vecns[u2 - 1]);
			vertex vertex3(vecs[v3 - 1], uvs[t3 - 1], vecns[u3 - 1]);

			triangle tri(vertex1, vertex2, vertex3);

			objs->back().tris.push_back(tri); // Add the triangle to the Object
		}
	}

	fclose(file); // Close file

	// Success
	return 1;
}
