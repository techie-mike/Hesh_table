//
// Created by texnar on 04/11/2019.
//

#include "HeshList.h"

template <typename T>
List<T>::List() {
    head_ = 0;
    tail_ = 0;
    size_ = 0;
    length_ = 1000;
    size_calloc = 0;

    point_from_calloc[size_calloc++] = (T*) calloc (length_, sizeof(struct element<T>));

    fillingFreePoints(length_);

    free_ = (T*)&(((element<T>*)point_from_calloc[0])[1]);

    ((element<T>*)point_from_calloc[0])[length_ - 1].next_ = (T*)&(((element<T>*)point_from_calloc[0])[0]);
    ((element<T>*)point_from_calloc[0])[0].next_ = (T*)&(((element<T>*)point_from_calloc[0])[0]);
}

template <typename T>
List<T>::~List()
{
    for (list_t i = 0; i < size_calloc; i++) {
        free(point_from_calloc[i]);
    }
}

template <typename T>
void List<T>::autoIncreaseLength(list_t MULTIPLICATION_FACTOR)
{
    if (size_ + 1 >= length_ - 1) {
        list_t last_length = length_;
        length_ *= MULTIPLICATION_FACTOR;

        point_from_calloc[size_calloc++] = (T*) calloc (last_length, sizeof(struct element<T>));

        fillingFreePoints(last_length);

        ((element<T> *) point_from_calloc[size_calloc - 1])[last_length - 1].next_ =
                (T *) &(((element<T> *) point_from_calloc[0])[0]);

        ((element<T> *) free_)->next_  =
                (T *) &(((element<T> *) point_from_calloc[size_calloc - 1])[0]);
    }
}

template <typename T>
void List<T>::fillingFreePoints(list_t last_length)
{

    if (point_from_calloc[size_calloc - 1] == nullptr) {
        printf("Error, in calloc\n");
        abort();
    }

    for (list_t i = 0; i < last_length; i++){
        (((element<T>*)point_from_calloc[size_calloc - 1])[i]).data_ = 0;

        (((element<T>*)point_from_calloc[size_calloc - 1])[i]).next_ = (T*)&(((element<T>*)point_from_calloc[size_calloc - 1])[i + 1]);
    }
}

template <typename T>
T* List<T>::insertInEnd (T value) {
    autoIncreaseLength();
    assert(free_);

    T* new_point = free_;
    free_ = ((element<T>*)free_)->next_;

    if (size_ == 0) {
        head_ = new_point;
        tail_ = new_point;
        ((element<T>*)new_point)->data_ = value;
    }
    else if (size_ > 0) {
        ((element<T>*)tail_)->next_ = new_point;
        tail_ = new_point;
        ((element<T>*)new_point)->data_ = value;
    }

    size_++;
}

template <typename T>
T* List<T>::searchByValue(T value) {
    T* temp_point = head_;
    for (list_t i = 0; i < size_; i++){
        if (((element<T>*)temp_point)->data_ == value){
            return temp_point;
        }
        temp_point = ((element<T>*)temp_point)->next_;
    }
    return nullptr;
}


template <typename T>
void HeshingWords (char *text, long length, List<T> a[], int size_array_list, type_list (*hesh_func)(const char*) ) {
    char *point_read = text;
    char *end_of_file = text + length - 2;
    while (point_read < end_of_file){
        int read_symbol = 0;
        char str[3000] = {};
        sscanf(point_read, "%s%n", str, &read_symbol);
        point_read += read_symbol;
        if (read_symbol == 0) break;

        T hesh = (*hesh_func)(str);
        int number = (int)(hesh % size_array_list);
        if (/*a[number].searchByValue(hesh) == nullptr*/ 1) { // to leave if you want write all worlds
            a[number].insertInEnd(hesh);
        }
    }
    printf("%p\n", end_of_file);
    printf("%p\n", point_read);

}

long ItLength(FILE* file) {
    assert(file != nullptr);

    fseek(file, 0, SEEK_END);
    long result = ftell(file);
    fseek(file, 0, SEEK_SET);

    return result;
}

char* Buffering (FILE* file, long length) {
    char *text = (char*) calloc (length, sizeof(char));
    assert(text != nullptr);
    long n = fread(text, sizeof(char), length, file);
    text[length - 1] = '\0';
    return text;
}

