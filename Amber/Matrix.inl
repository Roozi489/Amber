#pragma once
#include "Matrix.h"
#include "Vector.h"

#include <sstream>

template <typename T>
Matrix4x4<T>::Matrix4x4(T value)
{
    for (int i = 0; i < dataLength; ++i)
        data[i] = value;
}

template <typename T>
Matrix4x4<T>::Matrix4x4(const Matrix4x4& other)
{
    for (int i = 0; i < dataLength; ++i)
        data[i] = other.data[i];
}

template <typename T>
Matrix4x4<T>& Matrix4x4<T>::operator=(const Matrix4x4& other)
{
    if (this != &other)
    {
        for (int i = 0; i < dataLength; ++i)
            data[i] = other.data[i];
    }

    return *this;
}

template <typename T>
T* Matrix4x4<T>::operator[](int column)
{
    return &data[column * rows];
}

template <typename T>
T Matrix4x4<T>::getValue(int column, int row) const
{
    return data[column * rows + row];
}

template <typename T>
void Matrix4x4<T>::setValue(int column, int row, T value)
{
    data[column * rows + row] = value;
}

template <typename T>
void Matrix4x4<T>::init(T value = 0)
{
    for (int i = 0; i < dataLength; ++i)
        data[i] = value;
}

template <typename T>
void Matrix4x4<T>::fillDiag(T value)
{
    data[0] = value;
    data[5] = value;
    data[10] = value;
    data[15] = value;
}

template <typename T>
void Matrix4x4<T>::transpose()
{
    auto copy = *this;
    for (int x = 0; x < columns; ++x)
    {
        for (int y = 0; y < rows; ++y)
        {
            setValue(y, x, copy.getValue(x, y));
        }
    }
}

template <typename T>
T Matrix4x4<T>::determinant()
{
    T result = 0;
    for (int y = 0; y < rows; ++y)
    {
        T firstColumnValue = getValue(0, y);
        Matrix3x3<T> smallMatrix;
        int smallMatrixY = 0;
        for (int i = 0; i < rows; ++i)
        {
            // don't use the row of firstColumnValue
            if (y == i)
                continue;

            // for each column in smallMatrix
            for (int x = 0; x < 3; ++x)
                smallMatrix[x][smallMatrixY] = getValue(x + 1, i);

            ++smallMatrixY;
        }
        result += firstColumnValue * (std::pow(static_cast<T>(-1), static_cast<T>(y)))  * smallMatrix.determinant();
    }
    return result;
}

template <typename T>
void Matrix4x4<T>::isInvertible()
{
    return determinant() != 0;
}

