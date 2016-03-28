#include "Window/Window.h"

#include <sstream>

namespace Amber
{

template <typename T>
const Matrix4x4<T> Matrix4x4<T>::Identity = Matrix4x4<T>
(
	1, 0, 0, 0,
	0, 1, 0, 0,
	0, 0, 1, 0,
	0, 0, 0, 1
	);

template <typename T>
Matrix4x4<T>::Matrix4x4()
{
}

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
Matrix4x4<T>::Matrix4x4(T v0, T v1, T v2, T v3, T v4, T v5, T v6, T v7, T v8, T v9, T v10, T v11, T v12, T v13, T v14, T v15)
{
	data[0] = v0;
	data[1] = v1;
	data[2] = v2;
	data[3] = v3;
	data[4] = v4;
	data[5] = v5;
	data[6] = v6;
	data[7] = v7;
	data[8] = v8;
	data[9] = v9;
	data[10] = v10;
	data[11] = v11;
	data[12] = v12;
	data[13] = v13;
	data[14] = v14;
	data[15] = v15;
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
	return &data[column * rowsCount];
}

template <typename T>
const T* Matrix4x4<T>::operator[](int column) const
{
	return &data[column * rowsCount];
}

template <typename T>
T Matrix4x4<T>::getValue(int column, int row) const
{
	return data[column * rowsCount + row];
}

template <typename T>
void Matrix4x4<T>::setValue(int column, int row, T value)
{
	data[column * rowsCount + row] = value;
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
	for (int x = 0; x < columnsCount; ++x)
	{
		for (int y = 0; y < rowsCount; ++y)
		{
			setValue(y, x, copy.getValue(x, y));
		}
	}
}

template <typename T>
T Matrix4x4<T>::determinant()
{
	T result = 0;
	for (int y = 0; y < rowsCount; ++y)
	{
		T firstColumnValue = getValue(0, y);
		Matrix3x3<T> smallMatrix;
		int smallMatrixY = 0;
		for (int i = 0; i < rowsCount; ++i)
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
	Matrix4x4f rightMatrix = Matrix4x4::Identity;

	for (int y = 0; y < rowsCount; ++y)
	{
		// Get the first non-zero value in the row
		auto pivotValue = leftMatrix.getValue(y, y);
		// when pivot is 0 -> need to swap rows
		if (pivotValue == 0)
		{
			// no more rows to swap -> matrix is singular
			if (y == rowsCount - 1)
			{
				criticalError("ERROR: Matrix4x4::inverse - attempt to inverse singular matrix.");
			}

			int swapWithRowIndex = y + 1;
			T valueOnNextLine = leftMatrix.getValue(y, y + 1);

			if (valueOnNextLine == 0 && y == 0)
			{
				for (int i = y; y < rowsCount; ++y)
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
			for (int x = 0; x < columnsCount; ++x)
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
		for (int x = 0; x < columnsCount; ++x)
		{
			// divide by the pivot value -> pivot becomes 1
			auto prevValue = leftMatrix.getValue(x, y);
			leftMatrix[x][y] = prevValue / pivotValue;

			// do the same for the right matrix
			prevValue = rightMatrix.getValue(x, y);
			rightMatrix[x][y] = prevValue / pivotValue;
		}

		// subtract from remaining rows
		for (int k = 1; k < rowsCount - y; ++k)
		{
			// get the first value on the next row (how many time to subtract 1 from the next row)
			auto nextRowValue = leftMatrix.getValue(y, y + k);
			// go through columns
			for (int j = 0; j < columnsCount; ++j)
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
	for (int y = 1; y < rowsCount; ++y)
	{
		auto pivotValue = leftMatrix.getValue(y, y);
		// going from current row upwards
		for (int k = 1; k < y + 1; ++k)
		{
			auto prevValue = leftMatrix.getValue(y, y - k);
			auto timesToSubtract = pivotValue * prevValue;
			for (int x = 0; x < columnsCount; ++x)
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
	for (int y = 0; y < rowsCount; ++y)
	{
		for (int x = 0; x < columnsCount; ++x)
		{
			s << data[x * rowsCount + y] << " ";
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
	for (int i = 0; i < rowsCount; ++i)
	{
		for (int j = 0; j < columnsCount; ++j)
		{
			T value = 0.0;
			for (int k = 0; k < rowsCount; ++k)
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
Matrix4x4<T> operator*(const Matrix4x4<T>& lhs, const Matrix4x4<T>& rhs)
{
	Matrix4x4<T> newMatrix;
	for (int i = 0; i < lhs.rowsCount; ++i)
	{
		for (int j = 0; j < lhs.columnsCount; ++j)
		{
			T value = 0.0;
			for (int k = 0; k < lhs.rowsCount; ++k)
			{
				value += lhs.getValue(k, i) * rhs.getValue(j, k);
			}
			newMatrix[j][i] = value;
		}
	}
	return newMatrix;
}

template <typename T>
Vector3<T> operator*(const Matrix4x4<T>& lhs, const Vector3<T>& rhs)
{
	Vector3<T> tmp;
	tmp.x = rhs.x * lhs.getValue(0, 0) + rhs.y * lhs.getValue(1, 0) + rhs.z * lhs.getValue(2, 0) + lhs.getValue(3, 0);
	tmp.y = rhs.x * lhs.getValue(0, 1) + rhs.y * lhs.getValue(1, 1) + rhs.z * lhs.getValue(2, 1) + lhs.getValue(3, 1);
	tmp.z = rhs.x * lhs.getValue(0, 2) + rhs.y * lhs.getValue(1, 2) + rhs.z * lhs.getValue(2, 2) + lhs.getValue(3, 2);
	return tmp;
}

template <typename T>
Matrix4x4<T> operator*(const T& lhs, const Matrix4x4<T>& rhs)
{
	Matrix4x4<T> newMatrix;
	for (int i = 0; i < newMatrix.dataLength; ++i) newMatrix.data[i] = lhs * rhs.data[i];
	return newMatrix;
}

template <typename T>
Matrix4x4<T> operator/(const T& lhs, const Matrix4x4<T>& rhs)
{
	Matrix4x4<T> newMatrix;
	for (int i = 0; i < newMatrix.dataLength; ++i) newMatrix.data[i] = lhs / rhs.data[i];
	return newMatrix;
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
	for (int x = 0; x < result.columnsCount; ++x)
	{
		for (int y = 0; y < result.rowsCount; ++y)
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
const Matrix3x3<T> Matrix3x3<T>::Identity = Matrix3x3<T>
(
	1, 0, 0,
	0, 1, 0,
	0, 0, 1
	);

template <typename T>
Matrix3x3<T>::Matrix3x3(T v0, T v1, T v2, T v3, T v4, T v5, T v6, T v7, T v8)
{
	data[0] = v0;
	data[1] = v1;
	data[2] = v2;
	data[3] = v3;
	data[4] = v4;
	data[5] = v5;
	data[6] = v6;
	data[7] = v7;
	data[8] = v8;
}

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
	return &data[column * rowsCount];
}

template <typename T>
T Matrix3x3<T>::getValue(int column, int row) const
{
	return data[column * rowsCount + row];
}

template <typename T>
void Matrix3x3<T>::setValue(int column, int row, T value)
{
	data[column * rowsCount + row] = value;
}

template <typename T>
void Matrix3x3<T>::init(T value = 0)
{
	for (int i = 0; i < dataLength; ++i)
		data[i] = value;
}

template <typename T>
void Matrix3x3<T>::fillDiag(T value)
{
	data[0] = value;
	data[4] = value;
	data[8] = value;
}

template <typename T>
void Matrix3x3<T>::transpose()
{
	auto copy = *this;
	for (int x = 0; x < columnsCount; ++x)
	{
		for (int y = 0; y < rowsCount; ++y)
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
	Matrix3x3 rightMatrix = Matrix3x3::Identity;

	for (int y = 0; y < rowsCount; ++y)
	{
		// Get the first non-zero value in the row
		auto pivotValue = leftMatrix.getValue(y, y);
		// when pivot is 0 -> need to swap rows
		if (pivotValue == 0)
		{
			// no more rows to swap -> matrix is singular
			if (y == rowsCount - 1)
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
			for (int x = 0; x < columnsCount; ++x)
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
		for (int x = 0; x < columnsCount; ++x)
		{
			// divide by the pivot value -> pivot becomes 1
			auto prevValue = leftMatrix.getValue(x, y);
			leftMatrix[x][y] = prevValue / pivotValue;

			// do the same for the right matrix
			prevValue = rightMatrix.getValue(x, y);
			rightMatrix[x][y] = prevValue / pivotValue;
		}

		// subtract from remaining rows
		for (int k = 1; k < rowsCount - y; ++k)
		{
			// get the first value on the next row (how many time to subtract 1 from the next row)
			auto nextRowValue = leftMatrix.getValue(y, y + k);
			// go through columns
			for (int j = 0; j < columnsCount; ++j)
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
	for (int y = 1; y < rowsCount; ++y)
	{
		auto pivotValue = leftMatrix.getValue(y, y);
		// going from current row upwards
		for (int k = 1; k < y + 1; ++k)
		{
			auto prevValue = leftMatrix.getValue(y, y - k);
			auto timesToSubtract = pivotValue * prevValue;
			for (int x = 0; x < columnsCount; ++x)
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
	for (int y = 0; y < rowsCount; ++y)
	{
		for (int x = 0; x < columnsCount; ++x)
		{
			s << data[x * rowsCount + y] << " ";
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
	for (int i = 0; i < rowsCount; ++i)
	{
		for (int j = 0; j < columnsCount; ++j)
		{
			T value = 0;
			for (int k = 0; k < rowsCount; ++k)
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
	for (int i = 0; i < newMatrix.dataLength; ++i) newMatrix.data[i] = lhs * rhs.data[i];
	return newMatrix;
}

template <typename T>
Matrix3x3<T>
operator/(const T& lhs, const Matrix3x3<T>& rhs)
{
	Matrix3x3<T> newMatrix;
	for (int i = 0; i < newMatrix.dataLength; ++i) newMatrix.data[i] = lhs / rhs.data[i];
	return newMatrix;
}

}