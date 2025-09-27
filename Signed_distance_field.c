/*
  Unfinished experiments in generating a Signed distance field from a texture. 
*/

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
//#include <stdbool.h>
#include <unistd.h>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_write.h"


#define true 1
#define false 0
#define sq2 1.4142136
#define FAILED 1
#define SUCCEEDED 0

#define OPAQUE_THRESH 200
#define ALPHA_THRESH 25



static inline float signf(float f) { // return -1.0 if negative 1.0 if positive
	if (f < 0) return -1.0;
	return 1.0;
}

typedef struct {
	int x;
	int y;
} Pointi;

typedef struct {
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char a;
} Color;


int imax(int a, int b) {
	if (a > b) { return a; }
	return b;
}

void** allocate2DArray(int height, int width, size_t elementSize) { // USES CALLOC!
    // Allocate memory for an array of pointers to height
	void** array = (void**)calloc(height, sizeof(void*));

    if (array == NULL) {
        perror("Failed to allocate memory for height");
        exit(EXIT_FAILURE);
    }

    // Allocate memory for each row
    for (int i = 0; i < height; i++) {
		array[i] = calloc(width, elementSize);
        if (array[i] == NULL) {
            perror("Failed to allocate memory for columns");
            exit(EXIT_FAILURE);
        }
    }

    return array;
}

 
Color** stbi_image_to_2D_color_array(int height, int width, unsigned char *data, int channels) {
	// change it to a 2D grid of colors to make  things easier.
	Color **grid = (Color**)allocate2DArray(height, width, sizeof(Color));	
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			int index = (y * height + x) * channels;
			grid[y][x].r = data[index + 0];
			grid[y][x].g = data[index + 1];
			grid[y][x].b = data[index + 2];
			grid[y][x].a = data[index + 3];
		}
	}

	return grid;
}

void color_aray_to_stbi_image(Color **grid, unsigned char *data, int height, int width, int channels) {
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			int index = (y * height + x) * channels;
			data[index + 0] = grid[y][x].r;
			data[index + 1] = grid[y][x].g;
			data[index + 2] = grid[y][x].b;
			data[index + 3] = grid[y][x].a;
		}
	}
}

void sdf_to_stbi_image(float **sdf, unsigned char *data, int height, int width, int channels, float range_cutoff) {
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			int index = (y * height + x) * channels;
			data[index + 0] = 0;
			data[index + 1] = 0;
			data[index + 2] = 0;
			data[index + 3] = 255;

			float v = sdf[y][x];
			//printf("%f, ", v);
			if (v == 0.5) { data[index + 1] = 255; }
			if (v < 0.5) { 
				data[index + 2] = 255;
			}
			if (v > 0.5) { 
				data[index + 0] = 255;
			}
		}
		//printf("\n");
	}
}


void ppixel(unsigned char* data, int px, int py, int width, int channels) {
	int index = (py * width + px) * channels;
    // Get the pixel color data
    unsigned char r = data[index];     // Red channel
    unsigned char g = data[index + 1]; // Green channel
    unsigned char b = data[index + 2]; // Blue channel
    unsigned char a = data[index + 3]; // alpha channel
	//
	//printf("Pixel color at (%d, %d): R=%d, G=%d, B=%d, A=%d\n", px, py, r, g, b, a);
}


static inline int oob0(int x, int y,int  w, int h) { // out of bounds check, for x,y in range 0->w-1 0->h-1
	return (x >= 0 && x < w && y >= 0 && y < h);
}

static inline int bi_or_bo(unsigned char a1, unsigned char a2) { // test both opaque, or both are transparent
	if (a1 > OPAQUE_THRESH && a2 > OPAQUE_THRESH) return true;
	if (a1 < ALPHA_THRESH && a2 < ALPHA_THRESH) return true;
	return false;
}

static inline void check_neighbor(Color **grid, float **sdf, int width, int height, int x, int y, int nx, int ny) {

	float dton = 1.0;
	if (abs(nx) + abs(ny) == 2) { dton = sq2; }
	// fix dton direction...
	dton = signf(sdf[y][x]) * dton;

	nx = nx + x;
	ny = ny + y;
	
	int in_bounds = oob0(nx, ny, width, height);
	if (!in_bounds) return;
	
	float cur = sdf[y][x];
	float nei = sdf[ny][nx];

	// check valid compare
	if (!( nei == 0.5 || ( signf(cur) == signf(nei) ))) return;

	// if comparing with an edge, the value is just 
	// 1.5 or -1.5. if other inside -> -1.5, or sq2 + .5
	// if other is outisde -> 1.5 -> sq2 + .5
	// inside inside case
	// new value = min(cur, nei + (1 * sign))
	// outer is the same
	// new value = min(cur, nei + (1 * sign))
	if (nei == 0.5) sdf[y][x] = dton;
	else {
		sdf[y][x] = fminf(cur, nei + (dton));
	}

}