template <typename T>
void Matrix4x4<T>::inverse()
{
    Matrix4x4f leftMatrix = *this;
    // create identity matrix
    Matrix4x4f rightMatrix = Matrix4x4::identity();

    for (int y = 0; y < rows; ++y)
    {
        // Get the first non-zero value in the row
        auto pivotValue = leftMatrix.getValue(y, y);
        // when pivot is 0 -> need to swap rows
        if (pivotValue == 0)
        {
            // no more rows to swap -> matrix is singular
            if (y == rows - 1)
            {
                criticalError("ERROR: Matrix4x4::inverse - attempt to inverse singular matrix.");
            }

            int swapWithRowIndex = y + 1;
            T valueOnNextLine = leftMatrix.getValue(y, y + 1);

            if (valueOnNextLine == 0 && y == 0)
            {
                for (int i = y; y < rows; ++y)
                {
                    swapWithRowIndex = i;
                    valueOnNextLine = leftMatrix.getValue(y, swapWithRowIndex);
                    if (valueOnNextLine != 0)
                        break;
                }
            }

            if (valueOnNextLine == 0)
            {
                criticalError("ERROR: Matrix4x4::inverse - attempt to inverse singular matrix.");
            }

            // swap rows
            for (int x = 0; x < columns; ++x)
            {
                auto x1 = leftMatrix.getValue(x, y);
                leftMatrix[x][y] = leftMatrix.getValue(x, swapWithRowIndex);
                leftMatrix[x][swapWithRowIndex] = x1;

                x1 = rightMatrix.getValue(x, y);
                rightMatrix[x][y] = rightMatrix.getValue(x, swapWithRowIndex);
                rightMatrix[x][swapWithRowIndex] = x1;
            }

            pivotValue = leftMatrix.getValue(y, y);
        }

        // for each non zero value in the row
        for (int x = 0; x < columns; ++x)
        {
            // divide by the pivot value -> pivot becomes 1
            auto prevValue = leftMatrix.getValue(x, y);
            leftMatrix[x][y] = prevValue / pivotValue;

            // do the same for the right matrix
            prevValue = rightMatrix.getValue(x, y);
            rightMatrix[x][y] = prevValue / pivotValue;
        }

        // subtract from remaining rows
        for (int k = 1; k < rows - y; ++k)
        {
            // get the first value on the next row (how many time to subtract 1 from the next row)
            auto nextRowValue = leftMatrix.getValue(y, y + k);
            // go through columns
            for (int j = 0; j < columns; ++j)
            {
                auto upperRowValue = leftMatrix.getValue(j, y);
                auto lowerRowValue = leftMatrix.getValue(j, y + k);
                // subtract the upper value from the lower nextRowValue-times
                leftMatrix[j][y + k] = lowerRowValue - upperRowValue * nextRowValue;

                // the same for right matrix
                upperRowValue = rightMatrix.getValue(j, y);
                lowerRowValue = rightMatrix.getValue(j, y + k);
                rightMatrix[j][y + k] = lowerRowValue - upperRowValue * nextRowValue;
            }
        }
    }

    // now the opposite side of the diagonal
    // for each remaining row
    for (int y = 1; y < rows; ++y)
    {
        auto pivotValue = leftMatrix.getValue(y, y);
        // going from current row upwards
        for (int k = 1; k < y + 1; ++k)
        {
            auto prevValue = leftMatrix.getValue(y, y - k);
            auto timesToSubtract = pivotValue * prevValue;
            for (int x = 0; x < columns; ++x)
            {
                prevValue = leftMatrix.getValue(x, y - k);
                leftMatrix[x][y - k] = prevValue - timesToSubtract * leftMatrix.getValue(x, y);

                // same for the right matrix
                prevValue = rightMatrix.getValue(x, y - k);
                rightMatrix[x][y - k] = prevValue - timesToSubtract * rightMatrix.getValue(x, y);
            }
        }
    }

    *this = rightMatrix;
}

template <typename T>
std::string Matrix4x4<T>::toString() const
{
    std::stringstream s;
    for (int y = 0; y < rows; ++y)
    {
        for (int x = 0; x < columns; ++x)
        {
            s << data[x * rows + y] << " ";
        }
        s << std::endl;
    }
    return s.str();
}

template <typename T>
Matrix4x4<T> Matrix4x4<T>::operator+(const Matrix4x4& rhs)
{
    Matrix4x4 mat;
    for (int i = 0; i < dataLength; ++i)
        mat.data[i] = data[i] + rhs.data[i];

    return mat;
}

template <typename T>
Matrix4x4<T> Matrix4x4<T>::operator-(const Matrix4x4& rhs)
{
    Matrix4x4 mat;
    for (int i = 0; i < dataLength; ++i)
        mat.data[i] = data[i] - rhs.data[i];

    return mat;
}

template <typename T>
Matrix4x4<T> Matrix4x4<T>::operator*(const Matrix4x4& rhs)
{
    Matrix4x4 newMatrix;
    for (int i = 0; i < rows; ++i)
    {
        for (int j = 0; j < columns; ++j)
        {
            T value = 0.0;
            for (int k = 0; k < rows; ++k)
            {
                value += getValue(k, i) * rhs.getValue(j, k);
            }
            newMatrix[j][i] = value;
        }
    }
    return newMatrix;
}

template <typename T>
Matrix4x4<T> Matrix4x4<T>::operator*(const T& rhs)
{
    Matrix4x4 newMatrix;
    for (int i = 0; i < dataLength; ++i) newMatrix.data[i] = data[i] * rhs;
    return newMatrix;
}

template <typename T>
Matrix4x4<T> Matrix4x4<T>::operator/(const T& rhs)
{
    Matrix4x4 newMatrix;
    for (int i = 0; i < dataLength; ++i) newMatrix.data[i] = data[i] / rhs;
    return newMatrix;
}

