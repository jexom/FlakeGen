// CUDAflake.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "stdafx.h"
#include <math.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <Windows.h>
#include <time.h>

int maxIt = 10000;
double alpha = 0.502;
double beta = 0.4;
double gamma = 0.0001;
double beta2 = 0.4;
double gamma2 = 0.0001;

int mx = 400; 
int my = mx; // width and height of 2DCA
int mz = mx;
int dx[6] = { -1, 0, -1, 1, 0, 1 };
int dy[6] = { 1, -1, 0, 0, 1, -1 }; // 6 directions to grow
int dz[6] = { 0, 1, 1, -1, -1, 0 }; // 6 directions to grow


int offset(int x, int y, int z, int m) {
	int mx = m;
	int my = mx; // width and height of 2DCA
	int mz = mx;
	return (x + y * (mx + 1) + z * (mx + 1) * (my + 1));
}

void mirror(double *m, int ms, int ix, int iy) {
	int iz = -ix - iy;
	
	int x = (int)(ms / 2) + ix;
	int y = (int)(ms / 2) + iy;
	int z = (int)(ms / 2) + iz;

	int q = (int)(ms / 2) - ix;
	int r = (int)(ms / 2) - iy;
	int s = (int)(ms / 2) - iz;

	float v = m[offset(x,y,z, ms)];

	m[offset(y, x, z, ms)] = v;
	m[offset(s, r, q, ms)] = v;
	m[offset(x, z, y, ms)] = v;
	m[offset(r, q, s, ms)] = v;
	m[offset(z, y, x, ms)] = v;
	m[offset(q, s, r, ms)] = v;

	m[offset(q, r, s, ms)] = v;
	m[offset(z, x, y, ms)] = v;
	m[offset(y, z, x, ms)] = v;
	m[offset(r, s, q, ms)] = v;
	m[offset(s, q, r, ms)] = v;
}

