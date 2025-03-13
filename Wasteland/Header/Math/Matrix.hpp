#pragma once

#include "Math/Vector.hpp"

namespace Wasteland::Math
{
	template <typename Container>
	concept DoubleArrayType = requires(Container a)
	{
		typename Container::value_type;
		requires ArrayType<typename Container::value_type>;

		{ a.begin() } -> std::input_or_output_iterator;
		{ a.end() } -> std::input_or_output_iterator;
	};

	template <Arithmetic T, size_t R, size_t C>
	class Matrix final
	{

	public:

		using value_type = T;

		Matrix() = default;

		Matrix(std::initializer_list<std::initializer_list<T>> input)
		{
			assert(input.size() == R);

			size_t row = 0;

			for (const auto& r : input)
			{
				assert(r.size() == C);

				std::copy(r.begin(), r.end(), data[row].begin());

				++row;
			}
		}

		template <DoubleArrayType U>
		Matrix(const U& input)
		{
			assert(std::distance(input.begin(), input.end()) == R);
			size_t row = 0;

			for (const auto& r : input)
			{
				assert(std::distance(r.begin(), r.end()) == C);

				std::copy(r.begin(), r.end(), data[row].begin());

				++row;
			}
		}

		template <DoubleArrayType U>
		Matrix& operator=(const U& operand)
		{
			assert(std::distance(operand.begin(), operand.end()) == R);

			size_t row = 0;

			for (const auto& r : operand)
			{
				assert(std::distance(r.begin(), r.end()) == C);
				std::copy(r.begin(), r.end(), data[row].begin());

				++row;
			}

			return *this;
		}

		Matrix& operator=(std::initializer_list<std::initializer_list<T>> operand)
		{
			assert(operand.size() == R);

			size_t row = 0;

			for (const auto& r : operand)
			{
				assert(r.size() == C);
				std::copy(r.begin(), r.end(), data[row].begin());

				++row;
			}

			return *this;
		}

		template <DoubleArrayType U>
		Matrix operator+(const U& operand) const
		{
			return ApplyOperation(operand, std::plus<T>());
		}

		Matrix operator+(std::initializer_list<std::initializer_list<T>> operand) const
		{
			return ApplyOperation(operand, std::plus<T>());
		}

		Matrix operator+(const Matrix& operand) const
		{
			return ApplyOperation(operand, std::plus<T>());
		}

		template <DoubleArrayType U>
		Matrix operator-(const U& operand) const
		{
			return ApplyOperation(operand, std::minus<T>());
		}
		Matrix operator-(std::initializer_list<std::initializer_list<T>> operand) const
		{
			return ApplyOperation(operand, std::minus<T>());
		}
		Matrix operator-(const Matrix& operand) const
		{
			return ApplyOperation(operand, std::minus<T>());
		}

		template <size_t K>
		Matrix<T, R, K> operator*(const Matrix<T, C, K>& operand) const
		{
			Matrix<T, R, K> result;
			for (size_t i = 0; i < R; i++)
			{
				for (size_t j = 0; j < K; j++)
				{
					T sum = T(0);

					for (size_t k = 0; k < C; k++)
						sum += data[i][k] * operand[k][j];
					
					result[i][j] = sum;
				}
			}
			return result;
		}

		template <size_t K>
		Matrix<T, R, K> operator*(std::initializer_list<std::initializer_list<T>> operand) const
		{
			Matrix<T, C, K> temp(operand);

			return (*this) * temp;
		}

		template <DoubleArrayType U, size_t K>
		Matrix<T, R, K> operator*(const U& operand) const
		{
			Matrix<T, C, K> temp(operand);

			return (*this) * temp;
		}

		Matrix operator*(T scalar) const
		{
			Matrix result;
			for (size_t i = 0; i < R; i++)
			{
				for (size_t j = 0; j < C; j++)
					result[i][j] = data[i][j] * scalar;
			}

			return result;
		}

		template <DoubleArrayType U>
		Matrix operator/(const U& operand) const
		{
			return ApplyOperation(operand, std::divides<T>());
		}

		Matrix operator/(std::initializer_list<std::initializer_list<T>> operand) const
		{
			return ApplyOperation(operand, std::divides<T>());
		}

		Matrix operator/(const Matrix& operand) const
		{
			return ApplyOperation(operand, std::divides<T>());
		}

		template <DoubleArrayType U>
		Matrix& operator+=(const U& operand)
		{
			return ApplyOperationInPlace(operand, std::plus<T>());
		}

		Matrix& operator+=(std::initializer_list<std::initializer_list<T>> operand)
		{
			return ApplyOperationInPlace(operand, std::plus<T>());
		}

		Matrix& operator+=(const Matrix& operand)
		{
			return ApplyOperationInPlace(operand, std::plus<T>());
		}

		template <DoubleArrayType U>
		Matrix& operator-=(const U& operand)
		{
			return ApplyOperationInPlace(operand, std::minus<T>());
		}

