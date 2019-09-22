#ifndef MATRIX
#define MATRIX

#include <map>
#include <iostream>

#define DEBUG false
#define TRACE(value) \
    if (DEBUG)       \
    std::cout << value << std::endl
#define UNUSED(value) (void)value

namespace homework
{
template <class T, T DEFAULT_VALUE>
class Matrix
{

public:
    class MatrixValueWraper;
    class MatrixIterator;

/**
 * Класс для представления столбцов матрицы. Внутри каждого столбца будет map со строками. 
 * Map применяется для того что бы хранить элементы матрицы.
 */
    class MatrixColumn
    {
        friend class MatrixValueWraper;

    public:
        MatrixColumn() : _index(0) {};
        MatrixColumn(size_t index) : _index(index) {};
        /**
         * Оператор индексатора возвращает обертку MatrixValueWraper. Обертка нужна для того, чтобы
         * при операциях получения данных не создавать новых элементах в контейнере map. А при операциях
         * присваивания - создавать новые элементы.
         */
        MatrixValueWraper operator[](size_t index) { return MatrixValueWraper(this, index); }
        /**
         * Получаем количество заполненых элементов в колонке.
         */
        size_t size() { return _values.size(); }
        /**
         * Индекс колонки в матрице.
         */
        size_t get_index() { return _index;}

    private:
        friend MatrixIterator;
        size_t _index;
        std::map<size_t, T> _values;
    };

/**
 * Класс- обертка для представления значения ячейки матрицы.
 */
    class MatrixValueWraper
    {
    public:
        /**
         * В конструкторе мы сохраняем значение индекса ячейки и вычисляем текущее значение.
         * Хранить все это нужно, что бы была возможность превести обертку к tuple<int&,int&,T>
         * иначе бы можно было вычислять "на лету".
         */
        MatrixValueWraper(MatrixColumn *matrix_column, size_t row) : _matrix_column(matrix_column), _row(row)
        {
            _col   = _matrix_column->get_index();
            auto element = _matrix_column->_values.find(_row);
            if (element != _matrix_column->_values.end()){
                _value = element->second;
            }
            else{
                _value = DEFAULT_VALUE;
            }
        };

        /**
         * Сравниваем Wrapper со значением T. При сравнении не сохраняется нового элемента в матрицу.
         */
        bool operator==(const T &other)
        {
            auto element = _matrix_column->_values.find(_row);
            if (element != _matrix_column->_values.end())
            {
                return (element->second == other);
            }
            else
            {
                return (DEFAULT_VALUE == other);
            }
        }

        /**
         * Сохраняем в Wrapper новое значение типа T. При этом либо перезаписывается значение в map или
         * создается новое.
         */
        MatrixValueWraper &operator=(const T &other)
        {
            auto element = _matrix_column->_values.find(_row);
            if (element != _matrix_column->_values.end())
            {
                element->second = other;
            }
            else
            {
                _matrix_column->_values.insert({_row, other});
            }
            return *this;
        }

        /**
         * Злосчастный оператор преобразования к типу tuple. В нем возвращается индекс (столбец, строка, значение)
         */
        operator std::tuple<int&, int&, T&>(){
            return std::tuple<int&, int&, T&>(_col,_row,_value);
        }

        /**
         * Получаем текущее значение обернутого элемента.
         */
        operator T()
        {
            return _value;
        }

    private:
        MatrixColumn *_matrix_column;
        int _col;
        int _row;
        T _value;
    };

    /**
     * Метод для получения колонки по ее индексу.
     */
    MatrixColumn &operator[](size_t index)
    {
        auto element = _columns.find(index);
        if (element != _columns.end())
        {
            return element->second;
        }
        else
        {
            _columns[index] = MatrixColumn(index);
            return _columns[index];
        }
    }

    /**
     * Количество заполненых элементов в матрице. 
     */
    size_t size()
    {
        size_t result{0};
        for (auto i : _columns)
            result += i.second.size();
        TRACE(result);

        return result;
    }

    /**
    * Итератор по элементам матрицы.
    */
    class MatrixIterator
    {
    public:

        /**
         * Конструктор итератора. 
         * Может быть ситуация когда у нас етсь пустые столбцы. Т.е. столбец есть, а значений в нем нет.
         * Поэтому тут у нас не очень красивый код, который перескакивает через такие столбцы.
         */
        MatrixIterator(const Matrix *matrix, bool end) : _end(end), _matrix_ptr(matrix)
        {
            if (!end)
            {
                _column = _matrix_ptr->_columns.begin();
                if (_column != _matrix_ptr->_columns.end())
                {
                    while (((_row = _column->second._values.begin()) == _column->second._values.end()) &&
                           (_column != _matrix_ptr->_columns.end())){
                        ++_column;
                    }
                    if (_column != _matrix_ptr->_columns.end()){}
                    else _end = true;
                    
                }
                else
                {
                    _end = true;
                }
            }
        }

        /**
         * Получаем значение обертки над текущим элементом.
         */
        MatrixValueWraper operator*()
        {
            MatrixColumn *ptr_column =  (MatrixColumn *)&(_column->second);
            return MatrixValueWraper(ptr_column, _row->first);
        }

        /**
         * Сдвиг на следующий элемент. Тут опять некрасивый код с перепрыгиванием через пустые столбцы.
         * Надо бы с ним что-то сделать.
         */
        MatrixIterator &operator++()
        {
            if (_end)
                return *this;
            ++_row;
            if (_row != _column->second._values.end())
            {
            }
            else
            {
                ++_column;
                if (_column != _matrix_ptr->_columns.end())
                {
                    while (((_row = _column->second._values.begin()) == _column->second._values.end()) &&
                           (_column != _matrix_ptr->_columns.end()))
                        ++_column;
                    if (_column == _matrix_ptr->_columns.end())
                    {
                        _end = true;
                    }
                }
                else
                    _end = true;
            }

            return *this;
        }

        /**
         * Сравниваем итераторы по всем элементам.
         */
        bool operator!=(const MatrixIterator &other)
        {
            if (_matrix_ptr != other._matrix_ptr)
                return true;
            if (_end != other._end)
                return true;
            if ((_end == true) && (other._end == true))
                return false;
            {
                if (_column != other._column)
                    return true;
                if (_row != other._row)
                    return true;
            }
            return false;
        }

    private:
        bool _end;
        const Matrix *const _matrix_ptr;
        typename std::map<size_t, MatrixColumn>::const_iterator _column;
        typename std::map<size_t, T>::const_iterator _row;
    };

    /**
     * Получаем итератор на начало матрицы.
     */
    MatrixIterator begin()
    {
        return MatrixIterator(this, false);
    }
    /**
     * Получаем итератор на конец матрицы.
     */
    MatrixIterator end()
    {
        return MatrixIterator(this, true);
    }

private:
    friend class MatrixIterator;
    std::map<size_t, MatrixColumn> _columns;
};

} // namespace homework
#endif