int main() {
	int width, height, channels;
	char *filename = "test5x5.png";
	unsigned char *data = stbi_load(filename, &width, &height, &channels, 0);
	
	if (data == NULL) {
		//printf("FAILED!!!\n");
		return 1;
	}
	if (channels != 4) {
		//printf("assuming 4 color channels for now! \n");
		return 1;
	}

	Color **grid = stbi_image_to_2D_color_array(height, width, data, channels);
	unsigned char *scratch = malloc(sizeof(unsigned char) * width * height * channels);

	color_aray_to_stbi_image(grid, scratch, height, width, channels);
	stbi_write_png("reconverttest.png", width, height, channels, scratch, width * channels);

	// used a prepared texture that has had sobel ran on it's edges. 
	// non transparent parts are white, edges are red. 
	
	// Color **grid = (Color**)allocate2DArray(height, width, sizeof(Color));	

	/*
	* Custom implementation....
				sobel gives edge location.
				Just brute force it....
				edges are 0.5
				we know outside is at max sqrt(2)*w (full diagonal)
				use those as start values.
				(negative distance inside)
				first pass, is a neighbor an edge? 
					if so, dist becomes 1, or sqrt(2)
					if inside and neighbor, -1 or -sqrt(2)
				second passes
					do the normal two passes
					ignore any visited sections.
					new becomes neighbor distance + 1 (or sq2)
	*/	

	int **visited = (int**)allocate2DArray(height, width, sizeof(int));
	float **dist = (float**)allocate2DArray(height, width, sizeof(float));

	// setup
	// for (int y = 0; y < height; y++) {
	// 	for (int x = 0; x < width; x++) {
	//
	// 	}
	// }
	
	float large_outside_val = 2.0 * sq2 * imax(width, height);
	float large_inside_val  = -large_outside_val;

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			if (grid[y][x].a > 100) { dist[y][x] = large_inside_val; }// inside
			if (grid[y][x].a <= 100) { dist[y][x] = large_outside_val; }// outside
			if (grid[y][x].r > 245 && grid[y][x].g <= 10 && grid[y][x].b <= 10) { dist[y][x] = 0.5; }// edge
		}
	}


	Pointi n = {0, 0}; // neighbor	
	
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			// check neighbors
			//if (!(y == 1 && x == 1)) continue; // HACK: check

			//if (dist[y][x] == 0.5) printf("%i ", (int)dist[y][x]);
			if (dist[y][x] == 0.5) continue;  // don't touch exact edges!!!			
			
			check_neighbor(grid, dist, width, height, x, y, -1, -1); // TL 
			check_neighbor(grid, dist, width, height, x, y,  0, -1); // T 
			check_neighbor(grid, dist, width, height, x, y,  1, -1); // TR 
			check_neighbor(grid, dist, width, height, x, y, -1,  0); // L 

			// printf("%i ", (int)dist[y][x]);

			////printf("%i %i v: %f \n", y, x, dist[y][x]);
			// system("magick pass1.png -scale 3200% pass1.png");
			//scanf("%c", &c);
			//getchar();


		}
	}
	for (int y = height -1; y >= 0; y--) {
		for (int x = width -1; x >= 0; x--) {
			// check neighbors
			//if (!(y == 1 && x == 1)) continue; // HACK: check

			//if (dist[y][x] == 0.5) printf("%i ", (int)dist[y][x]);
			if (dist[y][x] == 0.5) continue;  // don't touch exact edges!!!			
			
			check_neighbor(grid, dist, width, height, x, y,  1,  1); // BR 
			check_neighbor(grid, dist, width, height, x, y,  0,  1); // B
			check_neighbor(grid, dist, width, height, x, y, -1,  1); // BL 
			check_neighbor(grid, dist, width, height, x, y,  1,  0); // R 

			// printf("%i ", (int)dist[y][x]);

			////printf("%i %i v: %f \n", y, x, dist[y][x]);
			// system("magick pass1.png -scale 3200% pass1.png");
			//scanf("%c", &c);
			//getchar();


		}
	}


	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			printf("%i ", (int)dist[y][x]);
		}
		printf("\n");
	}

	sdf_to_stbi_image(dist, scratch, height, width, channels, 20.0);
	stbi_write_png("pass1.png", width, height, channels, scratch, width * channels);
	system("magick pass1.png -scale 6000% pass1.png");

	//printf("large val: %f \n", large_outside_val);



	return 0;
}