template <typename T>
Vector3<T> Matrix4x4<T>::operator*(const Vector3<T>& rhs)
{
    Vector3<T> tmp;
    tmp.x = rhs.x * getValue(0, 0) + rhs.y * getValue(1, 0) + rhs.z * getValue(2, 0) + getValue(3, 0);
    tmp.y = rhs.x * getValue(0, 1) + rhs.y * getValue(1, 1) + rhs.z * getValue(2, 1) + getValue(3, 1);
    tmp.z = rhs.x * getValue(0, 2) + rhs.y * getValue(1, 2) + rhs.z * getValue(2, 2) + getValue(3, 2);
    return tmp;
}

template <typename T>
Matrix4x4<T> operator*(const T& lhs, const Matrix4x4<T>& rhs)
{
    Matrix4x4<T> newMatrix;
    for (int i = 0; i < dataLenght; ++i) newMatrix.data[i] = lhs * rhs.data[i];
    return newMatrix;
}

template <typename T>
Matrix4x4<T> operator/(const T& lhs, const Matrix4x4<T>& rhs)
{
    Matrix4x4<T> newMatrix;
    for (int i = 0; i < dataLenght; ++i) newMatrix.data[i] = lhs / rhs.data[i];
    return newMatrix;
}


template <typename T>
Matrix4x4<T> Matrix4x4<T>::identity()
{
    Matrix4x4 identityMatrix;
    identityMatrix.data[0] = 1.0;
    identityMatrix.data[1] = 0.0;
    identityMatrix.data[2] = 0.0;
    identityMatrix.data[3] = 0.0;
    identityMatrix.data[4] = 0.0;
    identityMatrix.data[5] = 1.0;
    identityMatrix.data[6] = 0.0;
    identityMatrix.data[7] = 0.0;
    identityMatrix.data[8] = 0.0;
    identityMatrix.data[9] = 0.0;
    identityMatrix.data[10] = 1.0;
    identityMatrix.data[11] = 0.0;
    identityMatrix.data[12] = 0.0;
    identityMatrix.data[13] = 0.0;
    identityMatrix.data[14] = 0.0;
    identityMatrix.data[15] = 1.0;
    return identityMatrix;
}

template <typename T>
Matrix4x4<T> Matrix4x4<T>::translate(Vector3<T>& v)
{
    return translate(Matrix4x4f::identity(), v);
}

template <typename T>
Matrix4x4<T> Matrix4x4<T>::translate(Matrix4x4<T>& matrix, Vector3<T>& v)
{
    Matrix4x4<T> result(matrix);
    result[3][0] = matrix[0][0] * v.x + matrix[1][0] * v.y + matrix[2][0] * v.z + matrix[3][0];
    result[3][1] = matrix[0][1] * v.x + matrix[1][1] * v.y + matrix[2][1] * v.z + matrix[3][1];
    result[3][2] = matrix[0][2] * v.x + matrix[1][2] * v.y + matrix[2][2] * v.z + matrix[3][2];
    result[3][3] = matrix[0][3] * v.x + matrix[1][3] * v.y + matrix[2][3] * v.z + matrix[3][3];
    return result;
}

template <typename T>
Matrix4x4<T> Matrix4x4<T>::rotate(T angle, Vector3<T> axis)
{
    return rotate(Matrix4x4f::identity(), angle, axis);
}

