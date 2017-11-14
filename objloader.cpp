#include <iostream>
#include <vector>
#include <string>
#include <cstring>

#include <glm\glm.hpp>

#include "objloader.h"

bool LoadOBJ(
	const char * path, 
	std::vector<glm::vec3> & outVertices, 
	std::vector<glm::vec2> & outUvs,
	std::vector<glm::vec3> & outNormals
){
	std::cout << "Load OBJ model : " << path << std::endl;

	std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
	std::vector<glm::vec3> tempVertices; 
	std::vector<glm::vec2> tempUvs;
	std::vector<glm::vec3> tempNormals;

	FILE * file = fopen(path, "r");
	if( file == NULL ){
		std::cout << "OBJ file open fail." << std::endl;
		return false;
	}

	while(true){

		char lineHeader[128];
		
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
			break;

		// parse lineHeader

		// vertex position
		if ( strcmp( lineHeader, "v" ) == 0 ){
			glm::vec3 vertex;
			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z );
			tempVertices.push_back(vertex);
		
		// vertex texture position
		}else if ( strcmp( lineHeader, "vt" ) == 0 ){
			glm::vec2 uv;
			fscanf(file, "%f %f\n", &uv.x, &uv.y );
			uv.y = -uv.y; // Invert V coordinate since we will only use DDS texture, which are inverted. Remove if you want to use TGA or BMP loaders.
			tempUvs.push_back(uv);

		// vertex normal
		}else if ( strcmp( lineHeader, "vn" ) == 0 ){
			glm::vec3 normal;
			fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z );
			tempNormals.push_back(normal);

		// face
		}else if ( strcmp( lineHeader, "f" ) == 0 ){
			std::string vertex1, vertex2, vertex3;
			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
			int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2] );
			if (matches != 9){
				std::cout << "Obj file format invaild." << std::endl;
				return false;
			}
			vertexIndices.push_back(vertexIndex[0]);
			vertexIndices.push_back(vertexIndex[1]);
			vertexIndices.push_back(vertexIndex[2]);
			uvIndices    .push_back(uvIndex[0]);
			uvIndices    .push_back(uvIndex[1]);
			uvIndices    .push_back(uvIndex[2]);
			normalIndices.push_back(normalIndex[0]);
			normalIndices.push_back(normalIndex[1]);
			normalIndices.push_back(normalIndex[2]);

		// not a data.
		}else{
			char stupidBuffer[1000];
			fgets(stupidBuffer, 1000, file);
		}

	}

	// position data -> vector
	for( unsigned int i=0; i<vertexIndices.size(); i++ ){

		unsigned int vertexIndex = vertexIndices[i];
		unsigned int uvIndex = uvIndices[i];
		unsigned int normalIndex = normalIndices[i];
		
		glm::vec3 vertex = tempVertices[ vertexIndex-1 ];
		glm::vec2 uv = tempUvs[ uvIndex-1 ];
		glm::vec3 normal = tempNormals[ normalIndex-1 ];
		
		outVertices.push_back(vertex);
		outUvs     .push_back(uv);
		outNormals .push_back(normal);
	}

	return true;
}
