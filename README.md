# Курсовой проект

## Формулировка задачи

```
Вариант №21: МКЭ для двумерной краевой задачи для эллиптического уравнения в декартовой
системе координат. Базисные функции линейные на треугольниках. Краевые условия всех типов.
Коэффициент диффузии разложить по линейным базисным функциям. Матрицу СЛАУ генерировать в
разреженном строчном формате. Для решения СЛАУ использовать метод сопряженных градиентов
(МСГ) или локально-оптимальную схему (ЛОС) с неполной факторизацией.
```

## Входные данные

`params:`
```
1. Количетсво узлов
2. Количество елементов
3. Количество областей
4. Количество краевых условий
```

`nodes` - _хранит координаты узлов_

`elems` - _хранит количество конечных элементов_

`areas` - _хранит gamma и betta коэффициенты в количетсве (params:3) строк и индекс области елемента в количество (params:2) положительных значений_

`bords` - _хранит (N) строк, каждая из которых состоит из 5 положительных чисел - номер области, пару чисел границы, номер условия, номер типа_

## Способы использования LOS

```c++

cxxtimer::Timer timer(true);
FEM fem(_input);

fem.printAll();         /// print input FEM data
fem.printSparse();      /// print input FEM data (sparse format)

                    ///~~~~~~~~~~~~~~~~///
                    /// ... method ... ///
                    ///~~~~~~~~~~~~~~~~///

timer.stop();
std::cout << "Milliseconds: " << timer.count<milliseconds>();

```

`Способ №1`

```c++
fem.writeFile(_output, 1E-14, 10000); /// overwriting files
LOS<double> los(_output);             /// here is reading
los.solve(Cond::DIAGONAL, true);
```

В данном способе происходит запись глобальной матрицы в разреженном виде в файлы, после чего считывание этих файлов и решение методом LOS

`Способ №2`

```c++
LOS<double> los (
    fem.takeDate(),     /// data
    fem.getNodes(),     /// count nodes
    1E-16, 1000 );      /// epsilon and max iteration
los.solve(Cond::HOLLESKY, true);
```

В данном способе все данные будут отданы классу `LOS`, это сделано для быстродействия программы