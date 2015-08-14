#include "Utility.h"
#include "Globals.h"
#include "Amber.h"
#include "Ray.h"

#include <SDL/SDL.h>
#include <GL/glew.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

const Color Color::Black(0.f, 0.f, 0.f);
const Color Color::White(1.f, 1.f, 1.f);
const Color Color::Red(1.f, 0.f, 0.f);
const Color Color::Green(0, 1.f, 0.f);
const Color Color::Blue(0.f, 0.f, 1.f);
const Color Color::Yellow(1.f, 1.f, 0.f);
const Color Color::Magenta(1.f, 0.f, 1.f);
const Color Color::Cyan(0.f, 1.f, 1.f);
const Color Color::Transparent(0.f, 0.f, 0.f, 0.f);

Color::Color()
    : Color(1.f, 1.f, 1.f, 1.f)
{

}

Color::Color(float r, float g, float b)
    : Color(r, g, b, 1.f)
{

}

Color::Color(float r, float g, float b, float a)
    : r(r)
    , g(g)
    , b(b)
    , a(a)
{

}

float toDegrees(float valueInRadians)
{
    return 180.f / pi * valueInRadians;
}

float toRadians(float valueInDegrees)
{
    return pi / 180.f * valueInDegrees;
}

bool getLowestPosRoot(float a, float b, float c, float maxR, float* root)
{
    float determinant = b * b - 4 * a * c;

    if (determinant < 0.f)
        return false;

    float sqrtD = sqrt(determinant);
    float x1 = (-b - sqrtD) / (2 * a);
    float x2 = (-b + sqrtD) / (2 * a);

    if (x1 > x2)
    {
        float temp = x2;
        x2 = x1;
        x1 = temp;
    }

    if (x1 > 0 && x1 < maxR)
    {
        *root = x1;
        return true;
    }

    if (x2 > 0 && x2 < maxR)
    {
        *root = x2;
        return true;
    }

    return false;
}

void testVectorsMatricesRays()
{
    std::cout << "-----Vectors-----" << std::endl;
    Vector3f v1(1.f, 1.f, 1.f);
    Vector3f v2(0.f, -3.f, 8.f);

    std::cout << "Vector v1" << std::endl;
    v1.toString();
    std::cout << "Vector v2" << std::endl;
    v2.toString();

    std::cout << "\nNormalization of vector (0,-3,8)" << std::endl;
    std::cout << normalize(v2).toString();

    std::cout << "\nMagnitude of vector (0,-3,8)" << std::endl;
    std::cout << "Result: " << v2.length() << std::endl;

    std::cout << "\nInversion of vector (0,-3,8)" << std::endl;
    std::cout << inverse(v2).toString();

    std::cout << "\nAddition of (1,1,1) & (0,-3,8)" << std::endl;
    (v1 + v2).toString();

    std::cout << "\nSubtraction of (1,1,1) & (0,-3,8)" << std::endl;
    (v1 - v2).toString();

    std::cout << "\nDot product of (1,1,1) & (0,-3,8)" << std::endl;
    std::cout << "Result: " << dot(v1, v2) << std::endl;

    std::cout << "\nCross product of (1,1,1) & (0,-3,8)" << std::endl;
    cross(v1, v2).toString();

    std::cout << "\nScaling of vector (1,1,1) 5 times" << std::endl;
    (5 * v1).toString();
    (v1 * 5).toString();

    std::cout << "\nEquality of vector (1,1,1) & (0,-3,8)" << std::endl;
    if (v1 == v2)
        std::cout << "(1,1,1) = (0,-3,8)" << std::endl;
    else
        std::cout << "(1,1,1) != (0,-3,8)" << std::endl;

    std::cout << "\nDot product of vector (0,-3,8) and its inverse" << std::endl;
    std::cout << "Result: " << dot(v2, inverse(v2)) << std::endl;

    std::cout << "\n-----Matrices-----" << std::endl;
    Matrix3x3f m1;
    m1.data[0] = 1.f;
    m1.data[1] = 4.f;
    m1.data[2] = 7.f;
    m1.data[3] = 2.f;
    m1.data[4] = 5.f;
    m1.data[5] = 8.f;
    m1.data[6] = 3.f;
    m1.data[7] = 6.f;
    m1.data[8] = 10.f;
    Matrix3x3f m2;
    m2.data[0] = 1.f;
    m2.data[1] = 1.f;
    m2.data[2] = 1.f;
    m2.data[3] = 1.f;
    m2.data[4] = 1.f;
    m2.data[5] = 1.f;
    m2.data[6] = 1.f;
    m2.data[7] = 1.f;
    m2.data[8] = 1.f;

    std::cout << "Matrix m1" << std::endl;
    m1.toString();
    std::cout << "Matrix m2" << std::endl;
    m2.toString();

    std::cout << "\nAddition of m1 and m2" << std::endl;
    (m1 + m2).toString();

    std::cout << "\nSubtraction of m1 and m2" << std::endl;
    (m1 - m2).toString();

    std::cout << "\nMatrix multiplication m1*m2" << std::endl;
    (m1 * m2).toString();

    std::cout << "\nMatrix multiplication m2*m1" << std::endl;
    (m2 * m1).toString();

    std::cout << "\nTransposed matrix 1" << std::endl;
    Matrix3x3f m1_tmp = m1;
    m1_tmp.transpose();
    m1_tmp.toString();

    std::cout << "\nInversed matrix 1" << std::endl;
    m1_tmp = m1;
    m1_tmp.inverse();
    m1_tmp.toString();

    std::cout << "\nDeterminant of matrix 1" << std::endl;
    std::cout << "Result: " << m1.determinant() << std::endl;

    std::cout << "\n-----Rays-----" << std::endl;
    Rayf r1(Vector3f(0, 2, -1), Vector3f(1, 1, 2));
    Rayf r2(Vector3f(1, 0, -1), Vector3f(1, 1, 3));

    std::cout << "\nDistance from r1 to r2" << std::endl;
    std::cout << "Result: " << distance(r1, r2) << std::endl;

    std::cout << "\nDistance from r1 to v1" << std::endl;
    std::cout << "Result: " << distance(r1, v1) << std::endl;
}

