#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <stdexcept>
#include <type_traits>
#include <concepts>
#include <utility>
#include <memory>

using namespace std;

// ============================================================================
// КОНЦЕПТЫ (Ограничения типов из файла graph_matrix_adjacency.cpp)
// ============================================================================
template <typename T>
concept TYPE = (std::is_integral_v<T> &&
                !std::is_same_v<T, char> &&
                !std::is_same_v<T, char16_t> &&
                !std::is_same_v<T, char32_t> &&
                !std::is_same_v<T, wchar_t>) ||
               std::is_floating_point_v<T>;

template <typename T>
// requires TYPE<T>
class BaseMatrix
{
public:
    virtual ~BaseMatrix() {}
    virtual void print() const = 0;
    virtual T getValue(const size_t indexColm, const size_t indexLine) const = 0;
    virtual void setValue(const T value, const size_t indexColm, const size_t indexLine) = 0;
    virtual size_t getSize() const = 0;
    virtual void resize(size_t newSize) = 0;
    virtual std::unique_ptr<BaseMatrix<T>> clone() const = 0;
};

// ДВУМЕРНАЯ ДИНАМИЧЕСКАЯ МАТРИЦА (Matrix)
template <typename T>
// requires TYPE<T>
class Matrix final : public BaseMatrix<T>
{
    T **matrix__ = nullptr;
    size_t size__ = 0;

    void swap(Matrix &other) noexcept
    {
        std::swap(matrix__, other.matrix__);
        std::swap(size__, other.size__);
    }

public:
    Matrix() = default;

    Matrix(size_t size) : size__(size)
    {
        if (size__ > 0)
        {
            matrix__ = new T *[size__];
            for (size_t i = 0; i < size__; i++)
            {
                matrix__[i] = new T[size__]();
            }
        }
    }

    Matrix(const Matrix &node) : size__(node.size__)
    {
        if (size__ > 0)
        {
            matrix__ = new T *[size__];
            for (size_t i = 0; i < size__; i++)
            {
                matrix__[i] = new T[size__];
                for (size_t j = 0; j < size__; j++)
                {
                    matrix__[i][j] = node.matrix__[i][j];
                }
            }
        }
    }

    Matrix(Matrix &&node) noexcept
    {
        node.swap(*this);
    }

    Matrix &operator=(const Matrix &node)
    {
        if (&node != this)
        {
            Matrix temp(node);
            temp.swap(*this);
        }
        return *this;
    }

    Matrix &operator=(Matrix &&node) noexcept
    {
        if (&node != this)
        {
            Matrix temp(std::move(node));
            temp.swap(*this);
        }
        return *this;
    }

    ~Matrix()
    {
        if (matrix__ != nullptr)
        {
            for (size_t i = 0; i < size__; i++)
            {
                delete[] matrix__[i];
            }
            delete[] matrix__;
        }
    }

    T getValue(const size_t indexColm, const size_t indexLine) const override
    {
        if (indexColm < size__ && indexLine < size__)
            return matrix__[indexColm][indexLine];
        else
            throw std::invalid_argument("invalid matrix index");
    }

    void setValue(const T value, const size_t indexColm, const size_t indexLine) override
    {
        if (indexColm < size__ && indexLine < size__)
            matrix__[indexColm][indexLine] = value;
        else
            throw std::invalid_argument("invalid matrix index");
    }

    size_t getSize() const override { return size__; }

    void print() const override
    {
        for (size_t i = 0; i < size__; i++)
        {
            for (size_t j = 0; j < size__; j++)
            {
                std::cout << matrix__[i][j] << "  ";
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }

    void resize(size_t newSize) override
    {
        Matrix<T> newMatr(newSize);
        size_t minSize = std::min(size__, newSize);
        for (size_t i = 0; i < minSize; ++i)
        {
            for (size_t j = 0; j < minSize; ++j)
            {
                newMatr.setValue(this->matrix__[i][j], i, j);
            }
        }
        *this = std::move(newMatr);
    }

    std::unique_ptr<BaseMatrix<T>> clone() const override
    {
        return std::make_unique<Matrix<T>>(*this);
    }
};

// ОДНОМЕРНАЯ ДИНАМИЧЕСКАЯ МАТРИЦА (Matrix1D)
template <typename T>
// requires TYPE<T>
class Matrix1D final : public BaseMatrix<T>
{
    T *matrix__ = nullptr;
    size_t size__ = 0;

    void swap(Matrix1D &other) noexcept
    {
        std::swap(matrix__, other.matrix__);
        std::swap(size__, other.size__);
    }

public:
    Matrix1D() = default;

    Matrix1D(size_t size) : size__(size)
    {
        if (size__ > 0)
        {
            matrix__ = new T[size__ * size__]();
        }
    }

    Matrix1D(const Matrix1D &node) : size__(node.size__)
    {
        if (size__ > 0)
        {
            matrix__ = new T[size__ * size__];
            for (size_t i = 0; i < size__ * size__; ++i)
            {
                matrix__[i] = node.matrix__[i];
            }
        }
    }

    Matrix1D(Matrix1D &&node) noexcept
    {
        node.swap(*this);
    }

    Matrix1D &operator=(const Matrix1D &node)
    {
        if (&node != this)
        {
            Matrix1D temp(node);
            temp.swap(*this);
        }
        return *this;
    }

    Matrix1D &operator=(Matrix1D &&node) noexcept
    {
        if (&node != this)
        {
            Matrix1D temp(std::move(node));
            temp.swap(*this);
        }
        return *this;
    }

    ~Matrix1D()
    {
        delete[] matrix__;
    }

    T getValue(const size_t indexColm, const size_t indexLine) const override
    {
        if (indexColm < size__ && indexLine < size__)
            return matrix__[indexColm * size__ + indexLine];
        else
            throw std::invalid_argument("invalid matrix index");
    }

    void setValue(const T value, const size_t indexColm, const size_t indexLine) override
    {
        if (indexColm < size__ && indexLine < size__)
            matrix__[indexColm * size__ + indexLine] = value;
        else
            throw std::invalid_argument("invalid matrix index");
    }

    size_t getSize() const override { return size__; }

    void print() const override
    {
        for (size_t i = 0; i < size__; i++)
        {
            for (size_t j = 0; j < size__; j++)
            {
                std::cout << matrix__[i * size__ + j] << "  ";
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }

    void resize(size_t newSize) override
    {
        Matrix1D<T> newMatrix(newSize);
        size_t minSize = std::min(size__, newSize);
        for (size_t i = 0; i < minSize; ++i)
        {
            for (size_t j = 0; j < minSize; ++j)
            {
                newMatrix.setValue(this->getValue(i, j), i, j);
            }
        }
        *this = std::move(newMatrix);
    }

    std::unique_ptr<BaseMatrix<T>> clone() const override
    {
        return std::make_unique<Matrix1D<T>>(*this);
    }
};
