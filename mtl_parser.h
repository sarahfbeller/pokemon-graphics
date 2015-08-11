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

// std::map<std::string, Material> read_mtl(const std::string& filename) {
	// std::ifstream ifs(filename.c_str(), std::ifstream::in);
	
	// char c = ifs.get();
	// std::string word;

	// float x;

	// Material *mat = new Material();
	// std::map<std::string,Material> result;
	// int start = 0;

	// while (ifs.good()) {

	// 	if (c != ' ' && c != '\n' && c != '\t') {
	// 		word += c;
	// 	}

	// 	else if (word == "newmtl") {
	// 		if (start != 0) {
	// 			result[mat->id] = *mat;
	// 			mat = new Material();
	// 		}

	// 		word = "";
	// 		c = ifs.get();
	// 		while (c != ' ' && c!= '\n') {
	// 			word += c;
	// 			c = ifs.get();
	// 		}

	// 		mat->id = word;
	// 		printf("%s\n", word.c_str());
	// 		word = "";
	// 		start++;

	// 	}

	// 	else if (word == "Ns") {
	// 		ifs >> x;
	// 		mat->Ns = x;
	// 		word = "";
	// 		c = ifs.get();
	// 	}

	// 	else if (word == "Ni") {
	// 		ifs >> x;
	// 		mat->Ni = x;
	// 		word = "";
	// 	}

	// 	else if (word == "d") {
	// 		ifs >> x;
	// 		mat->d = x;
	// 		word = "";
	// 	}

	// 	else if (word == "illum") {
	// 		ifs >> x;
	// 		mat->illum = x;
	// 		word = "";
	// 	}

	// 	else if (word == "Ka") {
	// 		for (int i = 0; i < 3; i++) {
	// 			ifs >> x;
	// 			mat->Ka.push_back(x);
	// 		}
	// 		word = "";
	// 	}

	// 	else if (word == "Kd") {
	// 		for (int i = 0; i < 3; i++) {
	// 			ifs >> x;
	// 			mat->Kd.push_back(x);
	// 		}
	// 		word = "";
	// 	}

	// 	else if (word == "Ks") {
	// 		for (int i = 0; i < 3; i++) {
	// 			ifs >> x;
	// 			mat->Kd.push_back(x);
	// 		}
	// 		word = "";
	// 	}

	// 	else if (word == "map_Kd") {
	// 		word = "";
	// 		c = ifs.get();
	// 		while (c != ' ' && c!= '\n') {
	// 			word += c;
	// 			c = ifs.get();
	// 		}
	// 		printf("%s\n", word.c_str());
	// 		mat->texture = word;
	// 		word = "";
	// 	}

	// 	else if (word == "Tf") {
	// 		ifs.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	// 	}
	// 	word = "";
	// 	c = ifs.get();
	// }
	// result[mat->id] = *mat;
	// return result;

// }


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
		char* arguments = strtok (str, " ");

		if(arguments == '\0'){
			continue;
		}

		if (strcmp(arguments, "newmtl") == 0){
			if (start != 0) {
				result[mat->id] = *mat;
				mat = new Material();
			}

			arguments = strtok(NULL, " ");
			mat->id = strtof(arguments, NULL);
			start++;

		} else if (strcmp(arguments, "Ns") == 0) {
			arguments = strtok (NULL, " ");
			mat->Ns = strtof(arguments, NULL);

		} else if (strcmp(arguments, "Ni") == 0) {
			arguments = strtok (NULL, " ");
			mat->Ni = strtof(arguments, NULL);

		} else if (strcmp(arguments, "d") == 0) {
			arguments = strtok (NULL, " ");
			mat->d = strtof(arguments, NULL);

		} else if (strcmp(arguments, "illum") == 0) {
			arguments = strtok (NULL, " ");
			mat->illum = strtof(arguments, NULL);

		} else if (strcmp(arguments, "Ka") == 0) {
			for (int i = 0; i < 3; i++) {
				arguments = strtok(NULL, " ");
				mat->Ka.push_back(strtof(arguments, NULL));
			}

		} else if (strcmp(arguments, "Kd") == 0) {
			for (int i = 0; i < 3; i++) {
				arguments = strtok(NULL, " ");
				mat->Kd.push_back(strtof(arguments, NULL));
			}
		
		} else if (strcmp(arguments, "Ks") == 0) {
			for (int i = 0; i < 3; i++) {
				arguments = strtok(NULL, " ");
				mat->Ks.push_back(strtof(arguments, NULL));
			}

		} else if (strcmp(arguments, "map_Kd") == 0) {
			arguments = strtok(NULL, " ");
			char file_name[80];
			strcpy(file_name, "meshes/Textures/");
			strcat(file_name, arguments);
			std::cout << file_name << std::endl;
			mat->texture = arguments;

		// } else if (strcmp(arguments, "Tf") == 0) {
		// 	for(int i = 0; i < 3; i++)
		// 		arguments = strtok(NULL, " ");
		}

	}

	result[mat->id] = *mat;
	return result;
}

#endif