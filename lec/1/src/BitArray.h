#include <cstddef>
#include <cstdint>
#include <initializer_list>
#include <string>
#include <vector>

#define private public

class BitArray {
  public:
    BitArray();
    ~BitArray();

    //Конструирует массив, хранящий заданное количество бит.
    //Первые sizeof(long) бит можно инициализровать с помощью параметра value.
    explicit BitArray(std::size_t length, std::uint64_t value = 0);
    BitArray(const std::initializer_list<bool> list);
    BitArray(const std::vector<bool> &vec);

    BitArray(const BitArray &b);
    BitArray(BitArray &&b);

    //Обменивает значения двух битовых массивов.
    void swap(BitArray &b);

    BitArray &operator=(const BitArray &b);
    BitArray &operator=(BitArray &&b);

    //Изменяет размер массива. В случае расширения, новые элементы
    //инициализируются значением value.
    void resize(std::size_t length, bool value = false);
    //Очищает массив.
    void clear();
    //Добавляет новый бит в конец массива. В случае необходимости
    //происходит перераспределение памяти.
    void push_back(bool bit);

    //Битовые операции над массивами.
    //Работают только на массивах одинакового размера.
    //Обоснование реакции на параметр неверного размера входит в задачу.
    BitArray &operator&=(const BitArray &b);
    BitArray &operator|=(const BitArray &b);
    BitArray &operator^=(const BitArray &b);

    //Битовый сдвиг с заполнением нулями.
    BitArray &operator<<=(int n);
    BitArray &operator>>=(int n);
    BitArray operator<<(int n) const;
    BitArray operator>>(int n) const;

    //Заполняет массив истиной.
    BitArray &set();

    //Устанавливает бит с индексом n в значение false.
    BitArray &reset(std::size_t n);

    //Заполняет массив ложью.
    BitArray &reset();

    // true, если массив содержит истинный бит.
    bool any() const;
    // true, если все биты массива ложны.
    bool none() const;
    //Битовая инверсия
    BitArray operator~() const;
    //Подсчитывает количество единичных бит.
    std::size_t count() const;

    std::size_t size() const;
    bool empty() const;

    //Возвращает строковое представление массива.
    std::string to_string() const;

    class Reference {
      public:
        Reference(BitArray &array, std::size_t index);
        bool operator&=(const Reference &other);
        bool operator=(bool value);
        operator bool() const;

      private:
        BitArray &array;
        std::size_t index;
    };

    //Возвращает значение бита по индексу i.
    BitArray::Reference operator[](std::size_t i);

    //Возвращает значение бита по индексу i.
    bool operator[](std::size_t i) const;

    //Класс для безопасного доступа к индексу
    class SafeReference {
      public:
        SafeReference(BitArray &array, std::size_t index);
        bool operator&=(const Reference &other);
        bool operator=(bool value);
        operator bool() const;

      private:
        BitArray &array;
        std::size_t index;
    };

    // Безопасный доступ к индексу
    BitArray::SafeReference at(std::size_t i);

    // Безопасный доступ к индексу (const)
    bool at(std::size_t i) const;

    friend std::ostream &operator<<(std::ostream &stream, const BitArray &arr);

  private:
    std::uint64_t *data;
    std::size_t len;
    std::size_t cap;

    void set_cap(std::size_t cap);
    void grow_at_least(std::size_t least_cap);

    void set_single(std::size_t n, bool val);
    bool get_single(std::size_t n) const;
    void set_range(std::size_t offset, std::size_t len, bool value);

    BitArray(std::size_t cap, std::size_t length, bool fill);

    // upper bound of length in chunks
    static std::size_t chunks(std::size_t n) {
        return (n + 63) / 64;
    };

    friend bool operator==(const BitArray &a, const BitArray &b);
};

bool operator==(const BitArray &a, const BitArray &b);
bool operator!=(const BitArray &a, const BitArray &b);

BitArray operator&(const BitArray &b1, const BitArray &b2);
BitArray operator|(const BitArray &b1, const BitArray &b2);
BitArray operator^(const BitArray &b1, const BitArray &b2);