int main(int argc, char* argv[])
{
	HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);

	CONSOLE_CURSOR_INFO     cursorInfo;

	GetConsoleCursorInfo(out, &cursorInfo);
	cursorInfo.bVisible = false; // set the cursor visibility
	SetConsoleCursorInfo(out, &cursorInfo);

	srand(static_cast <unsigned> (time(0)));

	if (argc == 2) {
		maxIt = atoi(argv[1]);
		alpha = (static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) * 1.5 + 0.5;
		beta = (static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) * 0.3 + 0.3;
		gamma = (static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) * 0.01;
		std::cout << "alpha = " << alpha << " | beta = " << beta << " | gamma = " << gamma << "\n";
		beta2 = beta;
		gamma2 = gamma;
	}
	else if (argc == 3) {
		maxIt = atoi(argv[1]);
		mx = atoi(argv[2]) * 2;
		my = atoi(argv[2]) * 2;
		mz = atoi(argv[2]) * 2;
		alpha = (static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) * 1.5 + 0.5;
		beta = (static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) * 0.3 + 0.3;
		gamma = (static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) * 0.01;
		std::cout << "alpha = " << alpha << " | beta = " << beta << " | gamma = " << gamma << "\n";
		beta2 = beta;
		gamma2 = gamma;
	}
	else if (argc == 6) {
		maxIt = atoi(argv[1]);
		mx = atoi(argv[2]) * 2;
		my = atoi(argv[2]) * 2;
		mz = atoi(argv[2]) * 2;

		alpha = atof(argv[3]);
		beta = atof(argv[4]);
		gamma = atof(argv[5]);

		beta2 = beta;
		gamma2 = gamma;
	}
	else if (argc == 7) {
		maxIt = atoi(argv[1]);
		mx = atoi(argv[2]) * 2;
		my = atoi(argv[2]) * 2;
		mz = atoi(argv[2]) * 2;

		alpha = atof(argv[3]);
		beta = atof(argv[4]);
		gamma = atof(argv[5]);

		beta2 = atof(argv[6]);
		gamma2 = gamma;
	}
	else if (argc > 7) {
		maxIt = atoi(argv[1]);
		mx = atoi(argv[2]) * 2;
		my = atoi(argv[2]) * 2;
		mz = atoi(argv[2]) * 2;

		alpha = atof(argv[3]);
		beta = atof(argv[4]);
		gamma = atof(argv[5]);

		beta2 = atof(argv[6]);
		gamma2 = atof(argv[7]);
	}
	else if (argc == 4) {
		std::cout << "Usage: OMPflake [iterations] [radius] [alpha beta gamma]\niterations - number of cycles (default = 10000)\nradius - radius of a snowflake in tiles (default = 200)\n alpha, beta, gamma - computational parameters (deafult: random)\n";
		exit(0);
	}
	else {
		alpha = (static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) + 0.5;
		beta = (static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) * 0.3 + 0.2;
		gamma = (static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) * 0.01;
		std::cout << "alpha = " << alpha << " | beta = " << beta << " | gamma = " << gamma << "\n";
		beta2 = beta;
		gamma2 = gamma;
	}

	std::cout << "Loading...\r";

	double *ca = new double[(mx + 1) * (my + 1) * (mz + 1)];
	double *caRep = new double[(mx + 1) * (my + 1) * (mz + 1)]; // receptive cells
	double *caNRep = new double[(mx + 1) * (my + 1) * (mz + 1)]; // non - receptive cells

	for (int ix = (int)(-mx/2); ix <= (int)(mx/2); ix++) {
		for (int iy = (int)(-my/2); iy <= (int)(my/2); iy++) {
			for (int iz = (int)(-mz / 2); iz <= (int)(mz / 2); iz++) {
				int x = (int)(mx/2) + ix;
				int y = (int)(my/2) + iy;
				int z = (int)(mz/2) + iz;
				ca[offset(x, y, z, mx)] = beta;
				caRep[offset(x, y, z, mx)] = beta;
				caNRep[offset(x, y, z, mx)] = beta;
			}
		}
	}

	ca[offset((int)(mx/2), (int)(my/2), (int)(mz/2), mx)] = 1;

	int blocks = 2;
	int threads = 10;
	int steps = (int)(((mx + 1) * (mx + 1) * (mx + 1))/(blocks * threads));

	int size = sizeof(float) * (mx + 1) * (mx + 1) * (mx + 1);
	
	int symCount = 30 - 1;
	int barperc = maxIt / symCount;

	for (int i = 0; i < maxIt; i++) {
		std::cout << "[";
		int a = i / barperc;
		for (int bar = 0; bar < a + 1; bar ++) {
			std::cout << "=";
		}
		std::cout << ">";
		for (int bar = 0; bar <= symCount-a-1; bar++) {
			std::cout << " ";
		}
		std::cout << "] ";
		std::cout <<"Iteration " << i << " of "  << maxIt << "\r" << std::flush;

		if (i == (int)(9.99 * maxIt / 10)) {
			beta = beta2;
			gamma = gamma2;
		}
		
		#pragma omp parallel for
		for (int ix = 0; ix <= (int)(mx / 2); ix++) {
			for (int iy = 0; iy <= ix; iy++) {
				//for (int iz = (int)(-mz / 2); iz <= (int)(mz / 2); iz++) {
				int iz = -ix - iy;
					//if (ix + iy + iz == 0) {
				if (abs(iz) <= (int)((mx + 1) / 2)) {
					int x = (int)(mx / 2) + ix;
					int y = (int)(my / 2) + iy;
					int z = (int)(mz / 2) + iz;
					bool receptive = false;
					if (ca[offset(x, y, z, mx)] >= 1) {
						receptive = true;
					}
					else {
						for (int j = 0; j < 6; j++) {
							int jx = ix + dx[j];
							int jy = iy + dy[j];
							int jz = iz + dz[j];
							int jxx = (int)(mx / 2) + jx;
							int jyy = (int)(my / 2) + jy;
							int jzz = (int)(mz / 2) + jz;
							if ((jxx >= 0) && (jxx <= mx) && (jyy >= 0) && (jyy <= my) && (jzz >= 0) && (jzz <= mz)) {
								if (ca[offset(jxx, jyy, jzz, mx)] >= 1) {
									receptive = true;
									break;
								}
							}
						}
					}

					if (receptive) {
						caRep[offset(x, y, z, mx)] = ca[offset(x, y, z, mx)] + gamma;
						caNRep[offset(x, y, z, mx)] = 0;
					}
					else {
						caRep[offset(x, y, z, mx)] = 0;
						caNRep[offset(x, y, z, mx)] = ca[offset(x, y, z, mx)];
					}
					mirror(caRep, mx, ix, iy);
					mirror(caNRep, mx, ix, iy);
				}
				//}
			}
		}

		#pragma omp parallel for
		for (int ix = 0; ix <= (int)(mx / 2); ix++) {
			for (int iy = 0; iy <= ix; iy++) {
				//for (int iz = (int)(-mz / 2); iz <= (int)(mz / 2); iz++) {
				int iz = -ix - iy;
					//if (ix + iy + iz == 0) {
					if (abs(iz) <= (int)((mx + 1) / 2)) {
						int x = (int)(mx/2) + ix;
						int y = (int)(my/2) + iy;
						int z = (int)(mz/2) + iz;
						double wsum = 0;
						//float wsum = caNRep[iy*mx + ix + iz * mx * my] * (1.0 - alpha * 6.0 / 12.0);
						for (int j = 0; j < 6; j++) {
							int jx = ix + dx[j];
							int jy = iy + dy[j];
							int jz = iz + dz[j];
							int jxx = (int)(mx / 2) + jx;
							int jyy = (int)(my / 2) + jy;
							int jzz = (int)(mz / 2) + jz;
							if ((jxx >= 0) && (jxx <= mx) && (jyy >= 0) && (jyy <= my) && (jzz >= 0) && (jzz <= mz)) {
									wsum += caNRep[offset(jxx, jyy, jzz, mx)];// *alpha / 12;
							}
						}
						caNRep[offset(x, y, z, mx)] = caNRep[offset(x, y, z, mx)] + (alpha / 12)*(wsum - 6 * caNRep[offset(x, y, z, mx)]);
						ca[offset(x, y, z, mx)] = caRep[offset(x, y, z, mx)] + caNRep[offset(x, y, z, mx)];
						mirror(ca, mx, ix, iy);
					}
				//}
			}
		}
	}

	std::cout << "\n";

	//std::ofstream myfile;
	std::ofstream myfile2;
	//myfile.open("example.txt");
	myfile2.open("example.txt");

	//myfile << mx + 1 << "\n";
	myfile2 << mx + 1 << "\n";

	for (int ix = (int)(-mx / 2); ix <= (int)(mx / 2); ix++) {
		int r1 = max((int)(-mx/2), (int)(-ix - mx/2));
		int r2 = min((int)(mx / 2), (int)(-ix + mx/2));
		for (int i = 1; i <= (int)(floor(abs(ix)/2)); i++) {
			//printf("    ");
			//myfile << "0\t";
			myfile2 << "0\t";
		}
		for (int r = r1; r <= r2; r++) {
			int x = ix + (int)(mx/2);
			int y = r + (int)(my/2);
			int iz = -ix - r; int z = iz + (int)(mz/2);
			if (z >= 0) {
				if (ca[offset(x, y, z, mx)] >= 1) {
					//printf("|%.2f|", ca[offset(x, y, z, mx)]);
					//myfile << (int)(ca[offset(x, y, z, mx)]/ca[offset((int)(mx/2), (int)(mx / 2), (int)(mx / 2), mx)]*255) << "\t0\t";
					myfile2 << (int)(ca[offset(x, y, z, mx)] / ca[offset((int)(mx / 2), (int)(mx / 2), (int)(mx / 2), mx)] * 255) << "\t";
				}
				else {
					//printf("|%.2f|", 0);
					//myfile << 0 << "0\t";
					myfile2 << 0 << "0\t";
				}
			}
		}
		for (int i = 1; i <= (int)(ceil(abs(ix) / 2)); i++) {
			//printf("    ");
			//myfile << "0\t";
			myfile2 << "0\t";
		}
		//printf("\n");
		//myfile << "\n";
		myfile2 << "\n";
	}
	//myfile.close();
	myfile2.close();
	cursorInfo.bVisible = true; // set the cursor visibility
	SetConsoleCursorInfo(out, &cursorInfo);

	delete[] ca;
	delete[] caRep;
	delete[] caNRep;
}