		Matrix& operator-=(std::initializer_list<std::initializer_list<T>> operand)
		{
			return ApplyOperationInPlace(operand, std::minus<T>());
		}

		Matrix& operator-=(const Matrix& operand)
		{
			return ApplyOperationInPlace(operand, std::minus<T>());
		}

		Matrix& operator*=(const Matrix& operand) requires (R == C)
		{
			*this = *this * operand;
			return *this;
		}

		template <DoubleArrayType U>
		Matrix& operator*=(const U& operand) requires (R == C)
		{
			*this = *this * operand;
			return *this;
		}

		Matrix& operator*=(const T& operand) requires (R == C)
		{
			*this = *this * operand;
			return *this;
		}

		template <DoubleArrayType U>
		Matrix& operator/=(const U& operand)
		{
			return ApplyOperationInPlace(operand, std::divides<T>());
		}

		Matrix& operator/=(std::initializer_list<std::initializer_list<T>> operand)
		{
			return ApplyOperationInPlace(operand, std::divides<T>());
		}

		Matrix& operator/=(const Matrix& operand)
		{
			return ApplyOperationInPlace(operand, std::divides<T>());
		}

		std::array<T, C>& operator[](size_t row)
		{
			assert(row < R);

			return data[row];
		}

		const std::array<T, C>& operator[](size_t row) const
		{
			assert(row < R);

			return data[row];
		}

		auto begin()
		{
			return data.begin();
		}

		auto end()
		{
			return data.end();
		}

		auto begin() const
		{
			return data.begin();
		}

		auto end() const
		{
			return data.end();
		}

		constexpr size_t Rows() const
		{
			return R;
		}

		constexpr size_t Columns() const
		{
			return C;
		}

		static Matrix Transpose(const Matrix& input)
		{
			Matrix result;

			for (size_t i = 0; i < R; ++i)
			{
				for (size_t j = 0; j < C; ++j)
					result[j][i] = input.data[i][j];
			}

			return result;
		}

		static Matrix Identity() requires (R == C)
		{
			Matrix result;

			for (size_t i = 0; i < R; ++i)
				result[i][i] = T(1);

			return result;
		}

		static Matrix Normalize(const Matrix& m)
		{
			Matrix result;

			for (size_t i = 0; i < R; ++i)
			{
				T length = std::sqrt(std::inner_product(m[i].begin(), m[i].end(), m[i].begin(), T(0)));

				if (length > 0)
				{
					for (size_t j = 0; j < C; ++j)
						result[i][j] = m[i][j] / length;
				}
			}

			return result;
		}

		static Matrix LookAt(const Vector<T, 3>& eye, const Vector<T, 3>& target, const Vector<T, 3>& up) requires(R == 4 && C == 4)
		{
			Vector<T, 3> zAxis = Vector<T, 3>::Normalize({ target[0] - eye[0], target[1] - eye[1], target[2] - eye[2] });
			Vector<T, 3> xAxis = CrossProduct(up, zAxis);
			Vector<T, 3> yAxis = CrossProduct(zAxis, xAxis);

			return Matrix(
			{
				{ xAxis[0], yAxis[0], zAxis[0], 0 },
				{ xAxis[1], yAxis[1], zAxis[1], 0 },
				{ xAxis[2], yAxis[2], zAxis[2], 0 },
				{ -DotProduct(xAxis, eye), -DotProduct(yAxis, eye), -DotProduct(zAxis, eye), 1 }
			});
		}

		static Matrix Perspective(T fovRadians, T aspect, T nearPlane, T farPlane) requires(R == 4 && C == 4)
		{
			T tanHalfFov = std::tan(fovRadians / 2);

			return Matrix(
			{
				{ 1 / (aspect * tanHalfFov), 0, 0, 0 },
				{ 0, 1 / tanHalfFov, 0, 0 },
				{ 0, 0, farPlane / (farPlane - nearPlane), 1 },
				{ 0, 0, (-nearPlane * farPlane) / (farPlane - nearPlane), 0 }
			});
		}

		static Matrix Orthographic(T left, T right, T bottom, T top, T nearPlane, T farPlane) requires(R == 4 && C == 4)
		{
			return Matrix(
			{
				{ 2 / (right - left), 0, 0, 0 },
				{ 0, 2 / (top - bottom), 0, 0 },
				{ 0, 0, 1 / (farPlane - nearPlane), 0 },
				{ -(right + left) / (right - left), -(top + bottom) / (top - bottom), -nearPlane / (farPlane - nearPlane), 1 }
			});
		}

		static Matrix Scale(const Vector<T, 3>& scale) requires(R == 4 && C == 4)
		{
			return Matrix(
			{
				{ scale.x(), 0,  0,  0},
				{ 0,  scale.y(), 0,  0},
				{ 0,  0,  scale.z(), 0},
				{ 0,  0,  0,  1 }
			});
		}

