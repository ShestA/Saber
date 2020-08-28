//
// Calculate smooth (average) per-vertex normals
//
// [out] normals - output per-vertex normals
// [in] verts - input per-vertex positions
// [in] faces - triangles (triplets of vertex indices)
// [in] nverts - total number of vertices (# of elements in verts and normals arrays)
// [in] nfaces - total number of faces (# of elements in faces array)
//
/*
*	Проверяем количество элементов в массиве faces - кратно трем, иначе исключение
*	Определяем нормали всех треугольников
*	Для каждой вершины находим нормаль, как нормализованная сумма нормалей треугольников, принадлежащих вершине
*/
#include <stdexcept>
#include <vector>
//Нормали к вершинам
struct vec3 {
	float x;
	float y;
	float z;

	vec3() :x(0), y(0), z(0) {};
	vec3(float ix, float iy, float iz) :x(ix), y(iy), z(iz) {};

	vec3& operator=(const vec3& other) {
		this->x = other.x;
		this->y = other.y;
		this->z = other.z;

		return *this;
	};

	vec3& operator+(const vec3& other) const {
		vec3 v;

		v.x = this->x + other.x;
		v.y = this->y + other.y;
		v.z = this->z + other.z;

		return v;
	};

	vec3& operator+=(const vec3& other) {
		this->x += other.x;
		this->y += other.y;
		this->z += other.z;

		return *this;
	};

	vec3& operator-(const vec3& other) const {
		vec3 v;

		v.x = this->x - other.x;
		v.y = this->y - other.y;
		v.z = this->z - other.z;

		return v;
	};
};

struct triplet {
	int v1;
	int v2;
	int v3;
	bool contains(const int v) const {
		return ((v1 == v) || (v2 == v) || (v3 == v));
	}
};

void Vec3Cross(vec3* pOut, const vec3* pV1, const vec3* pV2);
void Vec3Normalize(vec3* pOut, const vec3* pV);

void calc_mesh_normals(vec3* normals, const vec3* verts, const int* faces, int nverts, int nfaces)
{
	if (nfaces % 3 != 0) throw std::runtime_error("Wrong face list");

	const triplet* triangles;
	int ntriangles;
	vec3* trianglesNorms;

	ntriangles = nfaces / 3;
	triangles = (const triplet*)(faces);
	trianglesNorms = new vec3[ntriangles];

	for (int i = 0; i < ntriangles; i++) {//Расчет нормалей для треугольников
		vec3 M1, M2, M3, P, Q;
		vec3 normal;

		M1 = verts[triangles[i].v1];
		M2 = verts[triangles[i].v2];
		M3 = verts[triangles[i].v3];
		P = M2 - M1;
		Q = M3 - M1;
		Vec3Cross(&normal, &P, &Q);
		Vec3Normalize(&normal, &normal);
		trianglesNorms[i] = normal;
	}
	for (int i = 0; i < nverts; i++) {//Обход вершин, для подсчета нормалей
		vec3 normal;

		for (int j = 0; j < ntriangles; j++) {//Обход треугольников для суммирования нормалей треугольников, принадлежащих текущей вершине
			if (triangles[j].contains(i)) {//Треугольник принадлежит вершине
				normal += trianglesNorms[j];
			}
		}
		Vec3Normalize(&normal, &normal);//Нормализация суммы векторов
		normals[i] = normal;//Запись нормали в выходную переменную
	}
	delete[] trianglesNorms;
}

void Vec3Cross(vec3* pOut, const vec3* pV1, const vec3* pV2) {
	vec3 v;

	v.x = pV1->y * pV2->z - pV1->z * pV2->y;
	v.y = pV1->z * pV2->x - pV1->x * pV2->z;
	v.z = pV1->x * pV2->y - pV1->y * pV2->x;

	*pOut = v;
}

void Vec3Normalize(vec3* pOut, const vec3* pV) {
	float len;
	vec3 n;

	len = sqrtf(powf(pV->x, 2) + powf(pV->y, 2) + powf(pV->z, 2));
	n.x = pV->x / len;
	n.y = pV->y / len;
	n.z = pV->z / len;

	*pOut = n;
}

#include <iostream>
int test() {
	vec3* normals = new vec3[5];
	vec3* verts = new vec3[5];
	int* faces = new int[4 * 3];
	int nverts = 5;
	int nfaces = 4 * 3;

	verts[0] = vec3(0.0, 0.5, 0.0);
	verts[1] = vec3(-0.5, 0.0, 0.5);
	verts[2] = vec3(0.5, 0.0, 0.5);
	verts[3] = vec3(0.5, 0.0, -0.5);
	verts[4] = vec3(-0.5, 0.0, -0.5);

	faces[0] = 0;
	faces[1] = 1;
	faces[2] = 2;

	faces[3] = 0;
	faces[4] = 2;
	faces[5] = 3;

	faces[6] = 0;
	faces[7] = 3;
	faces[8] = 4;

	faces[9] = 0;
	faces[10] = 4;
	faces[11] = 1;
	calc_mesh_normals(normals, verts, faces, nverts, nfaces);
	for (int i = 0; i < nverts; i++) {
		std::cout << normals[i].x << ' ' << normals[i].y << ' ' << normals[i].z << std::endl;
	}
	delete[] normals;
	delete[] verts;
	delete[] faces;
}