#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <cmath>
#include <map>

#ifndef __MTL_H__
#define __MTL_H__

struct Material {

	std::string texture;
	std::string id;
	float Ns, Ni, d;
	float illum;
	std::vector<float> Ka;
	std::vector<float> Kd;
	std::vector<float> Ks;


	Material() {
		texture = "";
		id = "";
		Ns = 0.f; Ni = 0.f; d = 0.f; illum = 0.f;
	}
};

// For reading and linking in mtl files
std::map<std::string, Material> read_mtl(const std::string& filename) {
	std::ifstream ifs(filename.c_str(), std::ifstream::in);
	std::map<std::string,Material> result;

	if(!ifs.good()){
		std::cout << "Problem loading mtl file " << filename << std::endl;
		return result;
	}

	Material *mat = new Material();
	std::string next_line;
	char c;
	float x;
	int start = 0;

	while (getline(ifs, next_line)) {
		char *str = new char[next_line.length() + 1];
		strcpy(str, next_line.c_str());
		char* arguments = strtok (str, " \r");

		if(arguments == '\0'){
			continue;
		} else if(arguments[0] == '\t'){
			arguments ++;
		}

		if (strcmp(arguments, "newmtl") == 0){
			if (start != 0) {
				result[mat->id] = *mat;
				mat = new Material();
			}

			arguments = strtok(NULL, " \r");
			mat->id = arguments;
			start++;

		} else if (strcmp(arguments, "Ns") == 0) {
			arguments = strtok (NULL, " \r");
			mat->Ns = strtof(arguments, NULL);

		} else if (strcmp(arguments, "Ni") == 0) {
			arguments = strtok (NULL, " \r");
			mat->Ni = strtof(arguments, NULL);

		} else if (strcmp(arguments, "d") == 0) {
			arguments = strtok (NULL, " \r");
			mat->d = strtof(arguments, NULL);

		} else if (strcmp(arguments, "illum") == 0) {
			arguments = strtok (NULL, " \r");
			mat->illum = strtof(arguments, NULL);

		} else if (strcmp(arguments, "Ka") == 0) {
			for (int i = 0; i < 3; i++) {
				arguments = strtok(NULL, " \r");
				mat->Ka.push_back(strtof(arguments, NULL));
			}

		} else if (strcmp(arguments, "Kd") == 0) {
			for (int i = 0; i < 3; i++) {
				arguments = strtok(NULL, " \r");
				mat->Kd.push_back(strtof(arguments, NULL));
			}
		
		} else if (strcmp(arguments, "Ks") == 0) {
			for (int i = 0; i < 3; i++) {
				arguments = strtok(NULL, " \r");
				mat->Ks.push_back(strtof(arguments, NULL));
			}

		} else if (strcmp(arguments, "map_Kd") == 0) {
			arguments = strtok(NULL, " \r");
			char file_name[80] = "meshes/";
			strcat(file_name, arguments);
			while((char*)strchr(file_name, '\\') != NULL){
				int pos = strchr(file_name, '\\') - file_name;
				char new_file_name[80] = "";
				memcpy(new_file_name, file_name, pos);
				strcat(new_file_name, "/");
				strcat(new_file_name, &file_name[pos + 1]);
		    	stpcpy(file_name, new_file_name);
			}
			mat->texture = file_name;
		}

	}

	result[mat->id] = *mat;
	return result;
}

#endif