template <typename T>
Matrix4x4<T> Matrix4x4<T>::rotate(Matrix4x4<T>& matrix, T angle, Vector3<T> axis)
{
    T c = cos(angle);
    T s = sin(angle);

    axis.normalize();

    Vector3<T> temp = T(1 - c) * axis;

    Matrix4x4<T> rot(0);
    rot[0][0] = c + temp.x * axis.x;
    rot[0][1] = 0 + temp.x * axis.y + s * axis.z;
    rot[0][2] = 0 + temp.x * axis.z - s * axis.y;

    rot[1][0] = 0 + temp.y * axis.x - s * axis.z;
    rot[1][1] = c + temp.y * axis.y;
    rot[1][2] = 0 + temp.y * axis.z + s * axis.x;

    rot[2][0] = 0 + temp.z * axis.x + s * axis.y;
    rot[2][1] = 0 + temp.z * axis.y - s * axis.x;
    rot[2][2] = c + temp.z * axis.z;

    Matrix4x4<T> result(0.f);
    result[0][0] = matrix[0][0] * rot[0][0] + matrix[1][0] * rot[0][1] + matrix[2][0] * rot[0][2];
    result[0][1] = matrix[0][1] * rot[0][0] + matrix[1][1] * rot[0][1] + matrix[2][1] * rot[0][2];
    result[0][2] = matrix[0][2] * rot[0][0] + matrix[1][2] * rot[0][1] + matrix[2][2] * rot[0][2];
    result[0][3] = matrix[0][3] * rot[0][0] + matrix[1][3] * rot[0][1] + matrix[2][3] * rot[0][2];
    result[1][0] = matrix[0][0] * rot[1][0] + matrix[1][0] * rot[1][1] + matrix[2][0] * rot[1][2];
    result[1][1] = matrix[0][1] * rot[1][0] + matrix[1][1] * rot[1][1] + matrix[2][1] * rot[1][2];
    result[1][2] = matrix[0][2] * rot[1][0] + matrix[1][2] * rot[1][1] + matrix[2][2] * rot[1][2];
    result[1][3] = matrix[0][3] * rot[1][0] + matrix[1][3] * rot[1][1] + matrix[2][3] * rot[1][2];
    result[2][0] = matrix[0][0] * rot[2][0] + matrix[1][0] * rot[2][1] + matrix[2][0] * rot[2][2];
    result[2][1] = matrix[0][1] * rot[2][0] + matrix[1][1] * rot[2][1] + matrix[2][1] * rot[2][2];
    result[2][2] = matrix[0][2] * rot[2][0] + matrix[1][2] * rot[2][1] + matrix[2][2] * rot[2][2];
    result[2][3] = matrix[0][3] * rot[2][0] + matrix[1][3] * rot[2][1] + matrix[2][3] * rot[2][2];
    result[3][0] = matrix[3][0];
    result[3][1] = matrix[3][1];
    result[3][2] = matrix[3][2];
    result[3][3] = matrix[3][3];
    return result;
}

template <typename T>
Matrix4x4<T> Matrix4x4<T>::perspectiveFov(float fov, float width, float height, float zNear, float zFar)
{
    T h = cos(T(0.5) * fov) / sin(T(0.5) * fov);
    T w = h * height / width;

    Matrix4x4<T> result(0);
    result[0][0] = w;
    result[1][1] = h;
    result[2][2] = -(zFar + zNear) / (zFar - zNear);
    result[2][3] = -T(1);
    result[3][2] = -(T(2) * zFar * zNear) / (zFar - zNear);
    return result;
}

template <typename T>
Matrix4x4<T> Matrix4x4<T>::lookAt(Vector3<T>& eye, Vector3<T> center, Vector3<T> up)
{
    Vector3<T> f(normalize(center - eye));
    Vector3<T> s(normalize(cross(f, up)));
    Vector3<T> u(cross(s, f));

    Matrix4x4<T> result(0);
    result.fillDiag(1);

    result[0][0] = s.x;
    result[1][0] = s.y;
    result[2][0] = s.z;
    result[0][1] = u.x;
    result[1][1] = u.y;
    result[2][1] = u.z;
    result[0][2] = -f.x;
    result[1][2] = -f.y;
    result[2][2] = -f.z;
    result[3][0] = -dot(s, eye);
    result[3][1] = -dot(u, eye);
    result[3][2] = dot(f, eye);
    return result;
}

template <typename T>
Matrix4x4<T> inverse(const Matrix4x4<T>& matrix)
{
    Matrix4x4f result = matrix;
    result.inverse();
    return result;
}

template <typename T>
Matrix4x4<T> transpose(const Matrix4x4<T>& matrix)
{
    Matrix4x4<T> result = matrix;
    Matrix4x4<T> copy = matrix;
    for (int x = 0; x < result.columns; ++x)
    {
        for (int y = 0; y < result.rows; ++y)
        {
            result.setValue(y, x, copy.getValue(x, y));
        }
    }
    return result;
}


//
//
//	Matrix3x3
//
//

template <typename T>
Matrix3x3<T>::Matrix3x3(const Matrix3x3& other)
{
    for (int i = 0; i < dataLength; ++i)
        data[i] = other.data[i];
}

template <typename T>
Matrix3x3<T>& Matrix3x3<T>::operator=(const Matrix3x3& other)
{
    if (this != &other)
    {
        for (int i = 0; i < dataLength; ++i)
            data[i] = other.data[i];
    }

    return *this;
}