std::string loadFileToString(const std::string& fileName)
{
    std::ifstream file(fileName);
    std::stringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

std::vector<std::string>& split(const std::string &s, char delim, std::vector<std::string>& elems)
{
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim))
    {
        elems.push_back(item);
    }
    return elems;
}

std::vector<std::string> split(const std::string &s, char delim)
{
    std::vector<std::string> elems;
    split(s, delim, elems);
    return elems;
}

std::string toLower(const std::string& text)
{
    std::string result = text;
    std::transform(text.begin(), text.end(), result.begin(), ::tolower);
    return result;
}

bool compareStringsCaseInsensitive(const std::string& a, const std::string& b)
{
    size_t sz = a.size();
    if (b.size() != sz)
        return false;
    for (unsigned int i = 0; i < sz; ++i)
        if (tolower(a[i]) != tolower(b[i]))
            return false;
    return true;
}


void _checkGlError(const char* file, int line, const char* function)
{
    GLenum err(glGetError());

    while (err != GL_NO_ERROR)
    {
        std::string error;

        switch (err)
        {
            case GL_INVALID_OPERATION:
                error = "INVALID_OPERATION";
                break;
            case GL_INVALID_ENUM:
                error = "INVALID_ENUM";
                break;
            case GL_INVALID_VALUE:
                error = "INVALID_VALUE";
                break;
            case GL_OUT_OF_MEMORY:
                error = "OUT_OF_MEMORY";
                break;
            case GL_INVALID_FRAMEBUFFER_OPERATION:
                error = "INVALID_FRAMEBUFFER_OPERATION";
                break;
        }

        log("GL Error - " + error + " - " + file + ":" + std::to_string(line) + "\n" + function);
        err = glGetError();
    }
}

void log(std::string message)
{
    std::ofstream log_file("log.txt", std::ios_base::out | std::ios_base::app);
    log_file << message << std::endl;
}

void clearLog()
{
	std::ofstream log_file("log.txt", std::ios_base::out | std::ios_base::trunc);
}

void _criticalError(const char* message, const char* file, int line, const char* function)
{
	std::stringstream sstream;
	sstream << message << std::endl << file << ":" << line << "  -  " << function;
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", sstream.str().c_str(), gMainWindow);
	std::exit(1);
}

void _criticalError(const std::string& message, const char* file, int line, const char* function)
{
	_criticalError(message.c_str(), file, line, function);
}

void quit()
{
    SDL_GL_DeleteContext(gContext);
    SDL_DestroyWindow(gMainWindow);
    SDL_Quit();
    std::exit(0);
}
