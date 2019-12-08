#include "util.h"
#include <fstream>
#include <unistd.h>
#include <curl/curl.h>

using json = nlohmann::json;

extern "C" size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream);

#define BUFFER_SIZE 32*1024

struct Buffer {
    Buffer(){
        len = 0;
        data[0] = 0;
    }
    char data[BUFFER_SIZE];
    size_t len;
};



bool festi::canRead(const std::string& filename){
    return access(filename.c_str(),F_OK) == 0;
}

std::string festi::readFile(const std::string& filename){
    static const size_t MAX_FILE_LEN = 16*1024;
    std::ifstream file;
    std::ios_base::iostate exceptionMask = file.exceptions() | std::ios::failbit;
    file.exceptions(exceptionMask);
    file.open(filename,std::ifstream::binary);
    file.seekg(0,file.end);
    size_t length = file.tellg();
    if (length > (MAX_FILE_LEN - 1)) throw std::runtime_error("File too big");
    file.seekg(0,file.beg);
    char buffer[length+1];
    file.read(buffer,length);
    buffer[length] = '\0';
    return buffer;
}

json festi::readJson(const std::string& filename){
    return json::parse(readFile(filename));
}

std::string festi::readUrl(const std::string& url){
    CURL *curl;
    CURLcode res;
    curl = curl_easy_init();
    Buffer buffer;
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);
        res = curl_easy_perform(curl);
        if(res != CURLE_OK) {
            std::string error = curl_easy_strerror(res);
            curl_easy_cleanup(curl);
            throw std::runtime_error(error);
        }
        curl_easy_cleanup(curl);
    } else {
        throw std::runtime_error("Could not initialize curl");
    }
    return buffer.data;
}
nlohmann::json festi::readJsonUrl(const std::string& url){
    return json::parse(readUrl(url));
}


extern "C" size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream){
    size_t len = size * nmemb;
    Buffer& buffer = *((Buffer*)stream);
    if ((buffer.len + len) < BUFFER_SIZE){
        memcpy(buffer.data + buffer.len,ptr,len);
        buffer.len += len;
        buffer.data[buffer.len] = 0;
        return len;
    } else {
        return 0;
    }
}