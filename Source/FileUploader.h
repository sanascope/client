//
// Created by Gerard Castillo Lasheras on 2019-06-05.
//

#ifndef ANDROID_FILEUPLOADER_H
#define ANDROID_FILEUPLOADER_H


//
// Created by Gerard Castillo Lasheras on 2019-06-05.
// based on https://forum.juce.com/t/file-upload-with-progress-window-thread/33023/5
//

#include "FileUploader.h"

class FileUploader : public ThreadWithProgressWindow {
public:
    FileUploader(File file_to_upload, String host_name)
            : ThreadWithProgressWindow("Uploading file " + file_to_upload.getFileName(), true, true, 1000, "Cancel")
            , file_to_upload_(file_to_upload)
            , host_name_(host_name)
    {
    }

    virtual void run() override {

        if (!file_to_upload_.existsAsFile()){
            response_str = "Upload file does not exist.";
            return;
        }

        // open a URL connection to you Rest server.
        String url_str(host_name_ + "/files");
        URL url(url_str);

        url = url.withFileToUpload("file", file_to_upload_, "application/octet-stream");

        Utils::log("Uploading to: " + url.getDomain());

        URL::OpenStreamProgressCallback* callback = &FileUploader::ProgressCallback;

        Utils::log("Starting Streaming...");

        InputStream* input = url.createInputStream(true, callback, this);

//        String result_post = input->readEntireStreamAsString();
//        Utils::log("result post: " + result_post);
        response_str += String("one upload done");

    }

    static bool ProgressCallback(void *context, int bytesSent, int totalBytes){
        double progress_value = bytesSent / ((double)totalBytes);
        Utils::log("progress: " + String(progress_value));
        if (static_cast<FileUploader*>(context)->currentThreadShouldExit())
            return false;
        static_cast<FileUploader*>(context)->setProgress(progress_value);
        return true;
    }

    String getResponseString() {
        ScopedLock l(responseLock);
        return response_str;
    }



protected:

    File file_to_upload_;
    String host_name_;
    String response_str = "ok";
    CriticalSection responseLock;

};


#endif //ANDROID_FILEUPLOADER_H
