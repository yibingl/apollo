#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <memory.h>
#include "toolkit.h"

char * g_cameraId = NULL;

int imgDataHandler(const char* camId, struct ImageFrame* frame, FrameReleaser releaser)
{
    printf("Got image from %s: image ID %d, width %d, heigth %d, timestamp %lld.\n", camId, frame->id, frame->width, frame->height, frame->timestamp);
    releaser(frame);
    return 0;
}
int str_replace(char *src, char sub, char dest){
    int ret = 0;
    char *result = NULL;
    do {
        result = strchr(src, sub);
        if(result){
            result[0] = dest;
            ret++;
        }
    }while (result != NULL);
    return ret;
}

void initSDK()
{
    const char* list;
    initSmarterEyeSDK("192.168.100.100");
    startSmarterEyeSDK();
    do{
        list = getCameraList();
        if(list){
            char *tempList = strdup(list);
            str_replace(tempList, '{', ',');
            str_replace(tempList, '}', ',');
            char *jsonItem = strtok(tempList, ",");
            while (jsonItem != NULL) {
                if(strstr(jsonItem,"\"ID\"") != NULL){
                    char *cameraId = strchr(jsonItem, ':');
                    cameraId += 3;
                    cameraId[strlen(cameraId) - 1] = '\0';
                    printf("Found camera with ID %s\n",cameraId);
                    g_cameraId = strdup(cameraId);
                    break;
                }
                jsonItem = strtok(NULL, ",");
            }
            free(tempList);
        }else{
            sleep(1);
        }
    }while (list == NULL);
    free((void*)list);
}

int main(int argc, char *argv[])
{
    initSDK();
    setFrameHandler(g_cameraId, imgDataHandler);
    setTransferFrameRate(g_cameraId, 6.25);
    startCapture(g_cameraId);
    setTransferFrameRate(g_cameraId, 6.25);

    while (1) {
        pause();
    }
    stopCapture(g_cameraId);
    stopSmarterEyeSDK();
    deinitSmarterEyeSDK();

    return 0;
}
