#ifndef MATRIX
#define MATRIX

#include <map>
#include <iostream>
#include <exception>

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
    friend class MatrixValueWraper;
/**
 * Класс для представления столбцов матрицы. Внутри каждого столбца будет map со строками. 
 * Map применяется для того что бы хранить элементы матрицы.
 */
    class MatrixColumn
    {
    public:
        MatrixColumn(Matrix& matrix, size_t index) : _matrix (matrix),_index(index) {};

        /**
         * Оператор индексатора возвращает обертку MatrixValueWraper. Обертка нужна для того, чтобы
         * при операциях получения данных не создавать новых элементах в контейнере map. А при операциях
         * присваивания - создавать новые элементы.
         */
        MatrixValueWraper operator[](size_t row) { 
            TRACE("MatrixWraper[]");
            return MatrixValueWraper(_matrix, _index, row); }

        /**
         * Индекс колонки в матрице.
         */
        size_t get_index() { return _index;}



    private:
        friend MatrixIterator;
        Matrix& _matrix;
        size_t _index;
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
        MatrixValueWraper(Matrix &matrix, size_t col, size_t row) : _matrix(matrix), _col(col), _row(row),_value(DEFAULT_VALUE){
            auto col_element = _matrix._columns.find(_col);
            if(col_element != _matrix._columns.end()){
                auto row_element = col_element->second.find(_row);
                if( row_element != col_element->second.end()){
                    _value = row_element->second;
                    TRACE("MatrixValueWraper::Construct");
                }
            }
        };

        /**
         * Сравниваем Wrapper со значением T. При сравнении не сохраняется нового элемента в матрицу.
         */
        bool operator==(const T &other)
        {
            TRACE("MatrixValueWrapper==");
            return other==_value;
        }

        /**
         * Сохраняем в Wrapper новое значение типа T. При этом либо перезаписывается значение в map или
         * создается новое.
         */
        MatrixValueWraper &operator=(const T &other)
        {
            TRACE("MatrixValueWraper=");
            _value = other;
            if(_value!=DEFAULT_VALUE){
                _matrix.flush(*this);
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
        friend Matrix;
        Matrix & _matrix;
        int _col;
        int _row;
        T   _value;
    };

    /**
     * Сохраняем колонку в матрице
     */

    void flush(MatrixValueWraper &value){
        TRACE("flush");
        auto column = _columns.find(value._col);
        if (column == _columns.end()){
                _columns[value._col] = std::map<size_t,T>();
            }
        _columns[value._col][value._row] = value._value;
    }
    /**
     * Метод для получения колонки по ее индексу.
     */
    MatrixColumn operator[](size_t index)
    {
      TRACE("Matrix[]");
      return MatrixColumn(*this,index);
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
        MatrixIterator(Matrix &matrix, bool end) : _end(end), _matrix(matrix) {
            if(!_end){
                _column = matrix._columns.begin();
                _row    = _column->second.begin();
            }
        }

        /**
         * Получаем значение обертки над текущим элементом.
         */
        MatrixValueWraper operator*()
        {
             if(!_end){
                 return MatrixValueWraper(_matrix,_column->first,_row->first);
             } else throw std::logic_error("out of range");
        }

        /**
         * Сдвиг на следующий элемент. Тут опять некрасивый код с перепрыгиванием через пустые столбцы.
         * Надо бы с ним что-то сделать.
         */
        MatrixIterator &operator++()
        {
            TRACE("MatrixIterator::start");
            if (_end)
                return *this;

            TRACE("MatrixIterator::work");
            ++_row;
            if (_row == _column->second.end()){
                ++_column;
                if (_column != _matrix._columns.end()){
                    _row    = _column->second.begin();
                }
                else{
                    TRACE("MatrixIterator::end");
                    _end = true;
                }
            }

            return *this;
        }

        /**
         * Сравниваем итераторы по всем элементам.
         */
        bool operator!=(const MatrixIterator &other)
        {
            if(_end==true)
            if(other._end==true) return false;

            if (&_matrix != &other._matrix)
                return true;
            if (_end != other._end)
                return true;

            if (_column != other._column)
                    return true;
            if (_row != other._row)
                    return true;

            return false;
        }

    private:
        bool _end;
        Matrix &_matrix;
        typename std::map<size_t, std::map<size_t, T>>::const_iterator _column;
        typename std::map<size_t, T>::const_iterator _row;
    };

    /**
     * Получаем итератор на начало матрицы.
     */
    MatrixIterator begin()
    {
        return MatrixIterator(*this, false);
    }
    /**
     * Получаем итератор на конец матрицы.
     */
    MatrixIterator end()
    {
        return MatrixIterator(*this, true);
    }

private:
    friend class MatrixIterator;
    std::map<size_t, std::map<size_t,T> > _columns;
};

} // namespace homework
#endif