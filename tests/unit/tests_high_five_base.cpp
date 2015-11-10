/*
 * @file tests_mvd2.cpp
 *
 * @brief unit tests for mvd2 parser
 *
 *
 **/


#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <typeinfo>

#include <highfive/H5File.hpp>
#include <highfive/H5DataSpace.hpp>
#include <highfive/H5DataSet.hpp>

#define BOOST_TEST_MAIN HighFiveTest
#include <boost/test/included/unit_test.hpp>

using namespace HighFive;


template<typename T>
T content_generate(){
    return static_cast<T>(std::rand()*1000);
}

template<>
char content_generate<char>(){
    return char((std::rand()%26)+0x61);
}

template<>
std::string content_generate<std::string>(){
    const size_t size_string = std::rand()%1000;
    std::string random_string;
    random_string.resize(size_string);
    std::generate(random_string.begin(), random_string.end(), content_generate<char>);
    return random_string;
}



BOOST_AUTO_TEST_CASE( HighFiveBasic )
{

    const std::string FILE_NAME("h5tutr_dset.h5");
    const std::string DATASET_NAME("dset");


    // Create a new file using the default property lists.
    File file(FILE_NAME, File::ReadWrite | File::Create | File::Truncate);

    // Create the data space for the dataset.
    std::vector<size_t> dims;
    dims.push_back(4);
    dims.push_back(6);

    DataSpace dataspace(dims);

    // Create a dataset with double precision floating points
    DataSet dataset_double = file.createDataSet(DATASET_NAME + "_double", dataspace, AtomicType<double>());

    {
        BOOST_CHECK_THROW({
            DataSet fail_duplicated = file.createDataSet(DATASET_NAME + "_double", dataspace, AtomicType<double>());
        },
        DataSetException);
    }

    DataSet dataset_size_t = file.createDataSet<size_t>(DATASET_NAME + "_size_t",  dataspace);

}

BOOST_AUTO_TEST_CASE( HighFiveException )
{

    // Create a new file
    File file1("random_file_123", File::ReadWrite | File::Create | File::Truncate);

    BOOST_CHECK_THROW({
    // triggers a file creation conflict
        File file2("random_file_123", File::ReadWrite | File::Create);
     }, FileException);


}



BOOST_AUTO_TEST_CASE( DataTypeEqualSimple )
{

   using namespace HighFive;

    AtomicType<double> d_var;

    AtomicType<size_t> size_var;

    AtomicType<double> d_var_test;

    AtomicType<size_t> size_var_cpy(size_var);


    AtomicType<int> int_var;
    AtomicType<unsigned int> uint_var;

    // check different type matching
    BOOST_CHECK(d_var == d_var_test);
    BOOST_CHECK(d_var != size_var);

    // check type copy matching
    BOOST_CHECK(size_var_cpy == size_var);

    // check sign change not matching
    BOOST_CHECK(int_var != uint_var);

}

BOOST_AUTO_TEST_CASE( DataTypeEqualTakeBack )
{

    using namespace HighFive;

    const std::string FILE_NAME("h5tutr_dset.h5");
    const std::string DATASET_NAME("dset");


    // Create a new file using the default property lists.
    File file(FILE_NAME, File::ReadWrite | File::Create | File::Truncate);

    // Create the data space for the dataset.
    std::vector<size_t> dims;
    dims.push_back(10);
    dims.push_back(1);

    DataSpace dataspace(dims);

    // Create a dataset with double precision floating points
    DataSet dataset = file.createDataSet<size_t>(DATASET_NAME + "_double", dataspace);


    AtomicType<size_t> s;
    AtomicType<double> d;


    BOOST_CHECK(s == dataset.getDataType());
    BOOST_CHECK(d != dataset.getDataType());


}



BOOST_AUTO_TEST_CASE( ReadWriteDataSetDouble )
{

    const std::string FILE_NAME("h5_rw_double_test.h5");
    const std::string DATASET_NAME("dset");
    std::srand(std::time(0));

    const size_t x_size = 100;
    const size_t y_size = 600;

    // Create a new file using the default property lists.
    File file(FILE_NAME, File::ReadWrite | File::Create | File::Truncate);

    // Create the data space for the dataset.
    std::vector<size_t> dims;
    dims.push_back(x_size);
    dims.push_back(y_size);

    DataSpace dataspace(dims);

    // Create a dataset with double precision floating points
    DataSet dataset = file.createDataSet<double>(DATASET_NAME, dataspace);




    double array[x_size][y_size];

    for(int i =0; i < x_size; ++i){
        for(int j =0; j < y_size; ++j){
            array[i][j] = rand();
        }
    }

    dataset.write(array);


    double result[x_size][y_size];

    dataset.read(result);

    for(int i =0; i < x_size; ++i){
        for(int j =0; j < y_size; ++j){
            BOOST_CHECK_EQUAL(result[i][j], array[i][j]);
        }
    }
}



BOOST_AUTO_TEST_CASE( ReadWriteDataSetInteger )
{

    const std::string FILE_NAME("h5_rw_int_test.h5");
    const std::string DATASET_NAME("dset");
    std::srand(std::time(0));

    const size_t x_size = 100;
    const size_t y_size = 600;

    // Create a new file using the default property lists.
    File file(FILE_NAME, File::ReadWrite | File::Create | File::Truncate);

    // Create the data space for the dataset.
    std::vector<size_t> dims;
    dims.push_back(x_size);
    dims.push_back(y_size);

    DataSpace dataspace(dims);

    // Create a dataset with double precision floating points
    DataSet dataset = file.createDataSet<int>(DATASET_NAME, dataspace);




    int array[x_size][y_size];

    for(int i =0; i < x_size; ++i){
        for(int j =0; j < y_size; ++j){
            array[i][j] = int(rand()*1000);
        }
    }

    dataset.write(array);


    int result[x_size][y_size];

    dataset.read(result);

    for(int i =0; i < x_size; ++i){
        for(int j =0; j < y_size; ++j){
            BOOST_CHECK_EQUAL(result[i][j], array[i][j]);
        }
    }
}



template <typename T>
void ReadWriteVectorTest(){
    using namespace HighFive;

    std::ostringstream filename;
    filename << "h5_rw_vec_" << typeid(T).name() << "_test.h5";

    std::srand(std::time(0));
    const size_t x_size = 800;
    const std::string DATASET_NAME("dset");
    typename std::vector<T> vec;


    // Create a new file using the default property lists.
    File file(filename.str(), File::ReadWrite | File::Create | File::Truncate);

    vec.resize(x_size);
    std::generate(vec.begin(), vec.end(), content_generate<T>);

    // Create a dataset with double precision floating points
    DataSet dataset = file.createDataSet(DATASET_NAME, vec);


    dataset.write(vec);

    typename std::vector<T> result;

    dataset.read(result);

    BOOST_CHECK_EQUAL(vec.size(), x_size);
    BOOST_CHECK_EQUAL(result.size(), x_size);

    for(int i =0; i < x_size; ++i){
        //std::cout << result[i] << " " << vec[i] << "  ";
        BOOST_CHECK_EQUAL(result[i], vec[i]);
    }
}

typedef boost::mpl::list<int, unsigned int, long, unsigned long, unsigned char, char, float, double, std::string> dataset_test_types;

BOOST_AUTO_TEST_CASE_TEMPLATE( ReadWriteDataSetVectorInt, T, dataset_test_types)
{
    ReadWriteVectorTest<T>();
}