template <typename T>
T* Matrix3x3<T>::operator[](int column)
{
    return &data[column * rows];
}

template <typename T>
T Matrix3x3<T>::getValue(int column, int row) const
{
    return data[column * rows + row];
}

template <typename T>
void Matrix3x3<T>::setValue(int column, int row, T value)
{
    data[column * rows + row] = value;
}

template <typename T>
void Matrix3x3<T>::init(T value = 0)
{
    for (int i = 0; i < dataLength; ++i)
        data[i] = value;
}

template <typename T>
void Matrix3x3<T>::transpose()
{
    auto copy = *this;
    for (int x = 0; x < columns; ++x)
    {
        for (int y = 0; y < rows; ++y)
        {
            setValue(y, x, copy.getValue(x, y));
        }
    }
}

template <typename T>
T Matrix3x3<T>::determinant()
{
    auto val1 = getValue(0, 0) * getValue(1, 1) * getValue(2, 2) + getValue(0, 1) * getValue(1, 2) * getValue(2, 0) + getValue(0, 2) * getValue(1, 0) * getValue(2, 1);
    auto val2 = getValue(0, 2) * getValue(1, 1) * getValue(2, 0) + getValue(0, 0) * getValue(1, 2) * getValue(2, 1) + getValue(0, 1) * getValue(1, 0) * getValue(2, 2);
    return val1 - val2;
}

template <typename T>
void Matrix3x3<T>::isInvertible()
{
    return determinant() != 0;
}

template <typename T>
void Matrix3x3<T>::inverse()
{
    Matrix3x3 leftMatrix = *this;
    // create identity matrix
    Matrix3x3 rightMatrix = Matrix3x3::identity();

    for (int y = 0; y < rows; ++y)
    {
        // Get the first non-zero value in the row
        auto pivotValue = leftMatrix.getValue(y, y);
        // when pivot is 0 -> need to swap rows
        if (pivotValue == 0)
        {
            // no more rows to swap -> matrix is singular
            if (y == rows - 1)
            {
                criticalError("ERROR: Matrix3x3::inverse - attempt to inverse singular matrix.");
            }

            int swapWithRowIndex = y + 1;
            T valueOnNextLine = leftMatrix.getValue(y, y + 1);
            // if 0 try the last row if possible
            if (valueOnNextLine == 0 && y == 0)
            {
                swapWithRowIndex = y + 2;
                valueOnNextLine = leftMatrix.getValue(y, swapWithRowIndex);
            }

            if (valueOnNextLine == 0)
            {
                criticalError("ERROR: Matrix3x3::inverse - attempt to inverse singular matrix.");
            }

            // swap rows
            for (int x = 0; x < columns; ++x)
            {
                auto x1 = leftMatrix.getValue(x, y);
                leftMatrix[x][y] = leftMatrix.getValue(x, swapWithRowIndex);
                leftMatrix[x][swapWithRowIndex] = x1;

                x1 = rightMatrix.getValue(x, y);
                rightMatrix[x][y] = rightMatrix.getValue(x, swapWithRowIndex);
                rightMatrix[x][swapWithRowIndex] = x1;
            }

            pivotValue = leftMatrix.getValue(y, y);
        }

        // for each non zero value in the row
        for (int x = 0; x < columns; ++x)
        {
            // divide by the pivot value -> pivot becomes 1
            auto prevValue = leftMatrix.getValue(x, y);
            leftMatrix[x][y] = prevValue / pivotValue;

            // do the same for the right matrix
            prevValue = rightMatrix.getValue(x, y);
            rightMatrix[x][y] = prevValue / pivotValue;
        }

        // subtract from remaining rows
        for (int k = 1; k < rows - y; ++k)
        {
            // get the first value on the next row (how many time to subtract 1 from the next row)
            auto nextRowValue = leftMatrix.getValue(y, y + k);
            // go through columns
            for (int j = 0; j < columns; ++j)
            {
                auto upperRowValue = leftMatrix.getValue(j, y);
                auto lowerRowValue = leftMatrix.getValue(j, y + k);
                // subtract the upper value from the lower nextRowValue-times
                leftMatrix[j][y + k] = lowerRowValue - upperRowValue * nextRowValue;

                // the same for right matrix
                upperRowValue = rightMatrix.getValue(j, y);
                lowerRowValue = rightMatrix.getValue(j, y + k);
                rightMatrix[j][y + k] = lowerRowValue - upperRowValue * nextRowValue;
            }
        }
    }

    // now the opposite side of the diagonal
    // for each remaining row
    for (int y = 1; y < rows; ++y)
    {
        auto pivotValue = leftMatrix.getValue(y, y);
        // going from current row upwards
        for (int k = 1; k < y + 1; ++k)
        {
            auto prevValue = leftMatrix.getValue(y, y - k);
            auto timesToSubtract = pivotValue * prevValue;
            for (int x = 0; x < columns; ++x)
            {
                prevValue = leftMatrix.getValue(x, y - k);
                leftMatrix[x][y - k] = prevValue - timesToSubtract * leftMatrix.getValue(x, y);

                // same for the right matrix
                prevValue = rightMatrix.getValue(x, y - k);
                rightMatrix[x][y - k] = prevValue - timesToSubtract * rightMatrix.getValue(x, y);
            }
        }
    }

    *this = rightMatrix;
}

