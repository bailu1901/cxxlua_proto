

#ifndef GodWings_Downloader_h
#define GodWings_Downloader_h

#include <string>
#include <deque>
#include "Thread_t.h"
#include <curl/curl.h>

namespace net {

enum enumDownloadResult
{
    download_result_over_count = -2,
    download_couldnt_write_file = -1,
    download_success = 0,
    
    header_success_code = 200,
    header_not_found_code = 404,
};

struct stHeaderInfo
{
    stHeaderInfo()
    : conentLength(0), httpCode(header_not_found_code)
    {}
    
    long conentLength;
    short httpCode;
    
    void reset()
    {
        conentLength = 0;
        httpCode = header_not_found_code;
    }
};

struct stDownloadUrl
{
    stDownloadUrl()
    {}
    stDownloadUrl(const char *szIndex, const char *szUrl, const char *szFileName)
    : result(-1), downloadCount(0)
    {
        this->index.assign(szIndex);
        this->url.assign(szUrl);
        this->outFileName.assign(szFileName);
    }
    int         result;
    int         downloadCount;
    long        fileSize;
    std::string index;
    std::string url;
    std::string outFileName;
    
    stDownloadUrl& operator = (const stDownloadUrl& src)
    {
        this->result = src.result;
        this->downloadCount = src.downloadCount;
        this->index = src.index;
        this->url = src.url;
        this->outFileName = src.outFileName;
        return *this;
    }
};

typedef std::deque<stDownloadUrl> DownloadQueue_t;

class Downloader : public thread_ex
{
public:
    Downloader();
    ~Downloader();
    
protected:
    virtual void Run();
    
public:
    bool append(const char *szIndex, const char *szUrl, const char *szOutFileName);
    
    size_t getDownloadedSize();
    bool peekDownloaded(stDownloadUrl &result);
    
    bool allDownloaded() {return allDownloaded_;}

protected:
    static size_t _write_data(void *ptr, size_t size, size_t nmemb, FILE *stream);
    static size_t _parse_header(void *ptr, size_t size, size_t nmemb, void *stream);
    
    bool peekDownloading(stDownloadUrl &download);
    
    bool postDownloaded(stDownloadUrl &download);
    
    long getLocalFileSize(const char *szFile);
    
private:
    DownloadQueue_t downloadingQueue_;
    DownloadQueue_t resultQueue_;
    
    bool            isDownloading_;
    bool            allDownloaded_;
};

}

#endif
