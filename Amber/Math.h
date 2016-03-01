#pragma once

constexpr float PI = 3.141592653589793238462643383f;
constexpr float PI_HALF = 1.570796326794896619231321691f;

float toDegrees(float valueInRadians);
float toRadians(float valueInDegrees);

bool getLowestPosRoot(float a, float b, float c, float maxR, float* root);

// TODO: make actual tests, preferably unit tests
void testVectorsMatricesRays();