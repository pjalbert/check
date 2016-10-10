/*
 *  Networks.c
 *  Idiome
 *
 *  Created by Pierre-Jean ALBERT on Tue Apr 08 2015.
 *  Copyright (c) 2013 __An Intellectual Projet__. All rights reserved.
 *
 */

#include "networks.h"

/**
		 */
int main (int argn, char* argc[]) {
  Networks* pNetworks;
  Listener* pListener;

  pNetworks = (Networks*)InitNetwoks("Kepler - 1.0");
  if (!pNetworks)
  {
    printf("Error Creating Networks - \n");
    exit(-1);
  }

  pListener = (Listener *)CreateListener(pNetworks, "eth0", 9000, IPV4, TCP);
  if (pListener)
    SetEchoProtocol(pListener);

  pListener = (Listener *)CreateListener(pNetworks, "eth0", 9001, IPV4, UDP);
  if (pListener)
    SetEchoProtocol(pListener);


  pListener = CreateListener(pNetworks, "eth0", 8000, IPV4, TCP);
  SetUuidProtocol(pListener);

  pListener = CreateListener(pNetworks, "eth0", 8001, IPV4, UDP);
  SetUuidProtocol(pListener);

  /* pListener = CreateListener(pNetworks, NULL, 7000, IPV4, TCP);
  SetB64Protocol(pListener);

  pListener = CreateListener(pNetworks, NULL, 7001, IPV4, UDP); */

    RunNetworks(pNetworks);

  return 0;
}
