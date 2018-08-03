
#include "Downloader.h"
#include "cocos2d.h"
#include <sys/stat.h>
#include <stdio.h>
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
#include <io.h>
#endif

#if defined(CC_TARGET_OS_IPHONE) || defined(CC_TARGET_OS_MAC)
#include <unistd.h>
#endif

#define FILE_MAX_DOWNLOAD_COUNT     (3)
#define FILE_MAX_DOWNLOAD_TIME      (120)

namespace net {

Downloader::Downloader()
    : isDownloading_(false)
    , allDownloaded_(false)
{
    
}

Downloader::~Downloader()
{
    isDownloading_ = false;
    allDownloaded_ = false;
}

void Downloader::Run()
{
    CURL            *curlConn = NULL;
    CURLcode        code = CURLE_GOT_NOTHING;
    long            localFileLength = 0;
    stHeaderInfo    headerInfo;
//    double          downloadSpeed_ = 0.0f;
    
    // global initialize outsize
    // curl_global_init(CURL_GLOBAL_ALL);
    curlConn = curl_easy_init();
    if (curlConn == NULL) {
        return;
    }
    
    curl_easy_setopt(curlConn, CURLOPT_TIMEOUT, FILE_MAX_DOWNLOAD_TIME);
	curl_easy_setopt(curlConn, CURLOPT_FORBID_REUSE, 1);
	curl_easy_setopt(curlConn, CURLOPT_NOSIGNAL, 1);
    curl_easy_setopt(curlConn, CURLOPT_NOPROGRESS, 1L);
    curl_easy_setopt(curlConn, CURLOPT_HEADERFUNCTION, Downloader::_parse_header);
    curl_easy_setopt(curlConn, CURLOPT_HEADERDATA, &headerInfo);
    curl_easy_setopt(curlConn, CURLOPT_WRITEFUNCTION, Downloader::_write_data);
    // debug info
    // curl_easy_setopt(curlConn, CURLOPT_VERBOSE, 1L);
    
    while (downloadingQueue_.size() > 0) {
        isDownloading_ = true;
        stDownloadUrl downloadUrl;
        if (! peekDownloading(downloadUrl)) {
            break;
        }
        
        if (downloadUrl.downloadCount > FILE_MAX_DOWNLOAD_COUNT) {
            downloadUrl.result = download_result_over_count;
            postDownloaded(downloadUrl);
            continue;
        }
        
        FILE *fp = fopen(downloadUrl.outFileName.c_str(), "wb");
        if (fp) {
            headerInfo.reset();
            curl_easy_setopt(curlConn, CURLOPT_URL, downloadUrl.url.c_str());
            
            curl_easy_setopt(curlConn, CURLOPT_WRITEDATA, fp);
            code = curl_easy_perform(curlConn);
            fclose(fp);
            if (code != CURLE_OK) {
                CCLOG("error: could not downdload url:%s to file%s", downloadUrl.url.c_str(), downloadUrl.outFileName.c_str());
                downloadUrl.result = code;
                remove(downloadUrl.outFileName.c_str());
            } else {
                // if not http/1.1 200 OK
                if (headerInfo.httpCode != header_success_code) {
                    downloadUrl.result = headerInfo.httpCode;
                    remove(downloadUrl.outFileName.c_str());
                }
                else {
                    localFileLength = getLocalFileSize(downloadUrl.outFileName.c_str());
                    if (localFileLength != headerInfo.conentLength) {
                        CCLOG("ERROR: download file:%s locallength:%ld is not match remotelength:%ld",
                                       downloadUrl.outFileName.c_str(), localFileLength, headerInfo.conentLength);
                        remove(downloadUrl.outFileName.c_str());
                        downloadUrl.downloadCount++;
                        downloadingQueue_.push_back(stDownloadUrl(downloadUrl.index.c_str(), downloadUrl.url.c_str(), downloadUrl.outFileName.c_str()));
                        continue;
                    }
                    CCLOG("downloaded file:[%s] from url:[%s] remoteFileLength:[%ld] localFileLength[%ld]",
                                   downloadUrl.outFileName.c_str(), downloadUrl.url.c_str(), headerInfo.conentLength, localFileLength);
                    downloadUrl.result = download_success;
                }
            }
            postDownloaded(downloadUrl);
        } else {
            CCLOG("error: could not open download file:%s when downloading url:%s", downloadUrl.outFileName.c_str(), downloadUrl.url.c_str());
            downloadUrl.result = download_couldnt_write_file;
            postDownloaded(downloadUrl);
        }
    }
    
    curl_easy_cleanup(curlConn);
    isDownloading_ = false;
    allDownloaded_ = true;
}

bool Downloader::append(const char *szIndex, const char *szUrl, const char *szOutFileName)
{
    if (NULL == szIndex || NULL == szUrl || NULL == szOutFileName) {
        return false;
    }
    if ('\0' == szIndex[0] || '\0' == szUrl[0] || '\0' == szOutFileName[0]) {
        return false;
    }
    
    downloadingQueue_.push_back(stDownloadUrl(szIndex, szUrl, szOutFileName));
    
    if (!isDownloading_) {
        RunThread();
    }
    
    return true;
}

size_t Downloader::_write_data(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
    return fwrite(ptr, size, nmemb, stream);
}

size_t Downloader::_parse_header(void *ptr, size_t size, size_t nmemb, void *stream)
{
    long len = 0;
    int code = header_not_found_code;
    stHeaderInfo *pHeaderInfo = (stHeaderInfo*)stream;
    if (NULL == pHeaderInfo) {
        return size * nmemb;
    }
    if (sscanf((char*)ptr, "Content-Length: %ld\n", &len)) {
        pHeaderInfo->conentLength = len;
    }
    if (sscanf((char*)ptr, "HTTP/1.1 %d ", &code)) {
        pHeaderInfo->httpCode = code;
    }
    
    return size * nmemb;
}

size_t Downloader::getDownloadedSize()
{
    return resultQueue_.size();
}

bool Downloader::peekDownloaded(stDownloadUrl &result)
{
    if (resultQueue_.size() > 0) {
        result = resultQueue_.front();
        resultQueue_.pop_front();
        return true;
    }
    return false;
}

bool Downloader::peekDownloading(stDownloadUrl &download)
{
    if (downloadingQueue_.size() > 0) {
        download = downloadingQueue_.front();
        downloadingQueue_.pop_front();
        return true;
    }
    return false;
}

bool Downloader::postDownloaded(stDownloadUrl &download)
{
    resultQueue_.push_back(download);
    return true;
}

long Downloader::getLocalFileSize(const char *szFile)
{
    if (NULL == szFile || szFile[0] == '\0') {
        return -1;
    }

	return 0;
//     if (access(szFile, 0) != 0) {
//         return -1;
//     }
    
    struct stat file_info;
    if (stat(szFile, &file_info) == 0) {
        return file_info.st_size;
    }
    
    return -1;
}

}