		static Matrix Translation(const Vector<T, 3>& translation) requires(R == 4 && C == 4)
		{
			return Matrix(
			{
				{ 1, 0, 0, translation.x() },
				{ 0, 1, 0, translation.y() },
				{ 0, 0, 1, translation.z() },
				{ 0, 0, 0, 1 }
			});
		}

		static Matrix RotationX(T angle) requires(R == 4 && C == 4)
		{
			T cosA = std::cos(angle);
			T sinA = std::sin(angle);

			return Matrix(
			{
				{ 1,  0,    0,   0 },
				{ 0, cosA, -sinA, 0 },
				{ 0, sinA,  cosA, 0 },
				{ 0,  0,    0,   1 }
			});
		}

		static Matrix RotationY(T angle) requires(R == 4 && C == 4)
		{
			T cosA = std::cos(angle);
			T sinA = std::sin(angle);

			return Matrix(
			{
				{ cosA,  0, sinA, 0 },
				{ 0,     1, 0,    0 },
				{ -sinA, 0, cosA, 0 },
				{ 0,     0, 0,    1 }
			});
		}

		static Matrix RotationZ(T angle) requires(R == 4 && C == 4)
		{
			T cosA = std::cos(angle);
			T sinA = std::sin(angle);

			return Matrix(
			{
				{ cosA, -sinA, 0, 0 },
				{ sinA, cosA,  0, 0 },
				{ 0,    0,     1, 0 },
				{ 0,    0,     0, 1 }
			});
		}

		static Vector<T, 3> CrossProduct(const Vector<T, 3>& a, const Vector<T, 3>& b)
		{
			return
			{
				a[1] * b[2] - a[2] * b[1],
				a[2] * b[0] - a[0] * b[2],
				a[0] * b[1] - a[1] * b[0]
			};
		}

		static T DotProduct(const Vector<T, 3>& a, const Vector<T, 3>& b)
		{
			return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
		}

	private:

		template <typename Op, typename U>
		Matrix ApplyOperation(const U& operand, Op op) const
		{
			Matrix result;

			for (size_t i = 0; i < R; ++i)
			{
				for (size_t j = 0; j < C; ++j)
					result[i][j] = op(data[i][j], operand[i][j]);
			}

			return result;
		}

		template <typename Op, typename U>
		Matrix& ApplyOperationInPlace(const U& operand, Op op)
		{
			for (size_t i = 0; i < R; ++i)
			{
				for (size_t j = 0; j < C; ++j)
					data[i][j] = op(data[i][j], operand[i][j]);
			}

			return *this;
		}

		std::array<std::array<T, C>, R> data;

	};

	template <Arithmetic T, size_t R, size_t C>
	bool operator==(const Matrix<T, R, C>& lhs, const Matrix<T, R, C>& rhs)
	{
		for (size_t i = 0; i < R; ++i)
		{
			for (size_t j = 0; j < C; ++j)
			{
				if (lhs[i][j] != rhs[i][j])
					return false;
			}
		}

		return true;
	}

	template <Arithmetic T, size_t R, size_t C>
	bool operator!=(const Matrix<T, R, C>& lhs, const Matrix<T, R, C>& rhs)
	{
		return !(lhs == rhs);
	}

	template <Arithmetic T, size_t R, size_t C>
	std::ostream& operator<<(std::ostream& os, const Matrix<T, R, C>& m)
	{
		for (size_t i = 0; i < R; ++i)
		{
			os << '[';

			for (size_t j = 0; j < C; ++j)
			{
				os << m[i][j];
				if (j + 1 < C)
				{
					os << ", ";
				}
			}

			os << ']';

			if (i + 1 < R)
				os << '\n';
		}

		return os;
	}
}

namespace std
{
	inline void HashCombine(std::size_t& seed, std::size_t value) noexcept
	{
		constexpr std::size_t magic = 0x9e3779b97f4a7c15ULL;
		seed ^= value + magic + (seed << 6) + (seed >> 2);
	}

	template <Arithmetic T, size_t R, size_t C>
	struct hash<Wasteland::Math::Matrix<T, R, C>>
	{
		std::size_t operator()(const Matrix<T, R, C>& m) const noexcept
		{
			std::size_t result = 0;

			for (size_t i = 0; i < R; ++i)
			{
				for (size_t j = 0; j < C; ++j)
				{
					const auto valHash = std::hash<T>{}(m[i][j]);
					HashCombine(result, valHash);
				}
			}

			return result;
		}
	};
}

template <Arithmetic T, size_t R, size_t C>
struct std::formatter<Wasteland::Math::Matrix<T, R, C>> : std::formatter<std::string>
{
	auto format(const Wasteland::Math::Matrix<T, R, C>& mat, std::format_context& ctx)
	{
		std::ostringstream oss;

		oss << mat;

		return std::formatter<std::string>::format(oss.str(), ctx);
	}
};