template <typename T>
void WriteInTable(const char* name,List<T> a[], FILE* table, int size_array_list) {
    fprintf(table, "%s", name);

    for (int i = 0; i < size_array_list; i++){
        fprintf(table, ",%lld" ,a[i].size_);
    }
    fprintf(table, "\n");
}

FILE* OpenToWriteInTable(int size_array_list){
    FILE* table = fopen("table.csv", "wb");
    assert(table != nullptr);

    fprintf(table, "No");
    for (int i = 0; i < size_array_list; i++){
        fprintf(table, ",%d" ,i);
    }
    fprintf(table, "\n");
    return table;
}

template <typename T>
void CleaningStruct (List<T> a[], int size_array_list){
    for (int i = 0; i < size_array_list; ++i) {
        a[i].free_ = (T*)(((element<T>*)(a[i].point_from_calloc[0]))->next_);
        a[i].size_ = 0;
        a[i].head_ = (T*)((element<T>*)(a[i].point_from_calloc[0]));
    }
}

void CloseToWriteInTable(FILE* table){
    fclose(table);
}

template <typename T>
void WordProcessing(List<T> a[], int size_array_list) {
    FILE* file = fopen("voina-i-mir.txt", "rb");
    long length = ItLength(file) + 1;
    char *text = Buffering(file, length);

    FILE* table = OpenToWriteInTable(size_array_list);

    HeshingWords (text, length, a, size_array_list, HeshingFirstSymbol);
    WriteInTable ("HeshingFirstSymbol", a, table, size_array_list);
    CleaningStruct(a, size_array_list);

    printf("First hesh\n");

    HeshingWords (text, length, a, size_array_list, HeshingStringLength);
    WriteInTable ("HeshingStringLength",a, table, size_array_list);
    CleaningStruct(a, size_array_list);

    printf("Second hesh\n");

    HeshingWords (text, length, a, size_array_list, HeshingSumSymbol);
    WriteInTable ("HeshingSumSymbol",a, table, size_array_list);
    CleaningStruct(a, size_array_list);

    printf("Third hesh\n");

    HeshingWords (text, length, a, size_array_list, HeshingRol);
    WriteInTable ("HeshingRol",a, table, size_array_list);
    CleaningStruct(a, size_array_list);

    printf("Fourth hesh\n");

    HeshingWords (text, length, a, size_array_list, HeshingGNU);
    WriteInTable ("HeshingGNU",a, table, size_array_list);
    CleaningStruct(a, size_array_list);

    printf("Fifth hesh\n");

    CloseToWriteInTable(table);
}

type_list HeshingAlwaysOne(const char* str) {
    return 1;
}

type_list HeshingFirstSymbol(const char* str) {
    return  (type_list)(str[0]);
}

type_list HeshingStringLength(const char* str) {
    return strlen(str);
}

type_list HeshingSumSymbol(const char* str) {
    type_list sum = 0;
    int length = strlen(str);
    for (int i = 0; i < length; i++){
        sum +=str[i];
    }
    return sum;
}

type_list HeshingRol(const char* str) {
    type_list hesh = 0;
    int length = strlen(str);
    for (int i = 1; i < length; i++){
        hesh = ((hesh >> 31) + (hesh <<1))^str[i];
    }
    return hesh;
}

type_list HeshingGNU(const char* str) {
    type_list hesh = 0;
    int length = strlen(str);
    for (int i = 0; i < length; i++){
        hesh = hesh*33 + str[i];
    }
    return hesh;
}

unsigned int HeshingMurmurHash(const char text[])
{
    const unsigned int m = 0x5bd1e995;
    const unsigned int seed = 0;
    const int r = 24;

    unsigned int len = strlen(text);

    const unsigned char * data = (const unsigned char *) text;
    unsigned int h = seed ^ len;

    unsigned int k = 0;

    while (len >= 4)
    {
        k  = data[0];
        k |= data[1] << 8;
        k |= data[2] << 16;
        k |= data[3] << 24;

        k *= m;
        k ^= k >> r;
        k *= m;

        h *= m;
        h ^= k;

        data += 4;
        len -= 4;
    }

    switch (len)
    {
        case 3:
            h ^= data[2] << 16;
        case 2:
            h ^= data[1] << 8;
        case 1:
            h ^= data[0];
            h *= m;
    };

    h ^= h >> 13;
    h *= m;
    h ^= h >> 15;

    return h;
}
