#include <iostream>
#include <opencv2/opencv.hpp>
#include <fstream>
#include <ctime>

#include <curl/curl.h>

#include <unistd.h>

using namespace std;
using namespace cv;

void good(String path)
{
    vector<String> fn;
    String arg = String(path);
    glob(arg + "/*.jpg", fn, false);

    ofstream good;
    good.open("Good.dat");

    size_t count = fn.size();
    cout << "The count of images in folder is : " << count << endl;

    std::clock_t start;
    double duration;

    start = std::clock();

    for (size_t i = 1; i <= count ; i++)
    {
        cout << "Good" << "\\" << i << ".jpg " << "1 " << "0 " << "0 " << imread(fn[i]).cols << " " << imread(fn[i]).rows << endl;
        good << "Good" << "\\" << i << ".jpg " << "1 " << "0 " << "0 " << imread(fn[i]).cols << " " << imread(fn[i]).rows << endl;
    }


    duration = ( std::clock() - start ) / (double) CLOCKS_PER_SEC;

    std::cout<<"time in seconds: "<< duration <<'\n';

    good.close();
}

void bad(String path)
{
    vector<String> fn;
    String arg = String(path);
    glob(arg + "/*.jpg", fn, false);

    ofstream bad;
    bad.open("Bad.dat");

    size_t count = fn.size();
    cout << "The count of images in folder is : " << count << endl;

    std::clock_t start;
    double duration;

    start = std::clock();

    for (size_t i = 0; i < count ; i++)
    {
        cout << "Bad" << "\\" << i << ".jpg" << endl;
        bad  << "Bad" << "\\" << i << ".jpg" << endl;
    }


    duration = ( std::clock() - start ) / (double) CLOCKS_PER_SEC;

    std::cout<<"time in seconds: "<< duration <<'\n';

    bad.close();
}

size_t write_data(char *ptr, size_t size, size_t nmemb, void *userdata)
{
    vector<uchar> *stream = (vector<uchar>*)userdata;
    size_t count = size * nmemb;
    stream->insert(stream->end(), ptr, ptr + count);
    return count;
}

//Функция получения списка с сайта.
vector<uchar> curlStream(const char *img_url, int timeout=1000)
{
    vector<uchar> stream;
    CURL *curl = curl_easy_init();
    if(curl)
    {
//        cout << curl << endl;
        curl_easy_setopt(curl, CURLOPT_URL, img_url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &stream);
//        curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, timeout);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout);
        CURLcode res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
    }


//вывод полученных данных
//    for(auto i = 0; i < stream.size(); i++)
//        cout << stream[i];

    return stream;
}

//конвертация списка в str
string curlStr(vector<uchar>stream)
{
    unsigned char *uch = &*stream.begin();
    string str;
    str.append(reinterpret_cast<const char*>(uch));
    return str;
}

//конвертация в img
Mat curlImg(vector<uchar> img)
{
    if(!img.empty())
    return imdecode(img, -1);
}

char* appendCharToCharArray(char* array, char a)
{
    size_t len = strlen(array);

    char* ret = new char[len+2];

    strcpy(ret, array);
    ret[len] = a;
    ret[len+1] = '\0';

    return ret;
}

int main(int argc, char** argv) {
    std::cout << "dat to train" << std::endl;

//    good(argv[1]);
//    bad(argv[2]);

    // сылка на общей список
    vector<uchar> vec_uch = curlStream("http://www.image-net.org/api/text/imagenet.synset.geturls?wnid=n04556533");

    string str = curlStr(vec_uch);

//    cout << str << endl;

    // Парсинг списка
    string start = "http";
    string end = "jpg";

    for(int k = 0 ; k < 10; k++)
    {
        unsigned first = str.find(start);
        unsigned last = str.find(end);

        //Сылку которую будем использовать
        string strNew = str.substr(first, last + end.size());
        cout << "STR  : " << strNew << endl;

        //Удаление из списка используемого элемента
        str.erase(str.begin(), str.begin() + strNew.size() + 1);

        //Конвертация в char
        char * charNew = const_cast<char *>(strNew.c_str());
        cout << "CHAR : " << charNew << endl;

        char buf = '\0';
        int ll = 0;
//        while(*(charNew + ll) != 0)
//        {
//            cout << int(charNew[ll++]) << " ";
//        }

        if(int(charNew[strlen(charNew) - 1]) == 13)
        {
//            cout << int(charNew[strlen(charNew) - 1]) << endl;
            charNew[strlen(charNew) - 1] = '\0';
            cout << charNew << endl;
        }

//        while(*(charNew + ll) != 0)
//        {
//            cout << int(charNew[ll++]) << " ";
//        }


        vector<uchar>uimg = curlStream(charNew);//"https://i.pinimgcom/736x/56/37/e0/5637e06fe6fa24e028db8d3513cb4dbc--cartier-watches-women-ladies-watches.jpg"

        for (auto iter = uimg.begin();
             iter != uimg.end(); ++iter)
        cout << *iter;


//        Mat img = curlImg(uimg);

//        for (auto iter = uimg.begin();
//            iter != uimg.end(); ++iter)
//            cout << *iter;

        if(uimg.size() > 0)
            cout << uimg.size() << endl;



        Mat img = curlImg(uimg);
        if(!img.empty()) {
            imshow((k), img);
        }else{
            continue;
        }

//        if(!img.empty())
//        {
//            imshow("Img", img);
//        } else
//        {
//            cout << "LOL : " << k << endl;
//            continue;
//        }


        cout << "===========================================" << endl;
    }

    cvWaitKey(0);

    return 0;
}