/*
 *  Networks.c
 *  Idiome
 *
 *  Created by Pierre-Jean ALBERT on Tue May 15 2015.
 *  Copyright (c) 2013 __An Intellectual Projet__. All rights reserved.
 *
 */

#include "networks.h"

/**
		 */
int main (int argn, char* argc[]) {
    Networks* pNetworks;
    Connect* pConnect;

    pNetworks = (Networks*)InitNetwoks("Kepler - 1.0");
    if (!pNetworks) {
        printf("Error Creating Networks - \n");
        exit(-1);
    }

    pConnect = (Connect *)CreateConnector(pNetworks, "wwww.lemonde.fr", 80, IPV4, TCP);
/*    LoadHTTPCrawlerProtocol(pConnect, AddRequestFromJson("{ \"Deepless\" : 3, \"Field\" : [\"bali\", \"indonesia\"] }");*/

/*    pCrawl = NULL;
    AddMemberOnTop(&pCrawl, "indonésie");
    AddMemberOnTop(&pCrawl, "bali");
    AddMemberOnTop(&pCrawl, "");

    pConnect = (Connect *)CreateConnector(pNetworks, "wwww.lemonde.fr", 80, IPV4, TCP);
    if (pConnect)
        SetCrawlerProtocol(pConnect, pCrawl, pFilter);


    pConnect = (Connect *)CreateConnector(pNetworks, "wwww.liberation.fr", 80, IPV4, TCP);
    if (pConnect)
        SetCrawlerProtocol(pConnect, pCrawl, pFilter);


    pConnect = (Connect *)CreateConnector(pNetworks, "wwww.huffingtonpost.fr", 80, IPV4, TCP);
    if (pConnect)
        SetCrawlerProtocol(pConnect, pCrawl, pFilter);*/

    RunNetworks(pNetworks);

    return 0;
}