template <typename T>
std::string Matrix3x3<T>::toString() const
{
    std::stringstream s;
    for (int y = 0; y < rows; ++y)
    {
        for (int x = 0; x < columns; ++x)
        {
            s << data[x * rows + y] << " ";
        }
        s << std::endl;
    }
    return s.str();
}

template <typename T>
Matrix3x3<T> Matrix3x3<T>::operator+(const Matrix3x3& rhs)
{
    Matrix3x3 mat;
    for (int i = 0; i < dataLength; ++i)
        mat.data[i] = data[i] + rhs.data[i];

    return mat;
}

template <typename T>
Matrix3x3<T> Matrix3x3<T>::operator-(const Matrix3x3& rhs)
{
    Matrix3x3 mat;
    for (int i = 0; i < dataLength; ++i)
        mat.data[i] = data[i] - rhs.data[i];

    return mat;
}

template <typename T>
Matrix3x3<T> Matrix3x3<T>::operator*(const Matrix3x3& rhs)
{
    Matrix3x3 newMatrix;
    for (int i = 0; i < rows; ++i)
    {
        for (int j = 0; j < columns; ++j)
        {
            T value = 0;
            for (int k = 0; k < rows; ++k)
            {
                value += getValue(k, i) * rhs.getValue(j, k);
            }
            newMatrix[j][i] = value;
        }
    }
    return newMatrix;
}

template <typename T>
Matrix3x3<T> Matrix3x3<T>::operator*(const T& rhs)
{
    Matrix3x3 newMatrix;
    for (int i = 0; i < dataLength; ++i) newMatrix.data[i] = data[i] * rhs;
    return newMatrix;
}

template <typename T>
Matrix3x3<T> Matrix3x3<T>::operator/(const T& rhs)
{
    Matrix3x3 newMatrix;
    for (int i = 0; i < dataLength; ++i) newMatrix.data[i] = data[i] / rhs;
    return newMatrix;
}

template <typename T>
Matrix3x3<T> operator*(const T& lhs, const Matrix3x3<T>& rhs)
{
    Matrix3x3<T> newMatrix;
    for (int i = 0; i < dataLenght; ++i) newMatrix.data[i] = lhs * rhs.data[i];
    return newMatrix;
}

template <typename T>
Matrix3x3<T>
operator/(const T& lhs, const Matrix3x3<T>& rhs)
{
    Matrix3x3<T> newMatrix;
    for (int i = 0; i < dataLenght; ++i) newMatrix.data[i] = lhs / rhs.data[i];
    return newMatrix;
}

template <typename T>
Matrix3x3<T> Matrix3x3<T>::identity()
{
    Matrix3x3 identityMatrix;
    identityMatrix.data[0] = 1.0;
    identityMatrix.data[1] = 0.0;
    identityMatrix.data[2] = 0.0;
    identityMatrix.data[3] = 0.0;
    identityMatrix.data[4] = 1.0;
    identityMatrix.data[5] = 0.0;
    identityMatrix.data[6] = 0.0;
    identityMatrix.data[7] = 0.0;
    identityMatrix.data[8] = 1.0;
    return identityMatrix;
}

using Matrix4x4f = Matrix4x4<float>;
using Matrix4x4d = Matrix4x4<double>;

using Matrix3x3f = Matrix3x3<float>;
using Matrix3x3d = Matrix3x3<double>;