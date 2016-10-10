/*
 *  iomanage.c
 *  Idiome
 *
 *  Created by Pierre-Jean ALBERT on Mon Sep 08 2014.
 *  Copyright (c) 2014 __An Intellectual Projet__. All rights reserved.
 *
 */
#include "iomanage.h"

int main(int argn, char* argc[]) {
    IOManage* pIOManage;
    IOEntity* pIOEntity;
    int rc, Cnt;


    Cnt = 0;
    printf("My process ID : %d\n", getpid());

    sleep(10);

    pIOEntity = IOEntityFile("dump.log", NULL, 0, false, 'r');

    pIOManage = CreateIOManager(pIOEntity, "Test MultiFile");
    if (!pIOManage)
        return 0;

    rc = IOManagerAddEntity(pIOManage, IOEntityFile("duplicate.0.thread.txt", NULL, 1, false, 'w'));
    rc = IOManagerAddEntity(pIOManage, IOEntityFile("b64.0.thread.txt", IOBase64, 1, false, 'w'));
    rc = IOManagerAddEntity(pIOManage, IOEntityFile("crypto.0.thread.txt", IOCrypt, 1, false, 'w'));
    //rc = IOManagerAddEntity(pIOManage, IOEntityScreen(NULL, 1, false));

    LaunchIOManager(pIOManage);

    FreeThSafeIOManage(pIOManage);

    pIOEntity = IOEntityFile("crypto.0.thread.txt", NULL, 0, false, 'r');


    pIOManage = CreateIOManager(pIOEntity, "Test MultiFile");
    if (!pIOManage)
        return 0;

    rc = IOManagerAddEntity(pIOManage, IOEntityFile("b64.crypto.thread.txt", IOBase64, 1, false, 'w'));
    rc = IOManagerAddEntity(pIOManage, IOEntityFile("uncrypto.4.thread.txt", IODCrypt, 1, false, 'w'));

    LaunchIOManager(pIOManage);
    FreeThSafeIOManage(pIOManage);

    printf("Step 5.%d\n", Cnt++);

    return 1;
}
